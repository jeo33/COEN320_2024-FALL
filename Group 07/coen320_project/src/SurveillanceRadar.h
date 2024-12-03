/*
 * SurveillanceRadar.h
 *
 *  Created on: Nov 20, 2024
 *      Author: j_gai
 */

#ifndef SRC_SURVEILLANCERADAR_H_
#define SRC_SURVEILLANCERADAR_H_

#include "Plane.h"
class SurveillanceRadar {
public:
	SurveillanceRadar(){};
	virtual ~SurveillanceRadar();
    static void task_handler(union sigval arg) {
        Plane* plane = (Plane*)arg.sival_ptr;
        int coid = ConnectAttach(0, 0, plane->chid, 0, 0);  // Node 0, PID 0 (any process), Channel ID 1
               if (coid == -1) {
                   perror("ConnectAttach failed");
                   return ;
               }

               msg_struct msg;
               strcpy(msg.body, "P");

               msg.id = plane->flight_id;
               msg_struct reply;

               int status = MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply));
               if (status == -1) {
                   perror("SR MsgSend failed");
                   return ;
               }
               //std::cout<<reply.body<<std::endl;
               Logger::getInstance().log("Reply received from the server. " + std::string(reply.body));
               ConnectDetach(coid);
    }


    static int create_task(Plane* tasks) {
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

        its.it_value.tv_sec = 2;
        its.it_value.tv_nsec = 0;
        its.it_interval.tv_sec = 2;
        its.it_interval.tv_nsec = 0;

        if (timer_settime(monitor_timer, 0, &its, NULL) == -1) {
            perror("Failed to start monitor timer");
            return -1;
        }
        return 0;
    }



    static void polling_task_handler(union sigval arg) {
        Plane* plane = (Plane*)arg.sival_ptr;
        int coid = ConnectAttach(0, 0, plane->chid, 0, 0);  // Node 0, PID 0 (any process), Channel ID 1
               if (coid == -1) {
                   perror("ConnectAttach failed");
                   return ;
               }

               msg_struct msg;
               msg.label='P';

               msg.id = plane->flight_id;
               msg_struct reply;

               // Send a message to the server and receive a reply
               int status = MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply));
               if (status == -1) {
                   perror("SR MsgSend failed");
                   return ;
               }
               std::cout<<reply.body<<std::endl;
               Logger::getInstance().log("Reply received from the server. " + std::string(reply.body));
               ConnectDetach(coid);
    }


    static int create_polling_task(Plane* tasks) {
        struct sigevent sev;
        struct itimerspec its;
        timer_t monitor_timer;

        sev.sigev_notify = SIGEV_THREAD;
        sev.sigev_value.sival_ptr = tasks;
        sev.sigev_notify_function = polling_task_handler;
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

#endif /* SRC_SURVEILLANCERADAR_H_ */
