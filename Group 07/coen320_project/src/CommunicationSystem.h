#ifndef SRC_COMMUNICATIONSYSTEM_H_
#define SRC_COMMUNICATIONSYSTEM_H_

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <sys/neutrino.h>
#include <ctime>
#include "Plane.h"
#include "Logger.h"

struct Command {
    char label;
    std::vector<Plane*> planes;  // List of planes to operate on
    int aircraft_id;              // Target aircraft ID
    int vx, vy, vz;               // Velocity in the x, y, z directions
    bool all = false;

    // Deep copy function
    Command* deepCopy() const {
        Command* newCmd = new Command();
        newCmd->label = this->label;
        newCmd->aircraft_id = this->aircraft_id;
        newCmd->vx = this->vx;
        newCmd->vy = this->vy;
        newCmd->vz = this->vz;
        newCmd->all = this->all;

        newCmd->planes = this->planes;

        return newCmd;
    }
};

class CommunicationSystem {
public:
    CommunicationSystem();
    virtual ~CommunicationSystem();

    static void task_handler(union sigval arg);
    static int send(Command* c);
    void executeCommand(const std::string& userInput, std::vector<Plane*>& planes);
    void processCommand(const std::string& userInput, std::vector<Plane*>& planes);
    void handleVCommand(int flight_id, int vx, int vy, int vz,std::vector<Plane*>& planes);
    void handlePCommand(const std::string& parameter);
    void handleRCommand(const std::string& parameter);

private:

};

#endif /* SRC_COMMUNICATIONSYSTEM_H_ */
