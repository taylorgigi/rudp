#ifndef LIBJUPITER_LOGGER_H
#define LIBJUPITER_LOGGER_H

#include <memory>
#include <pthread.h>
#include "spdlog/spdlog.h"

extern pthread_once_t logger_is_initialized;
extern std::shared_ptr<spdlog::logger> logger;

//@brief                    init logger
//@param    logger_name     name of the logger which will be created
//@param    dir             directory name, which should be a name, neither absolute path nor relative path
//@param    log_name        file name of log file
//@return                   true:succeed false:failed
bool logger_init(const char *logger_name, const char *dir_name, const char *log_name);

#endif

