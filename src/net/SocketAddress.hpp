#ifndef _cServer_SOCKETADDRESS_HPP__
#define _cServer_SOCKETADDRESS_HPP__

#include <netdb.h>
#include <cstring>
#include <memory>

class SocketAddress
{
public:
    SocketAddress(uint32_t inAddress, uint16_t inPort)
    {
        GetSocketAddrIn()->sin_family = AF_INET;
        GetSocketAddrIn()->sin_addr.s_addr = htonl(inAddress);
        GetSocketAddrIn()->sin_port = htons(inPort);
    }

    SocketAddress(const sockaddr &inSockAddr)
    {
        ::memcpy(&mSockAddr, &inSockAddr, sizeof(sockaddr));
    }

    size_t GetSize() const
    {
        return sizeof(sockaddr);
    }

private:
    sockaddr mSockAddr;

    sockaddr_in *GetSocketAddrIn()
    {
        return reinterpret_cast<sockaddr_in *>(&mSockAddr);
    }
};

using SocketAddressPtr = std::shared_ptr<SocketAddress>;

class SocketAddressFactory
{
public:
    static SocketAddressPtr CreateIPv4FromString(const std::string &inString)
    {
        auto pos = inString.find_last_of(':');
        std::string host, service;
        if (pos != std::string::npos)
        {
            host = inString.substr(0, pos);
            service = inString.substr(pos + 1);
        }
        else
        {
            host = inString;
            service = "0";
        }

        addrinfo hint;
        memset(&hint, 0, sizeof(hint));
        hint.ai_family = AF_INET;

        addrinfo *result;
        int error = getaddrinfo(host.c_str(), service.c_str(), &hint, &result);
        if (error != 0 && result != nullptr)
        {
            freeaddrinfo(result);
            return nullptr;
        }

        while (!result->ai_addr && result->ai_next)
        {
            result = result->ai_next;
        }

        if (!result->ai_addr)
        {
            freeaddrinfo(result);
            return nullptr;
        }

        auto toRet = std::make_shared<SocketAddress>(*result->ai_addr);

        freeaddrinfo(result);

        return toRet;
    }
};

#endif