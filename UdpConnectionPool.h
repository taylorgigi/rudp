#ifndef LIBJUPITER_UDP_CONNECTION_POOL_H

#include <memory>
#include <mutex>
#include <unordered_map>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/core/noncopyable.hpp>
#include "UdpConnection.h"
#include "CallBack.h"
#include "UdpEndpointHasher.h"

using namespace boost::asio::ip;

class ProtocolBuffer;

class UdpConnectionPool: public boost::noncopyable
{
public:
        static std::shared_ptr<UdpConnectionPool> Create();
        UdpConnectionPool();
        ~UdpConnectionPool();
        void Register(const udp::endpoint& local_endpoint, const udp::endpoint& peer_endpoint, const ReceiveCallBack& receive_callback, const SendCallBack& send_callback);
        void Unregister(const udp::endpoint& local_endpoint, const udp::endpoint& peer_endpoint);
        void AsyncSend(const udp::endpoint& local_endpoint, const udp::endpoint& peer_endpoint, const std::shared_ptr<ProtocolBuffer>& data);
        void Start();
private:
        boost::asio::io_context io_context;
        std::vector<std::unique_ptr<boost::thread>> io_thread_pool;
        std::mutex mtx;
        std::unordered_map<udp::endpoint, std::shared_ptr<UdpConnection>, UdpEndPointHasher> conn_pool;
};

#endif

