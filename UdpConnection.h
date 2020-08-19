#ifndef LIBJUPITER_UDP_CONNECTION_H
#define LIBJUPITER_UDP_CONNECTION_H

#include <memory>
#include <mutex>
#include <unordered_map>
#include <boost/asio.hpp>
//#include <tbb/concurrent_queue.h>
#include "CallBack.h"
#include "UdpEndpointHasher.h"

using namespace boost::asio::ip;

class ProtocolBuffer;

struct SendCtx
{
        SendCtx(const std::shared_ptr<udp::endpoint> peer_, const std::shared_ptr<ProtocolBuffer>& data_): peer(peer_), data(data_) {}
        ~SendCtx() {}

        std::shared_ptr<udp::endpoint> peer;
        std::shared_ptr<ProtocolBuffer> data;
};

class UdpConnection
{
public:
        UdpConnection(const udp::endpoint& local_endpoint, boost::asio::io_context& io_context);
        ~UdpConnection();
        void Register(const udp::endpoint& peer, const ReceiveCallBack& receive_callback, const SendCallBack& send_callback);
        int Unregister(const udp::endpoint& peer);
        void AsyncSend(const udp::endpoint& peer, const std::shared_ptr<ProtocolBuffer>& data);
private:
        void ReceiveHandler(boost::system::error_code ec, size_t bytes, std::shared_ptr<ProtocolBuffer> data, std::shared_ptr<udp::endpoint> peer);
        void SendHandler(boost::system::error_code ec, size_t bytes, std::shared_ptr<ProtocolBuffer> data, std::shared_ptr<udp::endpoint> peer);
        void PrepareReceive();
        void Send(const udp::endpoint& peer_, const std::shared_ptr<ProtocolBuffer>& data);
private:
        //std::atomic<bool> first_send {true};
        //boost::asio::strand<boost::asio::io_context::executor_type> strand;
        std::mutex socket_lock;
        std::unique_ptr<udp::socket> socket;
        std::mutex mtx;
        std::unordered_map<udp::endpoint, std::shared_ptr<CallBackCtx>, UdpEndPointHasher> peer_callbacks;
        //tbb::concurrent_queue<std::shared_ptr<SendCtx>> data_cache;
};

#endif

