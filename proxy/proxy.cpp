//
// Created by taylor on 2020/8/19.
//

#include <iostream>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <yaml-cpp/yaml.h>
#include "../rudp/logger.h"
#include "../rudp/UdpConnectionPool.h"
#include "ProxyServer.h"

std::shared_ptr<UdpConnectionPool> conn_pool;
std::unique_ptr<ProxyServer> proxy_server;

bool ParseConfigFile(const char *file_path)
{
    YAML::Node config;
    try {
        config = YAML::LoadFile(file_path);
    }
    catch (YAML::BadFile &e) {
        logger->error("[Proxy] YAML::LoadFile({}) failed:{}!", file_path, e.what());
        return false;
    }
    catch (YAML::ParserException &e) {
        logger->error("[Proxy] YAML::LoadFile({}) failed:{}!", file_path, e.what());
        return false;
    }
    YAML::Node spdlog_cfg = config["spdlog"];
    if (!spdlog_cfg) {
        logger->error("[Proxy] spdlog item not found,{}", file_path);
        return false;
    }
    YAML::Node log_level_cfg = spdlog_cfg["level"];
    if (!log_level_cfg) {
        logger->error("[Proxy] spdlog.level item not found,{}", file_path);
        return false;
    }
    std::string log_level = log_level_cfg.as<std::string>();
    logger->trace("[Proxy] spdlog.level = {}", log_level.c_str());
    if (log_level == "trace")
        spdlog::set_level(spdlog::level::trace);
    else if (log_level == "debug")
        spdlog::set_level(spdlog::level::debug);
    else if (log_level == "info")
        spdlog::set_level(spdlog::level::info);
    else if (log_level == "warn")
        spdlog::set_level(spdlog::level::warn);
    else if (log_level == "err")
        spdlog::set_level(spdlog::level::err);
    else if (log_level == "critical")
        spdlog::set_level(spdlog::level::critical);
    else if (log_level == "off")
        spdlog::set_level(spdlog::level::off);
    else
        spdlog::set_level(spdlog::level::warn);
    YAML::Node udp_config = config["udp"];
    if (!udp_config) {
        logger->error("[Proxy] udp item not found,{}", file_path);
        return false;
    }
    logger->trace("[Proxy] udp {} channels", udp_config.size());
    for (int i = 0; i<udp_config.size(); ++i) {
        YAML::Node channel_config = udp_config[i]["channel"];
        try {
            conn_pool->Register(
                    udp::endpoint(make_address(channel_config[0].as<std::string>()), channel_config[1].as<int>()),
                    udp::endpoint(make_address(channel_config[2].as<std::string>()), channel_config[3].as<int>()),
                    boost::bind(&ProxyServer::ReceiveHandler, proxy_server.get()),
                    boost::bind(&ProxyServer::SendHandler, proxy_server.get()));
        }
        catch (...) {
            return false;
        }
        logger->trace("[Proxy] [{}] ip:{} port:{} target_ip:{} target_port:{} loss:{} delay:{}", i,
                      channel_config[0].as<std::string>().c_str(),
                      channel_config[1].as<int>(),
                      channel_config[2].as<std::string>().c_str(),
                      channel_config[3].as<int>(),
                      channel_config[4].as<int>(),
                      channel_config[5].as<int>());
    }
    return true;
}

int main(int argc, char *argv[])
{
    if(!logger_init("proxy-logger", "log", "proxy")) {
        std::cout << "logger_init() failed!" << std::endl;
        return -1;
    }
    std::cout << "logger_init() succeed!" << std::endl;
    conn_pool = UdpConnectionPool::Create();
    if(!conn_pool) {
        std::cout << "create udp connection pool failed!" << std::endl;
        return -1;
    }
    std::cout << "create udp connection pool succeed!" << std::endl;
    proxy_server = std::make_shared<ProxyServer>();
    if(!ParseConfigFile("config.yaml")) {
        std::cout << "parse config file failed!" << std::endl;
        return -1;
    }
    conn_pool->Start();
    conn_pool->Stop();
}
