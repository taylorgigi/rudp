#ifndef LIBJUPITER_PROTOCOL_BUFFER_H
#define LIBJUPITER_PROTOCOL_BUFFER_H

#define MTU                     1500
#define IP_HEADER_LEN           20
#define UDP_HEADER_LEN          20

#define PROTOCOL_HEADER_LEN     4

#define AVAILABLE_BUFFER_SIZE   (MTU-IP_HEADER_LEN-UDP_HEADER_LEN-PROTOCOL_HEADER_LEN)

class ProtocolBuffer
{
public:
        ProtocolBuffer(): len(0) {}
        ~ProtocolBuffer() {}
        //@brief                get user data address
        //@return               address
        char *UserDataAddress() { return buffer + PROTOCOL_HEADER_LEN; }

        //@brief                get available user buffer size
        //@return               available user buffer size
        int UserBufferLen() { return AVAILABLE_BUFFER_SIZE - PROTOCOL_HEADER_LEN; }

        //@brief                get user data len
        //@return               user data len
        int UserDataLen() { return len - PROTOCOL_HEADER_LEN; }

        //@brief                set user data len
        //@param    v   [IN]    user data len
        void UserDataLen(int v) { len = v + PROTOCOL_HEADER_LEN; }

        //@brief                get protocol data address
        //@return               protocol data address
        char *ProtocolDataAddress() { return buffer; }

        //@brief                get protocol data len
        //@return               protocol data len
        int ProtocolDataLen() { return len; }
        void ProtocolDataLen(int v) { len = v; }

        int ProtocolBufferLen() { return AVAILABLE_BUFFER_SIZE; }

        //@brief                get packet id
        //@return               packet id
        uint32_t PacketId() { return *((uint32_t*)buffer); }
        void PacketId(uint32_t v) { *((uint32_t*)buffer) = v; }
private:
        //protocol data len
        int len;

        //protocol data buffer
        char buffer[AVAILABLE_BUFFER_SIZE] {0};
};

#endif

