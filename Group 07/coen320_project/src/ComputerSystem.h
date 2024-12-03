/*
 * ComputerSystem.h
 *
 *  Created on: Nov 20, 2024
 *      Author: j_gai
 */

#ifndef SRC_COMPUTERSYSTEM_H_
#define SRC_COMPUTERSYSTEM_H_

#include "Logger.h"
#include<string>
#include <iostream>
#include <vector>
#include <cmath>
#include "Plane.h"

using namespace std;
class ComputerSystem {
public:
    ComputerSystem();
    virtual ~ComputerSystem();

    static void task_handler(union sigval arg) {
        std::vector<Plane*>* monitor = (std::vector<Plane*>*)arg.sival_ptr;
        check_planes_for_collisions(monitor);
    }
    static void alert(Plane* plane,string alert_message){
        int coid = ConnectAttach(0, 0, plane->chid, 0, 0);  // Node 0, PID 0 (any process), Channel ID 1
               if (coid == -1) {
                   perror("ConnectAttach failed");
                   return ;
               }
               msg_struct msg;
               strcpy(msg.body, alert_message.c_str());
               msg.label='A';
               msg.id = plane->flight_id;
               msg_struct reply;

               int status = MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply));
               if (status == -1) {
                   perror("Computer");
                   std:cout<<"Computer"<<std::endl;
                   return ;
               }

              ConnectDetach(coid);
    }
    static int create_task(std::vector<Plane*>* tasks) {
        struct sigevent sev;
        struct itimerspec its;
        timer_t monitor_timer;

        sev.sigev_notify = SIGEV_THREAD;
        sev.sigev_value.sival_ptr = tasks;
        sev.sigev_notify_function = task_handler;
        sev.sigev_notify_attributes = NULL;

        if (timer_create(CLOCK_REALTIME, &sev, &monitor_timer) == -1) {
            perror("Failed to create monitor timer");
            return -1;
        }

        its.it_value.tv_sec = 5;
        its.it_value.tv_nsec = 0;
        its.it_interval.tv_sec = 5;
        its.it_interval.tv_nsec = 0;

        if (timer_settime(monitor_timer, 0, &its, NULL) == -1) {
            perror("Failed to start monitor timer");
            return -1;
        }

        return 0;
    }

    static bool check_collision(const Plane& plane1, const Plane& plane2) {

        int height_diff = std::abs(plane1.Position_z - plane2.Position_z);
        int horizontal_dist = std::sqrt(std::pow(plane1.Position_x - plane2.Position_x, 2) +
                                        std::pow(plane1.Position_y - plane2.Position_y, 2));

        if (height_diff < 1000 && horizontal_dist < 3000) {
            return true;  // Collision detected
        }

        return false;  // No collision
    }


    static void check_planes_for_collisions(std::vector<Plane*>* monitor) {
        for (size_t i = 0; i < monitor->size(); ++i) {
            for (size_t j = i + 1; j < monitor->size(); ++j) {
                if (check_collision(*monitor->at(i), *monitor->at(j))) {
                	std::string message = "Collision detected between Plane " + std::to_string(monitor->at(i)->flight_id)
                	                     + " and Plane " + std::to_string(monitor->at(j)->flight_id);

                    Logger::getInstance().log("ALERT:"+message);
                    std::cout<<"ALERT:"+message<<std::endl;
                    alert(monitor->at(i),message);
                    alert(monitor->at(j),message);
                   // monitor->at(i)->display();
                    //monitor->at(j)->display();
                }
            }
        }
    }
};



#endif /* SRC_COMPUTERSYSTEM_H_ */
