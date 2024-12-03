#ifndef RADARTASK_H
#define RADARTASK_H
#include <cmath>  // For std::ceil() function
#include <iostream>
#include <vector>
#include "Plane.h"

#include "Logger.h"
#define SCALE 1000
#define x_max	100000
#define y_max	100000
#define z_max	25000
class RadarTask {
public:

    static int create_task(std::vector<Plane*>* tasks) {
        struct sigevent sev;
        struct itimerspec its;
        timer_t radar_timer;

        sev.sigev_notify = SIGEV_THREAD;
        sev.sigev_value.sival_ptr = tasks;
        sev.sigev_notify_function = scale_down_handler;
        sev.sigev_notify_attributes = NULL;

        if (timer_create(CLOCK_REALTIME, &sev, &radar_timer) == -1) {
            perror("Failed to create radar timer");
            return -1;
        }

        its.it_value.tv_sec = 10;
        its.it_value.tv_nsec = 0;
        its.it_interval.tv_sec = 10;
        its.it_interval.tv_nsec = 0;

        if (timer_settime(radar_timer, 0, &its, NULL) == -1) {
            perror("Failed to start radar timer");
            return -1;
        }

        return 0;
    }



    static int communicate(std::vector<Plane*>& tasks) {
            for (auto& p : tasks) {
                int cid = p->chid;
                std::cout << "Communicating with Plane " << p->flight_id << " at channel ID: " << cid << std::endl;

                msg_struct msg; // Message structure
                msg.id = p->flight_id; // Set the plane ID
                strcpy(msg.body, "Hello from Radar Task!");
                std::cout << "Sending message to Plane " << p->flight_id << ": " << msg.body << std::endl;

                msg_struct reply;
                int status = MsgSend(cid, &msg, sizeof(msg), &reply, sizeof(reply));
                while(status==-1){
                	status = MsgSend(cid, &msg, sizeof(msg), &reply, sizeof(reply));
                }

                std::cout << "Received reply: " << reply.body << std::endl;
            }
            return 0;
        }




    static void scale_down_handler(union sigval arg) {
           std::vector<Plane*>* radar = (std::vector<Plane*>*)arg.sival_ptr;

           int scaled_x_max = x_max / SCALE;
           int scaled_y_max = y_max / SCALE;
           char** tem = new char*[scaled_x_max];
           for (int i = 0; i < scaled_x_max; i++) {
               tem[i] = new char[scaled_y_max];
           }

           for (int i = 0; i < scaled_x_max; i++) {
               for (int j = 0; j < scaled_y_max; j++) {
                   tem[i][j] = ' ';
               }
           }

           for (auto& it : *radar) {
               int scaled_x = std::ceil(it->Position_x / (double)SCALE);  // Use ceil for rounding up
               int scaled_y = std::ceil(it->Position_y / (double)SCALE);  // Use ceil for rounding up


               if (scaled_x >= 0 && scaled_x < scaled_x_max && scaled_y >= 0 && scaled_y < scaled_y_max) {
                   tem[scaled_x][scaled_y] = '*';
               }
           }

           std::string map_string = "---------------------------------------------------------------\n";
           for (int i = 0; i < scaled_x_max; i++) {
               for (int j = scaled_y_max - 1; j >= 0; j--) {
                   map_string += tem[i][j]; // Append each character to the string
               }
               map_string += "\n";
           }
           map_string += "---------------------------------------------------------------\n";

           Logger::getInstance().writeGraph(map_string);

           for (int i = 0; i < scaled_x_max; i++) {
               delete[] tem[i];
           }
           delete[] tem;
       }

    static void polling_scale_down_handler(union sigval arg) {
           Plane* radar = (Plane*)arg.sival_ptr;

           int scaled_x_max = x_max / SCALE;
           int scaled_y_max = y_max / SCALE;

           char** tem = new char*[scaled_x_max];
           for (int i = 0; i < scaled_x_max; i++) {
               tem[i] = new char[scaled_y_max];
           }

           for (int i = 0; i < scaled_x_max; i++) {
               for (int j = 0; j < scaled_y_max; j++) {
                   tem[i][j] = ' ';
               }
           }

               int scaled_x = std::ceil(radar->Position_x / (double)SCALE);  // Use ceil for rounding up
               int scaled_y = std::ceil(radar->Position_y / (double)SCALE);  // Use ceil for rounding up


               if (scaled_x >= 0 && scaled_x < scaled_x_max && scaled_y >= 0 && scaled_y < scaled_y_max) {
                   tem[scaled_x][scaled_y] = '*';

           }

           std::cout<<"Flight:"<<radar->flight_id<<std::endl;
           std::string map_string = "---------------------------------------------------------------\n";
           for (int i = 0; i < scaled_x_max; i++) {
               for (int j = scaled_y_max - 1; j >= 0; j--) {
                   map_string += tem[i][j];
               }
               map_string += "\n";
           }
           map_string += "---------------------------------------------------------------\n";

           std::cout<<map_string<<std::endl;

           // Free the dynamically allocated memory
           for (int i = 0; i < scaled_x_max; i++) {
               delete[] tem[i];
           }
           delete[] tem;
       }

    static int create_polling_task( Plane* tasks) {
        struct sigevent sev;
        struct itimerspec its;
        timer_t radar_timer;

        sev.sigev_notify = SIGEV_THREAD;
        sev.sigev_value.sival_ptr = tasks;
        sev.sigev_notify_function = polling_scale_down_handler;
        sev.sigev_notify_attributes = NULL;

        if (timer_create(CLOCK_REALTIME, &sev, &radar_timer) == -1) {
            perror("Failed to create radar timer");
            return -1;
        }

        its.it_value.tv_sec = 1;
        its.it_value.tv_nsec = 0;
        its.it_interval.tv_sec = 0;
        its.it_interval.tv_nsec = 0;

        if (timer_settime(radar_timer, 0, &its, NULL) == -1) {
            perror("Failed to start radar timer");
            return -1;
        }

        return 0;
    }

};

#endif // RADARTASK_H
