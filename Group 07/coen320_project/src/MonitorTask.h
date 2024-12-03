#ifndef MONITORTASK_H
#define MONITORTASK_H

#include <ctime>
#include <iostream>
#include <vector>
#include "Plane.h"

#include "Logger.h"
class MonitorTask {
public:
    static void handler(union sigval arg) {
        std::vector<Plane*>* monitor = (std::vector<Plane*>*)arg.sival_ptr;

        //std::cout << "\n--- Current Displacements ---\n";
        for (auto* it : *monitor) {
        	std::string plane_info = "Plane " + std::to_string(it->flight_id) + ": (" + std::to_string(it->Position_x) + "," + std::to_string(it->Position_y) + "," + std::to_string(it->Position_z) + ")";

            //Logger::getInstance().log("MonitorTask:"+plane_info);
            //std::cout << "Plane " << it->flight_id << ": (" << it->Position_x << "," << it->Position_y << "," << it->Position_z << ")\n";
        }
        //std::cout << "-----------------------------\n";
    }

    static int create_task(std::vector<Plane*>& tasks) {
        struct sigevent sev;
        struct itimerspec its;
        timer_t monitor_timer;

        sev.sigev_notify = SIGEV_THREAD;
        sev.sigev_value.sival_ptr = &tasks;
        sev.sigev_notify_function = handler;
        sev.sigev_notify_attributes = NULL;

        if (timer_create(CLOCK_REALTIME, &sev, &monitor_timer) == -1) {
            perror("Failed to create monitor timer");
            return -1;
        }

        its.it_value.tv_sec = 30;
        its.it_value.tv_nsec = 0;
        its.it_interval.tv_sec = 30;
        its.it_interval.tv_nsec = 0;

        if (timer_settime(monitor_timer, 0, &its, NULL) == -1) {
            perror("Failed to start monitor timer");
            return -1;
        }

        return 0;
    }


    static int create_polling_task(std::vector<Plane*>& tasks) {
        struct sigevent sev;
        struct itimerspec its;
        timer_t monitor_timer;

        sev.sigev_notify = SIGEV_THREAD;
        sev.sigev_value.sival_ptr = &tasks;
        sev.sigev_notify_function = handler;
        sev.sigev_notify_attributes = NULL;

        if (timer_create(CLOCK_REALTIME, &sev, &monitor_timer) == -1) {
            perror("Failed to create monitor timer");
            return -1;
        }

        its.it_value.tv_sec = 1;
        its.it_value.tv_nsec = 0;
        its.it_interval.tv_sec = 0;
        its.it_interval.tv_nsec = 0;

        if (timer_settime(monitor_timer, 0, &its, NULL) == -1) {
            perror("Failed to start monitor timer");
            return -1;
        }

        return 0;
    }

};

#endif // MONITORTASK_H
