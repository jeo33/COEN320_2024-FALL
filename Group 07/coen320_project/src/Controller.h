#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <pthread.h>
#include <iostream>
#include <string>
#include <atomic>
#include <map>
#include <vector>
#include <cstring>
#include <sys/dispatch.h>
#include "Plane.h"

class Controller {
public:
    std::atomic<bool> running;   
    pthread_t thread;           
    std::vector<int> plane_chids;
    std::map<int, std::string> plane_data;

    Controller(const std::vector<int>& chids)
        : plane_chids(chids), running(true) {}

    ~Controller() {
        stop_thread(); 
    }

    static void* thread_function(void* arg) {
        Controller* controller = static_cast<Controller*>(arg);

        while (controller->running) {
            for (const auto& chid : controller->plane_chids) {
                msg_struct msg, reply;
                memset(&msg, 0, sizeof(msg));
                msg.label = "POLLING";  
                strcpy(msg.body, "P"); 

                int coid = ConnectAttach(0, 0, chid, _NTO_SIDE_CHANNEL, 0);
                if (coid == -1) {
                    perror("ConnectAttach error");
                    continue; 
                }

                if (MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply)) == -1) {
                    perror("MsgSend error");
                } else {
                    
                    controller->plane_data[reply.id] = std::string(reply.body);
                }

                if (ConnectDetach(coid) == -1) {
                    perror("ConnectDetach error");
                }
            }

            struct timespec req = {1, 0};  // 1 second delay
            nanosleep(&req, nullptr);
        }

        pthread_exit(nullptr);
        return nullptr;
    }

    void start_thread() {
        if (!running) {
            running = true;
            if (pthread_create(&thread, nullptr, Controller::thread_function, this) != 0) {
                perror("Failed to create pthread");
                running = false;
            }
        }
    }

    void stop_thread() {
        if (running) {
            running = false;
            if (pthread_join(thread, nullptr) != 0) {
                perror("Failed to join pthread");
            }
        }
    }

    void display_plane_info() const {
        for (const auto& [id, info] : plane_data) {
            std::cout << "Plane " << id << " Info:" << std::endl;
            std::cout << info << std::endl;
        }
    }
};

#endif  // CONTROLLER_H
