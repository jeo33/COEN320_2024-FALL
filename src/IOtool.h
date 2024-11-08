/*
 * IOtool.h
 *
 *  Created on: Nov. 4, 2024
 *      Author: gjp
 */

#ifndef IOTOOL_H
#define IOTOOL_H
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <signal.h>
#include <sys/dispatch.h>
typedef struct {
    timer_t timer_id;
    int flight_id;         // Unique identifier for the plane
    int velocity_x;       // Velocity in x direction
    int velocity_y;       // Velocity in y direction
    int velocity_z;       // Velocity in z direction
    int Position_x;       // Current x position
    int Position_y;       // Current y position
    int Position_z;       // Current z position
    int t_init;           // Initial time for initialization
    int current_t;        // Current time tracker
    int chid; // Channel ID
} TaskArgs;
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

class IOtool {
public:
    // Constructor
    IOtool() = default;

    // Method to read planes from a file
    void readPlanesFromFile(const std::string& filename, std::vector<TaskArgs*>* planes);
};

#endif // IOTOOL_H
