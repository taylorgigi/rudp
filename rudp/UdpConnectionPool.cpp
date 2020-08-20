#include <cstdio>
#include <gperftools/tcmalloc.h>
#include "logger.h"
#include "UdpConnectionPool.h"

std::shared_ptr<UdpConnectionPool> UdpConnectionPool::Create()
{
        return std::make_shared<UdpConnectionPool>();
}

UdpConnectionPool::UdpConnectionPool()
{
}

UdpConnectionPool::~UdpConnectionPool()
{
        io_context.stop();
        for(auto& it : io_thread_pool) {
                it->join();
        }
}

void UdpConnectionPool::Register(const udp::endpoint& local_endpoint,
                const udp::endpoint& peer_endpoint,
                const ReceiveCallBack& receive_callback,
                const SendCallBack& send_callback)
{
        std::unique_lock<std::mutex> lck(mtx);
        auto it = conn_pool.find(local_endpoint);
        if(it == conn_pool.end()) {
                conn_pool[local_endpoint] = std::make_shared<UdpConnection>(local_endpoint, io_context);
                it = conn_pool.find(local_endpoint);
        }
        it->second->Register(peer_endpoint, receive_callback, send_callback);
        logger->trace("[UdpConnectionPool] create connection for {}:{} <---> {}:{}", local_endpoint.address().to_string().c_str(), local_endpoint.port(), peer_endpoint.address().to_string().c_str(), peer_endpoint.port());
}

void UdpConnectionPool::Unregister(const udp::endpoint& local_endpoint, const udp::endpoint& peer_endpoint)
{
        std::unique_lock<std::mutex> lck(mtx);
        auto it = conn_pool.find(local_endpoint);
        if(it != conn_pool.end()) {
                if(it->second->Unregister(peer_endpoint) == 0)
                        conn_pool.erase(it);
        }
}

void UdpConnectionPool::AsyncSend(const udp::endpoint& local_endpoint, const udp::endpoint& peer_endpoint, const std::shared_ptr<ProtocolBuffer>& data)
{
        std::unique_lock<std::mutex> lck(mtx);
        auto it = conn_pool.find(local_endpoint);
        if(it != conn_pool.end()) {
                it->second->AsyncSend(peer_endpoint, data);
        }
        else
                logger->error("[UdpConnectionPool] local connection {}:{} does not create yet", local_endpoint.address().to_string().c_str(), local_endpoint.port());
}

void UdpConnectionPool::Start()
{
        unsigned int core_num = std::thread::hardware_concurrency();
        for(decltype(core_num) i=0; i<core_num; ++i) {
                io_thread_pool.push_back(std::unique_ptr<boost::thread>(new boost::thread(boost::bind(&boost::asio::io_context::run, &io_context))));
        }
}

void UdpConnectionPool::Stop()
{
    for(auto& t : io_thread_pool) {
        t->join();
    }
}
