//
// Created by taylor on 2020/8/19.
//

#ifndef PROXY_PROXYSERVER_H
#define PROXY_PROXYSERVER_H

#include "UdpConnectionPool.h"

extern std::shared_ptr<UdpConnectionPool> conn_pool;

class ProxyServer
{
public:
    ProxyServer()
    {
    }
    ~ProxyServer()
    {
    }
    void ReceiveHandler()
    {}
    void SendHandler()
    {}
};

#endif //PROXY_PROXYSERVER_H
