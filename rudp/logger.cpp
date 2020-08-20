#include <cstdlib>
#include <cstdio>
#include <boost/filesystem.hpp>
#include "logger.h"

pthread_once_t logger_is_initialized = PTHREAD_ONCE_INIT;

std::shared_ptr<spdlog::logger> logger;

bool logger_init(const char *logger_name, const char *dir_name, const char *log_name)
{
    boost::filesystem::path cur_path = boost::filesystem::current_path();
    cur_path.append("/");
    cur_path.append(dir_name);
    if (!boost::filesystem::exists(cur_path)) {
        if (!boost::filesystem::create_directory(cur_path)) {
            printf("create log dir failed: %s", cur_path.c_str());
            return false;
        }
    }
    spdlog::set_level(spdlog::level::trace);
    spdlog::set_async_mode(8192, spdlog::async_overflow_policy::block_retry, nullptr, std::chrono::milliseconds(1000));
    char buf[128] = {0};
    sprintf(buf, "%s/%s", cur_path.c_str(), log_name);
    logger = spdlog::hourly_logger_mt(logger_name, buf, 5 * 1024 * 1024);
    logger->set_pattern("[%Y-%m-%d %T.%e]: %v");
    return true;
}

