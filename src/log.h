#include <iostream>
#include <fstream>
#include <string>


#ifndef LOGGER_H
#define LOGGER_H

class Logger{
    std::ofstream* logFileP;
    std::string fp;
    public:
        Logger(std::string filePath);
        ~Logger();
        void LogMessage(std::string message);
};

#endif 