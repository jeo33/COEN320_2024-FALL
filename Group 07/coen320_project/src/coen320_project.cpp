#include <iostream>
#include <vector>
#include "IOtool.h"
#include "Plane.h"
#include "MonitorTask.h"
#include "RadarTask.h"
#include "SurveillanceRadar.h"
#include "ComputerSystem.h"
#include "Logger.h"
#include"CommunicationSystem.h"

int main() {
    std::vector<Plane*> planes;
    IOtool ioTool;
    ioTool.readPlanesFromFile("planes.txt", &planes);
    std::vector<Plane*>* planes_ptr = &planes;
    for (auto* plane : *planes_ptr) {
        Logger::getInstance().log(plane->to_cstring());
        plane->start_thread();
        SurveillanceRadar::create_task(plane);
    }
    if (RadarTask::create_task(planes_ptr) != 0) return EXIT_FAILURE;
    if (ComputerSystem::create_task(planes_ptr) != 0) return EXIT_FAILURE;
//
    CommunicationSystem console;
    while (true) {
        std::string userInput;
        std::cout << "Enter a command: ";
        std::getline(std::cin, userInput);
        if(userInput[0]=='R'){
        	if(userInput[1]=='A'){
    	    	if (RadarTask::create_task(planes_ptr) != 0) return EXIT_FAILURE;
        	}
        	else{
				std::stringstream ss(userInput);
				std::string commandType;
				std::string parameter;
				std::getline(ss, commandType, '#');
				std::getline(ss, parameter);
				int flight_id = std::stoi(parameter);
				std::cout << "Command: " << commandType << std::endl;
				std::cout << "Parameter: " << flight_id << std::endl;
			    for (auto* plane :  *planes_ptr) {
			    	if(plane->flight_id==flight_id){
						if (RadarTask::create_polling_task(plane) != 0) return EXIT_FAILURE;
			    	}
			    }
        	}
        }
        else if(userInput[0]=='P'){
        	if(userInput[1]=='A'){
        	    for (auto* plane : *planes_ptr) {
        	    	if (SurveillanceRadar::create_polling_task(plane) != 0) return EXIT_FAILURE;
        	    }
        	}
        	else{
				std::stringstream ss(userInput);
				std::string commandType;
				std::string parameter;
				std::getline(ss, commandType, '#');
				std::getline(ss, parameter);
				int flight_id = std::stoi(parameter);
				std::cout << "Command: " << commandType << std::endl;
				std::cout << "Parameter: " << flight_id << std::endl;
			    for (auto* plane :  *planes_ptr) {
			    	if(plane->flight_id==flight_id){
						if (SurveillanceRadar::create_polling_task(plane) != 0) return EXIT_FAILURE;
			    	}
			    }
        	}
        }
        else{
            console.executeCommand(userInput, planes);
        }
        if (userInput == "exit") break;
    }
    for(;;){};
    for (auto* it : planes) {
        delete it;
    }

    return 0;
}
