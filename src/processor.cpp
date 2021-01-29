#include "processor.h"
#include "linux_parser.h"
#include <iostream>
#include <unistd.h>

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    Idle = LinuxParser::IdleJiffies();
    NonIdle = LinuxParser::ActiveJiffies();

    long pTotal = pIdle + pNonIdle;
    long Total = Idle + NonIdle;




    long Tot_Delta = Total - pTotal;
    long Idle_Delta = Idle - pIdle;
    pIdle = Idle;
    pNonIdle = NonIdle;
    return float(1 -(float(Idle_Delta) / Tot_Delta));
}