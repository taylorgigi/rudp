#include <iostream>
#include "../UdpEndpointHasher.h"

int main()
{
        udp::endpoint local(make_address("192.168.0.100"), 20002);
        UdpEndPointHasher hasher;
        std::cout << hasher(local) << std::endl;

        return 0;
}

