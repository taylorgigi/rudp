#ifndef LIBJUPITER_CALL_BACK_H
#define LIBJUPITER_CALL_BACK_H

#include <boost/function.hpp>

class ProtocolBuffer;

typedef boost::function<void(const boost::system::error_code& ec, const size_t& bytes, std::weak_ptr<ProtocolBuffer> data)> ReceiveCallBack;
typedef boost::function<void(const boost::system::error_code& ec, const size_t& bytes, std::weak_ptr<ProtocolBuffer> data)> SendCallBack;

struct CallBackCtx
{
        CallBackCtx(const ReceiveCallBack& rcb, const SendCallBack& scb): receive_callback(rcb), send_callback(scb) {}
        ~CallBackCtx() {}

        ReceiveCallBack receive_callback;
        SendCallBack    send_callback;
};

#endif

