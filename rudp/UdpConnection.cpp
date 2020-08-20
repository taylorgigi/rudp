#include <cstdio>
#include <cstdlib>
#include <exception>
#include <boost/system/error_code.hpp>
#include <boost/bind/bind.hpp>
#include <gperftools/tcmalloc.h>
#include "UdpConnection.h"
#include "ProtocolBuffer.h"
#include "logger.h"
#include "backtrace.h"

UdpConnection::UdpConnection(const udp::endpoint& local_endpoint, boost::asio::io_context& io_context)//:
        //strand(boost::asio::make_strand(io_context))
{
        try {
                //socket.reset(new udp::socket(strand, local_endpoint));
                socket.reset(new udp::socket(io_context, local_endpoint));
        }
        catch(std::bad_alloc& e) {
                logger->error("[UdpConnection] alloc memory for udp::socket failed: {}", e.what());
                BACKTRACE();
                abort();
        }
        catch(boost::system::system_error& e) {
                logger->error("[UdpConnection] create udp::socket failed: {} {}", e.code().value(), e.what());
                BACKTRACE();
                abort();
        }
        PrepareReceive();
}

UdpConnection::~UdpConnection()
{
}

void UdpConnection::Register(const udp::endpoint& peer, const ReceiveCallBack& receive_callback, const SendCallBack& send_callback)
{
        std::unique_lock<std::mutex> lck(mtx);
        if(peer_callbacks.find(peer) == peer_callbacks.end()) {
                peer_callbacks[peer] = std::make_shared<CallBackCtx>(receive_callback, send_callback);
        }
        logger->trace("[UdpConnection] register callback {}:{}", peer.address().to_string().c_str(), peer.port());
}

int UdpConnection::Unregister(const udp::endpoint& peer)
{
        std::unique_lock<std::mutex> lck(mtx);
        peer_callbacks.erase(peer);
        logger->trace("[UdpConnection] unregister callback {}:{}", peer.address().to_string().c_str(), peer.port());
        return peer_callbacks.size();
}

void UdpConnection::AsyncSend(const udp::endpoint& peer, const std::shared_ptr<ProtocolBuffer>& data)
{
        Send(peer, data);
        //std::shared_ptr<udp::endpoint> peer_ = std::make_shared<udp::endpoint>(peer);
        //std::shared_ptr<SendCtx> ctx = std::make_shared<SendCtx>(peer_, data);
        //data_cache.push(ctx);
        //bool fs = true;
        //if(first_send.compare_exchange_strong(fs, false)) {
        //        Send();
        //}
}

void UdpConnection::PrepareReceive()
{
        std::shared_ptr<ProtocolBuffer> data;
        std::shared_ptr<udp::endpoint> peer;
        try {
                data = std::make_shared<ProtocolBuffer>();
                peer = std::make_shared<udp::endpoint>();
        }
        catch(std::bad_alloc& e) {
                logger->error("[UdpConnection] alloc memory failed: {}", e.what());
                BACKTRACE();
                abort();
                return;
        }
        std::unique_lock<std::mutex> lck(socket_lock);
        socket->async_receive_from(
                        boost::asio::mutable_buffer((void*)data->ProtocolDataAddress(), data->ProtocolBufferLen()),
                        *peer,
                        boost::bind(&UdpConnection::ReceiveHandler,
                                this,
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred,
                                data,
                                peer));
}

void UdpConnection::Send(const udp::endpoint& peer_, const std::shared_ptr<ProtocolBuffer>& data)
{
        std::shared_ptr<udp::endpoint> peer = std::make_shared<udp::endpoint>(peer_);
        std::unique_lock<std::mutex> lck(socket_lock);

        socket->async_send_to(boost::asio::buffer(data->ProtocolDataAddress(), data->ProtocolDataLen()), peer_, boost::bind(&UdpConnection::SendHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, data, peer));
        //std::shared_ptr<SendCtx> packet;
        //while(data_cache.try_pop(packet)) {
        //        socket->async_send_to(boost::asio::buffer(packet->data->ProtocolDataAddress(), packet->data->ProtocolDataLen()), *(packet->peer), boost::bind(&UdpConnection::SendHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, packet->data, packet->peer));
        //}
}

void UdpConnection::ReceiveHandler(boost::system::error_code ec, size_t bytes, std::shared_ptr<ProtocolBuffer> data, std::shared_ptr<udp::endpoint> peer)
{
        if(!ec) {
                data->ProtocolDataLen(bytes);
                std::unique_lock<std::mutex> lck(mtx);
                auto it = peer_callbacks.find(*peer);
                if(it != peer_callbacks.end()) {
                        it->second->receive_callback(ec, bytes, data);
                }
                else {
                        logger->error("[UdpConnection] receive callback not registered for {}:{}", peer->address().to_string().c_str(), peer->port());
                        return;
                }
                PrepareReceive();
        }
        else
                printf("failed %s\n", ec.message().c_str());
}

void UdpConnection::SendHandler(boost::system::error_code ec, const size_t bytes, const std::shared_ptr<ProtocolBuffer> data, const std::shared_ptr<udp::endpoint> peer)
{
        if(!ec) {
                std::unique_lock<std::mutex> lck(mtx);
                auto it = peer_callbacks.find(*peer);
                if(it != peer_callbacks.end()) {
                        it->second->send_callback(ec, bytes, data);
                }
                else {
                        logger->error("[UdpConnection] send callback not registered for {}:{}", peer->address().to_string().c_str(), peer->port());
                        return;
                }
        }
}

