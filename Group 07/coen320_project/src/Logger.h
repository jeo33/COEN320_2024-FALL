/*
 * Logger.h
 *
 *  Created on: Nov 20, 2024
 *      Author: j_gai
 */
#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>
#include <chrono>
#include <queue>
#include <condition_variable>
#include <string>
#include <atomic>

class Logger {
public:
    static Logger& getInstance();

    void log(const std::string& message);

    void writeGraph(const std::string& graphData);

    void stop();

private:

    Logger();
    ~Logger();

    void logWorker();

    std::string getTimestamp();

    std::ofstream logFile;

    std::ofstream graphFile;

    std::mutex logMutex;
    std::queue<std::string> logQueue;
    std::condition_variable cv;

    std::thread logThread;

    std::atomic<bool> isLoggingActive;
};

#endif /* LOGGER_H_ */
