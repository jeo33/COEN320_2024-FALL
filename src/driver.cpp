#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>
#include "IOtool.h"

#define x_max	100
#define y_max	100
#define z_max	250
void task_handler(union sigval arg) {
    TaskArgs* plane = (TaskArgs*)arg.sival_ptr;
    std::cout << "Update position..." << std::endl;
    if (plane->current_t < plane->t_init) {
        plane->current_t++;
    } else {
        plane->Position_x += plane->velocity_x;
        plane->Position_y += plane->velocity_y;
        plane->Position_z += plane->velocity_z;
        plane->current_t++;
    }
}

int create_periodic_task(TaskArgs* task, int period_sec, int period_nsec) {
    struct sigevent sev;
    struct itimerspec its;

    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_value.sival_ptr = task;
    sev.sigev_notify_function = task_handler;
    sev.sigev_notify_attributes = NULL;

    if (timer_create(CLOCK_REALTIME, &sev, &task->timer_id) == -1) {
        perror("Failed to create timer");
        return -1;
    }

    its.it_value.tv_sec = period_sec;
    its.it_value.tv_nsec = period_nsec;
    its.it_interval.tv_sec = period_sec;
    its.it_interval.tv_nsec = period_nsec;

    if (timer_settime(task->timer_id, 0, &its, NULL) == -1) {
        perror("Failed to start timer");
        return -1;
    }

    return 0;
}

void monitor_handler(union sigval arg) {
    std::vector<TaskArgs*>* monitor = (std::vector<TaskArgs*>*)arg.sival_ptr;

    printf("\n--- Current Displacements ---\n");
    for (auto* it : *monitor) {
        printf("Task %d: (%d,%d,%d)\n", it->flight_id, it->Position_x, it->Position_y, it->Position_z);
    }
    printf("-----------------------------\n");
}

int create_monitor_task(std::vector<TaskArgs*>& tasks) {
    struct sigevent sev;
    struct itimerspec its;
    timer_t monitor_timer;

    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_value.sival_ptr = &tasks;
    sev.sigev_notify_function = monitor_handler;
    sev.sigev_notify_attributes = NULL;

    if (timer_create(CLOCK_REALTIME, &sev, &monitor_timer) == -1) {
        perror("Failed to create monitor timer");
        return -1;
    }

    its.it_value.tv_sec = 1;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 1;
    its.it_interval.tv_nsec = 0;

    if (timer_settime(monitor_timer, 0, &its, NULL) == -1) {
        perror("Failed to start monitor timer");
        return -1;
    }

    return 0;
}
void radar_handler(union sigval arg) {
    std::vector<TaskArgs*>* radar = (std::vector<TaskArgs*>*)arg.sival_ptr;

    printf("\n------------------Map------------------\n");
    char** tem = new char*[x_max];
       for (int i = 0; i < x_max; i++) {
           tem[i] = new char[y_max];
       }

       // Initialize the array with spaces
       for (int i = 0; i < x_max; i++) {
           for (int j = 0; j < y_max; j++) {
               tem[i][j] = ' ';
           }
       }

       // Mark positions of planes with '*'
       for (auto& it : *radar) {
           if (it->Position_x >= 0 && it->Position_x < x_max && it->Position_y >= 0 && it->Position_y < y_max) {
               tem[it->Position_x][it->Position_y] = '*';
           }
       }

       // Print the 2D array
       std::cout << "---------------------------------------------------------------" << std::endl;
       for (int i = 0; i < x_max; i++) {
           for (int j = y_max-1; j >=0; j--) {
               std::cout << tem[i][j];
           }
           std::cout << std::endl;
       }
       std::cout << "---------------------------------------------------------------" << std::endl;

       // Free the dynamically allocated memory
       for (int i = 0; i < x_max; i++) {
           delete[] tem[i];
       }
       delete[] tem;
    printf("0------------------x---------------------->\n");
}
int create_rader_task(std::vector<TaskArgs*>& tasks) {
    struct sigevent sev;
    struct itimerspec its;
    timer_t monitor_timer;

    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_value.sival_ptr = &tasks;
    sev.sigev_notify_function = radar_handler;
    sev.sigev_notify_attributes = NULL;

    if (timer_create(CLOCK_REALTIME, &sev, &monitor_timer) == -1) {
        perror("Failed to create monitor timer");
        return -1;
    }

    its.it_value.tv_sec = 10;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 10;
    its.it_interval.tv_nsec = 0;

    if (timer_settime(monitor_timer, 0, &its, NULL) == -1) {
        perror("Failed to start monitor timer");
        return -1;
    }

    return 0;
}

int main() {
    std::vector<TaskArgs*> planes;
    IOtool ioTool;
    ioTool.readPlanesFromFile("planes.txt", &planes);

    for (auto* it : planes) {
        std::cout << it->flight_id << std::endl;
    }

    for (auto* it : planes) {
        if (create_periodic_task(it, 1, 0) != 0) return EXIT_FAILURE;
    }

    if (create_monitor_task(planes) != 0) return EXIT_FAILURE;
    if (create_rader_task(planes) != 0) return EXIT_FAILURE;

    while (1) {
        pause();
    }

    // Free allocated memory (this will never be reached with the infinite loop, but included for correctness)
    for (auto* it : planes) {
        delete it;
    }

    return EXIT_SUCCESS;
}
