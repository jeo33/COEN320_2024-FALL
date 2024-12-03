#include "CommunicationSystem.h"
#include <cstring>
#include <sstream>
#include <iostream>

// Constructor
CommunicationSystem::CommunicationSystem() {
}

// Destructor
CommunicationSystem::~CommunicationSystem() {
}

void CommunicationSystem::task_handler(union sigval arg) {
    Command* c_craft = (Command*)(arg.sival_ptr);

	std::cout<<"Handler "<<c_craft->aircraft_id<<std::endl;
	std::cout<<"Handler "<<c_craft->vx<<std::endl;
	std::cout<<"Handler "<<c_craft->vy<<std::endl;
	std::cout<<"Handler "<<c_craft->vz<<std::endl;
    for (Plane* plane : c_craft->planes) {  // Access planes from the Command structure
        if (plane->flight_id == c_craft->aircraft_id) {
            int coid = ConnectAttach(0, 0, plane->chid, 0, 0);
            if (coid == -1) {
                perror("ConnectAttach failed");
                return;
            }

            msg_struct msg;
            std::memset(&msg, 0, sizeof(msg));
            msg.label='V';
            msg.x = c_craft->vx;
            msg.y = c_craft->vy;
            msg.z = c_craft->vz;
            msg.id = c_craft->aircraft_id;

            msg_struct reply;
            int status = MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply));
            if (status == -1) {
                perror("MsgSend failed");
                return;
            }

            std::cout << "Client: Received reply: " << reply.body << std::endl;
            if (ConnectDetach(coid) == -1) {
                perror("ConnectDetach failed");
            }
            return; 
        }
    }

   
    Logger::getInstance().log("Command failed: No aircraft with ID " + std::to_string(c_craft->aircraft_id));
}

int CommunicationSystem::send(Command* c) {
    struct sigevent sev;
    struct itimerspec its;
    timer_t monitor_timer;

	std::cout<<"send "<<c->aircraft_id<<std::endl;
	std::cout<<"send "<<c->vx<<std::endl;
	std::cout<<"send "<<c->vy<<std::endl;
	std::cout<<"send "<<c->vz<<std::endl;
    std::memset(&sev, 0, sizeof(sev));
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_value.sival_ptr = c;  
    sev.sigev_notify_function = task_handler;
    sev.sigev_notify_attributes = nullptr;

    if (timer_create(CLOCK_REALTIME, &sev, &monitor_timer) == -1) {
        perror("Failed to create monitor timer");
        return -1;
    }

    its.it_value.tv_sec = 1;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 0; // No repeat
    its.it_interval.tv_nsec = 0;

    if (timer_settime(monitor_timer, 0, &its, nullptr) == -1) {
        perror("Failed to start monitor timer");
        return -1;
    }

    return 0; 
}


void CommunicationSystem::executeCommand(const std::string& userInput, std::vector<Plane*> &planes) {
    processCommand(userInput, planes);
}

void CommunicationSystem::processCommand(const std::string& userInput, std::vector<Plane*>& planes) {
	std::stringstream ss(userInput);
	std::string commandType;
	std::getline(ss, commandType, '#');  // Extract the first part before '#'
	std::cout << "User Input: " << userInput << std::endl;

	if (commandType == "V") {
	    int flight_id, vx, vy, vz;

	    std::string part;

	    std::getline(ss, part, '#');
	    flight_id = std::stoi(part); 
	    std::cout << "flight_id: " << flight_id << std::endl;

	    std::getline(ss, part, '#');
	    vx = std::stoi(part); 
	    std::cout << "vx: " << vx << std::endl;

	    std::getline(ss, part, '#');
	    vy = std::stoi(part); 
	    std::cout << "vy: " << vy << std::endl;

	    std::getline(ss, part, '#');
	    vz = std::stoi(part);  
	    std::cout << "vz: " << vz << std::endl;

	    handleVCommand(flight_id, vx, vy, vz, planes);
	}
	else {
	    std::cout << "Unknown command type!" << std::endl;
	}
}

// Handle the `v#flight_id#vx#vy#vz` command
void CommunicationSystem::handleVCommand(int flight_id, int vx, int vy, int vz,std::vector<Plane*>& planes) {

	std::cout<<flight_id<<std::endl;
	std::cout<<"vx "<<vx<<std::endl;
	std::cout<<"vy "<<vy<<std::endl;
	std::cout<<"vz "<<vz<<std::endl;
    Command cmd;
    cmd.label = 'V';  
    cmd.aircraft_id = flight_id;
    cmd.vx = vx;
    cmd.vy = vy;
    cmd.vz = vz;
    cmd.planes = planes;

    // Log the command and send it
    Logger::getInstance().log("Executing Velocity Command: Flight ID " + std::to_string(flight_id) + " vx=" + std::to_string(vx) + " vy=" + std::to_string(vy) + " vz=" + std::to_string(vz));
    if (send(cmd.deepCopy()) == -1) {
        Logger::getInstance().log("Command failed to execute: Velocity command");
    }
}

