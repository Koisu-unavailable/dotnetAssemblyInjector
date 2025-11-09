#include <stdio.h>
#include "readConfig.hpp"
#include "log.h"
#include <iostream>
#include <fstream>
int main(void){
    puts("Testing parse config");
    std::ofstream ofstream;
    ofstream.open("testConfig.cfg");
    ofstream << "configPath=bla" << std::endl;
    ofstream << "patchDllPath=b2la" << std::endl;
    ofstream << "patchMethodAssembly=bla323" << std::endl;
    ofstream << "patchMethodClass=bdwcsdkcla" << std::endl ;
    ofstream << "patchMethodName=bwecwecla" << std::endl;
    ofstream.close();
    auto config = parseConfig(const_cast<char*>("testConfig.cfg"));
    std::cout << pprintConfig(&config) << "\n";
   const std::string logFp = "payload.log";
    Logger logger(logFp);
    logger.LogMessage("Beginning Injection");


}