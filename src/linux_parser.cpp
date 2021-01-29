#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;


string LinuxParser::OperatingSystem() {
    string line;
    string key;
    string value;
    std::ifstream filestream(kOSPath);
    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::replace(line.begin(), line.end(), ' ', '_');
            std::replace(line.begin(), line.end(), '=', ' ');
            std::replace(line.begin(), line.end(), '"', ' ');
            std::istringstream linestream(line);
            while (linestream >> key >> value) {
                if (key == "PRETTY_NAME") {
                    std::replace(value.begin(), value.end(), '_', ' ');
                    return value;
                }
            }
        }
    }
    return value;
}


string LinuxParser::Kernel() {
    string os, kernel;
    string line;
    std::ifstream stream(kProcDirectory + kVersionFilename);
    if (stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> os >> kernel >> kernel;
    }
    return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
    vector<int> pids;
    DIR* directory = opendir(kProcDirectory.c_str());
    struct dirent* file;
    while ((file = readdir(directory)) != nullptr) {
        // Is this a directory?
        if (file->d_type == DT_DIR) {
            // Is every character of the name a digit?
            string filename(file->d_name);
            if (std::all_of(filename.begin(), filename.end(), isdigit)) {
                int pid = stoi(filename);
                pids.push_back(pid);
            }
        }
    }
    closedir(directory);
    return pids;
}


float LinuxParser::MemoryUtilization() {
    //in this method I used total memory, available memory and buffers to calculate Memory Utilization

    std::ifstream file(kProcDirectory + kMeminfoFilename);

    float memUtilization, availableMem, totalMem, buffer;

    string line;
    string name;
    float value;

    for(int i = 0; i < 4; i++) {
        getline(file, line);
        std::istringstream streamLine(line);

        streamLine >> name >> value;

        if(name == "MemTotal:") {
            totalMem = value;
        } else if(name == "MemAvailable:") {
            availableMem = value;
        } else if(name == "Buffers:") {
            buffer = value;
        }
    }

    memUtilization = 1 - (availableMem/(totalMem - buffer));

    return memUtilization;
}


long LinuxParser::UpTime() {
    std::ifstream file(kProcDirectory + kUptimeFilename);

    long upTime = 0;
    std::string line;
    if(file.is_open()) {
        file >> line;
        std::istringstream streamLine(line);
        streamLine >> upTime;
    }

    return upTime;
}


long LinuxParser::Jiffies() {
    return (LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies());
}


long LinuxParser::ActiveJiffies(int pid) {
    std::string line, value;
    long total_time = 0;
    std::ifstream file(kProcDirectory + to_string(pid) + kStatFilename);

    if(file.is_open()) {
        getline(file, line);
        std::istringstream streamLine(line);

        for(int i = 0; i < 17; i++) {
            streamLine >> value;
            if(i > 12) {
                total_time += std::stol(value);
            }
        }
    }

    return total_time;
}


long LinuxParser::ActiveJiffies() {
    std::ifstream file(kProcDirectory + kStatFilename);
    long activeJiffies = 0;

    if(file.is_open()) {
        std::string line;
        getline(file, line);
        std::istringstream streamLine(line);
//read the first word cpu on the line
        std::string cpu;
        streamLine >> cpu;
//start to adding the numbers expect idle and iowait
        int time;
        int count = 1;
        while(streamLine >> time) {
            if(count != 4 && count != 5) {
                activeJiffies += time;
            }
            count++;
        }
    }

    return activeJiffies;
}


long LinuxParser::IdleJiffies() {
    std::ifstream file(kProcDirectory + kStatFilename);
    std::string line;
    long idleJiffies = 0;
    if(file.is_open()) {
        std::string cpu;
        getline(file, line);
        std::istringstream streamLine(line);
        streamLine >> cpu;

        int time;
//count only last two numbers in the loop which are idle and iowait
        for(int i = 0; i < 5; i++) {
            streamLine >> time;
            if(i == 4 || i == 3) {
                idleJiffies += time;
            }
        }
    }
    return idleJiffies;
}

vector<long> LinuxParser::ForCpuUtilization(int pid) {
    vector<long> for_cpu_util;
    std::string line, value;
    long start_time;
    std::ifstream file(kProcDirectory + to_string(pid) + kStatFilename);

    if(file.is_open()) {
        getline(file, line);
        std::istringstream streamLine(line);

        for(int i = 0; i < 22; i++) {
            streamLine >> start_time;
        }
    }

    for_cpu_util.push_back(start_time);
    for_cpu_util.push_back(LinuxParser::ActiveJiffies(pid));
    return for_cpu_util;
}


int LinuxParser::TotalProcesses() {
    std::ifstream file(kProcDirectory + kStatFilename);
    while(file.is_open()) {
        std::string line;
        getline(file, line);
        std::istringstream streamLine(line);
        std::string lineName;
        streamLine >> lineName;
        if(lineName == "processes") {
            int numberOfProcesses;
            streamLine >> numberOfProcesses;
            return numberOfProcesses;
        }
    }
    return 0;
}


int LinuxParser::RunningProcesses() {
    std::ifstream file(kProcDirectory + kStatFilename);
    while(file.is_open()) {
        std::string line;
        getline(file, line);
        std::istringstream streamLine(line);
        std::string lineName;
        streamLine >> lineName;
        if(lineName == "procs_running") {
            int numberOfProcesses;
            streamLine >> numberOfProcesses;
            return numberOfProcesses;
        }
    }
    return 0;
}

string LinuxParser::Command(int pid) {
    std::ifstream file(kProcDirectory + to_string(pid) + kCmdlineFilename);
    std::string line;
    if(file.is_open()) {
        getline(file, line);
    }
    return line;
}

string LinuxParser::Ram(int pid) {
    std::string line, value, name;
    std::ifstream file(kProcDirectory + to_string(pid) + kStatusFilename);

    if(file.is_open()) {
        while(getline(file, line)) {
            std::istringstream streamLine(line);
            streamLine >> name >> value;
            if(name == "VmSize:") {
                return value;
            }
        }
    }
    return value;
}


string LinuxParser::Uid(int pid) {
    std::ifstream file(kProcDirectory + std::to_string(pid)+ kStatFilename);
    std::string lineName;
    std::string line;
    std::string uid;

    if(file.is_open()) {
        while(getline(file, line)) {
            std::istringstream streamLine(line);

            streamLine >> lineName;

            if(lineName == "Uid:") {
                streamLine >> uid;
                return uid;
            }
        }
    }
    return uid;
}


string LinuxParser::User(int pid) {
    std::ifstream file(kPasswordPath);
    std::string line, uid, user_name, x, key;
    uid = LinuxParser::Uid(pid);

    if(file.is_open()) {
        while(getline(file, line)) {
            std::replace(line.begin(), line.end(), ':', ' ');
            std::istringstream streamLine(line);

            streamLine >> user_name >> x >> key;

            if(key == uid) {
                return user_name;
            }
        }
    }
    return user_name;
}


long LinuxParser::UpTime(int pid) {

    std::string line;
    long up_time;
    std::ifstream file(kProcDirectory + to_string(pid) + kStatFilename);

    if(file.is_open()) {
        getline(file, line);
        std::istringstream streamLine(line);

        for(int i = 0; i < 22; i++) {
            streamLine >> up_time;
        }
    }

    return up_time;
}