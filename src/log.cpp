#include "log.h"
#include <chrono>
Logger::Logger(std::string filePath)
{
    logFileP = new std::ofstream();
    fp = filePath;
}

void Logger::LogMessage(std::string message)
{
    auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string formated_message = std::format("{} {}", std::ctime(&time), message);
    logFileP->open(fp);
    *logFileP << message;
    logFileP->close();
    std::cout << message << std::endl;
}
Logger::~Logger(){
    delete logFileP;
    logFileP = nullptr;
}