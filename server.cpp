#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <boost/filesystem.hpp>
#include "UdpConnectionPool.h"
#include "ProtocolBuffer.h"
#include "logger.h"

std::shared_ptr<UdpConnectionPool> conn_pool = UdpConnectionPool::Create();

class Server
{
public:
        Server(const char *local_ip, uint16_t local_port, const char *peer_ip, uint16_t peer_port):
                local(make_address(local_ip), local_port),
                peer(make_address(peer_ip), peer_port)
        {
                conn_pool->Register(local, peer, boost::bind(&Server::ReceiveHandler, this, boost::placeholders::_1, boost::placeholders::_2, boost::placeholders::_3), boost::bind(&Server::SendHandler, this, boost::placeholders::_1, boost::placeholders::_2, boost::placeholders::_3));
        }

        ~Server() {}

        void send()
        {
                std::shared_ptr<ProtocolBuffer> data = std::make_shared<ProtocolBuffer>();
                data->PacketId(pid.fetch_add(1));
                data->UserDataLen(0);
                conn_pool->AsyncSend(local, peer, data);
        }
private:
        void ReceiveHandler(const boost::system::error_code& ec, const size_t& bytes, std::weak_ptr<ProtocolBuffer> data)
        {
                std::shared_ptr<ProtocolBuffer> d = data.lock();
                if(d)
                        printf("receive packet %u\n", d->PacketId());
                else
                        printf("receive packet, weakptr lock failed %u\n", d->PacketId());
                if(ec)
                        printf("receive failed %s\n", ec.message().c_str());
                send();
        }

        void SendHandler(const boost::system::error_code& ec, const size_t& bytes, std::weak_ptr<ProtocolBuffer> data)
        {
                std::shared_ptr<ProtocolBuffer> d = data.lock();
                if(d)
                        ;//printf("send packet %u\n", d->PacketId());
                else
                        printf("send packet, weakptr lock failed %u\n", d->PacketId());
        }
private:
        std::atomic<uint32_t> pid {0};
        udp::endpoint local;
        udp::endpoint peer;
};

int main()
{
    // intitialize logger, if not yet
    logger_init("server-logger", "log-server", "server");

    Server server1("192.168.199.218", 20002, "192.168.199.218", 20001);
    Server server2("192.168.199.218", 20004, "192.168.199.218", 20003);

    conn_pool->Start();

    pause();

    return 0;
}

