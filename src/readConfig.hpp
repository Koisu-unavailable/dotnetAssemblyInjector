#ifndef READCONFIG
#define READCONFIG
#include <string>
typedef struct Config{
    std::string configPath;
    std::string patchDllPath;
    std::string patchMethodAssembly;
    std::string patchMethodClass;
    std::string patchMethodName;

} Config_t;

Config_t parseConfig(char path[]);
std::string pprintConfig(Config_t*);
#endif