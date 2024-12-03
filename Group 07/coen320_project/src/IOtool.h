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
#include"Plane.h"
typedef struct {
    int flight_id;         
    int velocity_x;        
    int velocity_y;        
    int velocity_z;        
    int Position_x;        
    int Position_y;        
    int Position_z;        
    int t_init;            
    int current_t;         
    timer_t timer_id;      
} TaskArgs;

class IOtool {
public:
    IOtool() = default;

    void readPlanesFromFile(const std::string& filename, std::vector<Plane*>* planes);
};

#endif // IOTOOL_H
