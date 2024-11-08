#include "IOtool.h"

void IOtool::readPlanesFromFile(const std::string& filename, std::vector<TaskArgs*>* planes) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
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
        int chid = ChannelCreate(0);
        planes->push_back(new TaskArgs{ .flight_id=id, .velocity_x=x, .velocity_y=y,
        	.velocity_z=z, .Position_x=speedX, .Position_y=speedY, .Position_z=speedZ,
			.t_init=time,.current_t=0,.chid=chid });
    }

    file.close();
}
