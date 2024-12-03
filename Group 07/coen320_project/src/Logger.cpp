#include "Logger.h"
#include <iostream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <sstream>

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() : isLoggingActive(true) {

    logFile.open("log.txt", std::ios::app);
    graphFile.open("graph.txt", std::ios::app);

    if (!logFile.is_open() || !graphFile.is_open()) {
        std::cerr << "Failed to open log or graph file!" << std::endl;
        exit(1);
    }

    logThread = std::thread(&Logger::logWorker, this);
}

Logger::~Logger() {
    stop();
}

void Logger::log(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);  // Ensure thread-safety

    std::string logMessage = getTimestamp() + " - " + message;
    logQueue.push(logMessage);

    cv.notify_one();
}

void Logger::writeGraph(const std::string& graphData) {
    std::lock_guard<std::mutex> lock(logMutex);  // Ensure thread-safety

    graphFile << graphData << std::endl;
}

std::string Logger::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time);

    // Format timestamp as "YYYY-MM-DD HH:MM:SS"
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);

    return std::string(buffer);
}

void Logger::logWorker() {
    while (isLoggingActive) {
        std::string logMessage;

        {
            std::unique_lock<std::mutex> lock(logMutex);

            cv.wait(lock, [this] { return !logQueue.empty() || !isLoggingActive; });

            if (!logQueue.empty()) {
                logMessage = logQueue.front();
                logQueue.pop();
            }
        }

        if (!logMessage.empty()) {
            logFile << logMessage << std::endl;
        }
    }
}

void Logger::stop() {
    isLoggingActive = false;

    cv.notify_one();
    if (logThread.joinable()) {
        logThread.join();
    }

    if (logFile.is_open()) {
        logFile.close();
    }
    if (graphFile.is_open()) {
        graphFile.close();
    }
}
