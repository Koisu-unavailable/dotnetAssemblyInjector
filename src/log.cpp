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
    auto time_text = std::string(std::ctime(&time));
    time_text = time_text.substr(0, time_text.find("\n"));
    std::string formated_message = std::format("{}: {}", time_text.c_str(), message);
    logFileP->open(fp, std::ios::app);
    *logFileP << formated_message << std::endl;
    logFileP->close();
    std::cout << formated_message << std::endl;
}
Logger::~Logger(){
    delete logFileP;
    logFileP = nullptr;
}