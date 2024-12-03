/*
 * IOtool.cpp
 *
 *  Created on: Nov. 24, 2024
 *      Author: gjp
 */

#include "IOtool.h"
#include <fstream>
#include <sstream>
#include <iostream>

void IOtool::readPlanesFromFile(const std::string& filename, std::vector<Plane*>* planes) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string token;
        int time, id, x, y, z, speedX, speedY, speedZ;

                std::getline(ss, token, ',');
                time = std::stoi(token);
                std::getline(ss, token, ','); id = std::stoi(token);
                std::getline(ss, token, ','); x = std::stoi(token);
                std::getline(ss, token, ','); y = std::stoi(token);
                std::getline(ss, token, ','); z = std::stoi(token);
                std::getline(ss, token, ','); speedX = std::stoi(token);
                std::getline(ss, token, ','); speedY = std::stoi(token);
                std::getline(ss, token, ','); speedZ = std::stoi(token);
                planes->push_back(new Plane(id, x, y,z,speedX,speedY,speedZ,time));
            }

    file.close();
}
