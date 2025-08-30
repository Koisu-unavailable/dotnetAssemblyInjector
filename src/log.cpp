#include "log.h"

Logger::Logger(std::string filePath)
{
    logFileP = new std::ofstream();
    fp = filePath;
}

void Logger::LogMessage(std::string message)
{
    logFileP->open(fp);
    *logFileP << message;
    logFileP->close();
}
Logger::~Logger(){
    delete logFileP;
    logFileP = nullptr;
}