#include "Logger.h"
#include <iostream>
#include <ctime>
#include <chrono>

std::vector<LogEntry> Logger::msgs;

//current datetime
std::string ConvertDatetimeTostring(){
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    
    std::string buf(30, '\0');
    std::strftime(&buf[0], buf.size(), "%d-%b-%Y %H:%M:%S", std::localtime(&now));
    return buf;
}

void Logger::Log(const std::string& msg){
    //green text for just logger messages
    LogEntry logEntry;
    logEntry.type = LOG_INFO;
    logEntry.msg = "Log | " + ConvertDatetimeTostring() + " - " + msg;

    std::cout << GREEN << logEntry.msg << RESET << std::endl;
    msgs.push_back(logEntry);
}

void Logger::Error(const std::string& msg){
    //red text for errors
    LogEntry logEntry;
    logEntry.type = LOG_ERROR;
    logEntry.msg = "Err | " + ConvertDatetimeTostring() + " - " + msg;
    
    std::cout << RED << logEntry.msg << RESET << std::endl;
    msgs.push_back(logEntry);
}


void Logger::Warning(const std::string& msg) {
    //yellow text for warnings
    LogEntry logEntry;
    logEntry.type = LOG_WARNING;
    logEntry.msg = "Warn | " + ConvertDatetimeTostring() + " - " + msg;
    
    std::cout << YELLOW << logEntry.msg << RESET << std::endl;
    msgs.push_back(logEntry);
}