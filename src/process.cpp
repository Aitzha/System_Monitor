#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return pid_; }

float Process::CpuUtilization() {
    vector<long> for_calculation = LinuxParser::ForCpuUtilization(pid_);
    total_time = for_calculation[1];
    start_time = for_calculation[0];
    long uptimeOfSystem = LinuxParser::UpTime();

    long elapsedTime = uptimeOfSystem - (start_time / sysconf(_SC_CLK_TCK));

    cpu_utilize = float(float(total_time / sysconf(_SC_CLK_TCK)) / elapsedTime);
    return cpu_utilize;
}

string Process::Command() { return LinuxParser::Command(pid_); }

string Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() { return LinuxParser::User(pid_); }

long int Process::UpTime() { return (start_time / sysconf(_SC_CLK_TCK)); }

bool Process::operator<(Process const& a) const {
    return cpu_utilize < a.cpu_utilize;
}