#include "error.h"
#include "log.h"
void handleError(const char message[]){
    printf("%s", message);
    Logger logger("error.log");
    logger.LogMessage(message);
    exit(1);
}