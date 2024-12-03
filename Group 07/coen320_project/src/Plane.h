#ifndef PLANE_H
#define PLANE_H

#include <pthread.h>
#include <cstdio>  // For snprintf
#include <iostream>
#include <string>  // Include for std::to_string()
#include <atomic>
#include <cstring>
#include <map>
#include <string>
#include <sstream>
#include <sys/dispatch.h>
#define RADAR_POLLING 'P'
#define CHANGE_SPEED 'V'
typedef struct {
    unsigned int id;
    char label;
    char body[1024];
    int x;
    int y;
    int z;
} msg_struct;

class Plane {
public:
    int flight_id;
    int Position_x;
    int Position_y;
    int Position_z;
    int velocity_x;
    int velocity_y;
    int velocity_z;
    int flight_level;
    int t_init;
    int current_t;
    int chid;
    int timer_id=0;
    std::string chanel_name;
    std::atomic<bool> running;
    pthread_t thread;

    Plane(int id, int x, int y, int z, int vx, int vy, int vz, int t_init)
        : flight_id(id), Position_x(x), Position_y(y), Position_z(z),
          velocity_x(vx), velocity_y(vy), velocity_z(vz), t_init(t_init),
          current_t(0), chid(ChannelCreate(0)), running(false),
          chanel_name(std::to_string(id)), flight_level(y + 15000) {
        if (chid == -1) {
            perror("Failed to create channel");
            exit(EXIT_FAILURE);
        }
        std::cout << "Channel created for Plane " << flight_id << " with chid: " << chid << std::endl;
    }

    ~Plane() {
        stop_thread();
        if (chid != -1) {
            if (ChannelDestroy(chid) == -1) {
                perror("Failed to destroy channel");
            } else {
                std::cout << "Channel destroyed for Plane " << flight_id << std::endl;
            }
        }
    }
    char* to_cstring() const {

           static char buffer[1024];
           snprintf(buffer, sizeof(buffer),
                    "Plane %d State:\n"
                    "  Position: (%d, %d, %d)\n"
                    "  Velocity: (%d, %d, %d)\n"
                    "  Time initialized: %d\n"
                    "  Current time: %d\n"
                    "  Channel ID: %d\n"
                    "  Timer ID: %d\n"
                    "-----------------------------------\n",
                    flight_id, Position_x, Position_y, Position_z,
                    velocity_x, velocity_y, velocity_z,
                    t_init, current_t, chid, timer_id);

           return buffer;
       }
    void update_position() {
        if (current_t < t_init) {
            current_t++;
        } else {
            Position_x += velocity_x;
            Position_y += velocity_y;
            Position_z += velocity_z;
            current_t++;
        }
    }

    void display() const {
        std::cout << "Plane " << flight_id << " State:" << std::endl;
        std::cout << "  Position: (" << Position_x << ", "
                  << Position_y << ", " << Position_z << ")" << std::endl;
        std::cout << "  Velocity: (" << velocity_x << ", "
                  << velocity_y << ", " << velocity_z << ")" << std::endl;
        std::cout << "  Time initialized: " << t_init << std::endl;
        std::cout << "  Current time: " << current_t << std::endl;
        std::cout << "  Channel ID: " << chid << std::endl;
        std::cout << "  Flight Level: " << flight_level << std::endl;
        std::cout << "-----------------------------------" << std::endl;
    }

    static void* thread_function_server(void* arg) {
           Plane* plane = (Plane*)(arg);
           while (plane->running) {
        	   int rcvid;
        	      		msg_struct msg;
        	      		std::string command=std::string(msg.body);
        	      		rcvid = MsgReceive(plane->chid, &msg, sizeof(msg), NULL);

        	      		if (rcvid == -1) {
        	      		  perror("MsgReceive error");
        	      		  continue;
        	      		}
        	      		msg_struct reply;
        	      		if(msg.label=='P'){
        	      			reply.id=plane->flight_id;
        	      			std::strcpy(reply.body, plane->to_cstring());
        	      		}
        	      		else if(msg.label=='V'){
        	      			std::cout<<"thread_function_server "<<std::endl;
        	      			plane->velocity_x=msg.x;
        	      			plane->velocity_y=msg.y;
        	      			plane->velocity_z=msg.z;
        	      			std::strcpy(reply.body, (command+" has been implemented").c_str());
        	      		}
        	      		else if(msg.label=='A'){
        	      			std::cout<<"ALERT!ALERT!ALERT!ALERT!-----Manually instruction is needed"<<std::endl;
        	      			std::cout<<msg.body<<std::endl;
        	      		}
        	      		else {
        	      			//std::cout<<msg.body<<std::endl;
        	      		}

        	      		if (MsgReply(rcvid, 0, &reply, sizeof(reply)) == -1) {
        	      		  perror("MsgReply error");
        	      		}
           }

           pthread_exit(nullptr);
           return nullptr;
       }
    static void* thread_function(void* arg) {
        Plane* plane = static_cast<Plane*>(arg);

        while (plane->running) {
            plane->update_position();
            struct timespec req = {1, 0};  // 1 second
            nanosleep(&req, nullptr);
        }

        pthread_exit(nullptr);
        return nullptr;
    }

    void start_thread() {
        if (!running) {
            running = true;
            if (pthread_create(&thread, nullptr, Plane::thread_function, this) != 0) {
                perror("Failed to create pthread");
                running = false;
            }
            if (pthread_create(&thread, nullptr, Plane::thread_function_server, this) != 0) {
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
};

#endif  // PLANE_H
