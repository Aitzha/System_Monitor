#include <string>
#include <iostream>
#include <sstream>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) {
    long hour = seconds / 3600;
    long minute = seconds % 3600 / 60;
    long second = seconds % 3600 % 60;

    return std::to_string(hour) + ":" + std::to_string(minute) + ":" + std::to_string(second);
}