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

    // Write the data to the file
    file << "0,1,1,2,1,5,2,0\n";
    file << "5,2,5,3,1,3,4,0\n";
    file << "10,3,7,5,2,2,3,1\n";
    file << "15,4,2,8,2,6,1,0\n";
    file << "20,5,9,1,5,1,5,2\n";

    // Close the file
    file.close();

    std::cout << "Data written to " << filename << " successfully." << std::endl;
    return 0; // Return success
}
