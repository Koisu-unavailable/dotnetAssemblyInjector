#include "readConfig.hpp"
#include "error.h"
#include <iostream>
#include <fstream>
#include <map>
Config_t parseConfig(char path[]){
    std::ifstream configP;
    Config_t config = {}; // default values
    configP.open(path);
    if (!configP.is_open()){
        char errorTemplate[] = "Could not read read config file: ";
        char* error_str = (char*)malloc(strlen(errorTemplate) + strlen(path) + 2); // accounting for 2 null terminators
        if (!error_str){
            handleError("FATAL ERROR!!!! Couldn't allocate memory");
        }
        const char* error_message = strcat(errorTemplate, path);
        handleError(error_message);
    }
    std::string line;
    int lineNum = 0;
    while (std::getline(configP, line)){
        lineNum++;
        size_t equalPos = line.find('=');
        std::string paramName = line.substr(0, equalPos);
        std::string paramValue = line.substr(equalPos+1, line.length() - 1);
        std::map<std::string, std::string*> paramMap;
        paramMap["configPath"] = &config.configPath;
        paramMap["patchDllPath"] = &config.patchDllPath;
        paramMap["patchMethodAssembly"] = &config.patchMethodAssembly;
        paramMap["patchMethodClass"] = &config.patchMethodClass;
        paramMap["patchMethodName"] = &config.patchMethodName;
        try{
            *paramMap.at(paramName) = paramValue; 
        }
        catch (std::out_of_range){
           
            handleError(std::format("Malformed Config on line: {} ", line).c_str());
        }
    }
    return config;
}
std::string pprintConfig(Config_t* config){
    std::string structStr;
    structStr += "Config Path: " + config->configPath + "\n";
    structStr += "Patch dll Path: " + config->patchDllPath + "\n";
    structStr += "Patch Method Assembly: " + config->patchMethodAssembly + "\n";
    structStr += "Patch Method Class: " + config->patchMethodClass + "\n";
    structStr += "Patch method name: " + config->patchMethodName + "\n";
    return structStr;
}