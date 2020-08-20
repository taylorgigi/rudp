#include <cstdio>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include "../UdpConnectionPool.h"
#include "../ProtocolBuffer.h"
#include "../logger.h"

std::shared_ptr<UdpConnectionPool> conn_pool = UdpConnectionPool::Create();

class Client
{
public:
        Client(const char *local_ip, uint16_t local_port, const char *peer_ip, uint16_t peer_port):
                local(make_address(local_ip), local_port),
                peer(make_address(peer_ip), peer_port)
        {
                conn_pool->Register(local, peer, boost::bind(&Client::ReceiveHandler, this, boost::placeholders::_1, boost::placeholders::_2, boost::placeholders::_3), boost::bind(&Client::SendHandler, this, boost::placeholders::_1, boost::placeholders::_2, boost::placeholders::_3));
        }

        ~Client() {}

        void start()
        {
                uint32_t pid = 0;
                while(true) {
                        std::shared_ptr<ProtocolBuffer> data = std::make_shared<ProtocolBuffer>();
                        data->PacketId(pid++);
                        data->UserDataLen(0);
                        conn_pool->AsyncSend(local, peer, data);

                        boost::this_thread::sleep_for(boost::chrono::milliseconds(200));
                }
        }
private:
        void ReceiveHandler(const boost::system::error_code& ec, const size_t& bytes, std::weak_ptr<ProtocolBuffer> data)
        {
                std::shared_ptr<ProtocolBuffer> d = data.lock();
                if(d)
                        printf("receive packet %u\n", d->PacketId());
                if(ec)
                        printf("receive failed %s\n", ec.message().c_str());
        }

        void SendHandler(const boost::system::error_code& ec, const size_t& bytes, std::weak_ptr<ProtocolBuffer> data)
        {
                //std::shared_ptr<ProtocolBuffer> d = data.lock();
                //if(d)
                //        printf("send packet %u\n", d->PacketId());
        }
private:
        udp::endpoint local;
        udp::endpoint peer;
};

int main()
{
    // intitialize logger, if not yet
    logger_init("client-logger", "log-client", "client");

    Client client1("192.168.199.218", 20001, "192.168.199.218", 20002);
    Client client2("192.168.199.218", 20003, "192.168.199.218", 20004);

    conn_pool->Start();

    std::thread thread1(std::bind(&Client::start, &client1));
    std::thread thread2(std::bind(&Client::start, &client2));

    thread1.join();
    thread2.join();

    return 0;
}

