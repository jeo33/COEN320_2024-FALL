#include <iostream>
#include <fstream>
#include <string>

int main() {
    // Define the file name
    std::string filename = "planes.txt";

    // Open the file in write mode
    std::ofstream file(filename);

    // Check if the file opened successfully
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return 1; // Return with an error code
    }
    // Write the plane data to the file with velocity scaled by 20
    file << "0,1,0,0,5000,600,500,-300\n";  // Start at (0,0), moves out of bounds on x, will be clamped
    file << "0,2,100000,0,6000,400,700,-200\n";  // Start at (100,0), moves out of bounds on y, will be clamped
    file << "0,3,100000,100000,7000,200,-100,100\n";  // Start at (100,100), moves out of bounds, will be clamped
    file << "0,4,0,100000,8000,1000,300,-400\n";  // Start at (0,100), moves out of bounds, will be clamped
    file << "0,5,50000,0,4000,300,-200,200\n";  // Start at (50,0), moves within bounds
    file << "0,6,100000,50000,3000,400,600,-100\n";  // Start at (100,50), moves out of bounds, will be clamped
    file << "0,7,0,50000,10000,500,300,0\n";  // Start at (0,50), moves within bounds
    file << "0,8,50000,100000,2500,-200,100,100\n";  // Start at (50,100), moves within bounds
    file << "0,9,100000,0,15000,-400,0,-300\n";  // Start at (100,0), moves out of bounds, will be clamped
    file << "0,10,0,0,2000,800,500,-600\n";  // Start at (0,0), moves out of bounds, will be clamped
    file << "0,11,50000,100000,12000,1200,-200,500\n";  // Start at (50,100), moves within bounds
    file << "0,12,100000,50000,17000,700,900,-800\n";  // Start at (100,50), moves within bounds
    file << "0,13,0,50000,1500,100,200,100\n";  // Start at (0,50), moves within bounds
    file << "0,14,50000,0,5000,0,-500,200\n";  // Start at (50,0), moves within bounds
    file << "0,15,100000,100000,8000,-300,200,-100\n";  // Start at (100,100), moves out of bounds, will be clamped
    file << "0,16,0,100000,2500,1000,100,200\n";  // Start at (0,100), moves out of bounds, will be clamped
    file << "0,17,100000,0,20000,400,300,600\n";  // Start at (100,0), moves out of bounds, will be clamped
    file << "0,18,0,0,10000,-600,100,-100\n";  // Start at (0,0), moves out of bounds, will be clamped
    file << "0,19,50000,0,12000,0,200,-200\n";  // Start at (50,0), moves within bounds
    file << "0,20,100000,100000,9000,200,-100,500\n";  // Start at (100,100), moves out of bounds, will be clamped


    // Close the file
    file.close();

    std::cout << "Data written to " << filename << " successfully." << std::endl;
    return 0; // Return success
}
