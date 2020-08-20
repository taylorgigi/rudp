#ifndef LIBJUPITER_UDP_ENDPOINT_HASHER_INCLUDE_H
#define LIBJUPITER_UDP_ENDPOINT_HASHER_INCLUDE_H

#include <boost/container_hash/hash.hpp>
#include <boost/asio.hpp>
using namespace boost::asio::ip;

class UdpEndPointHasher
{
public:
        size_t operator()(const udp::endpoint& ep) const
        {
                size_t seed = 0;
                boost::hash_combine(seed, ep.address().to_string());
                boost::hash_combine(seed, ep.port());
                return seed;
        }
};

#endif

