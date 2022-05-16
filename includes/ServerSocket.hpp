#pragma once

#include <Socket.hpp>
#include <Utils.hpp>

/*
 * Listener socket that closes only when
 * the server shuts down. Can be used with
 * the following steps:
 * 1. init(<address>, <port>)
 * 2. ClientSocket s = accept(<*addr>)
 */
class ServerSocket : public Socket
{
public:
    ServerSocket();
    ServerSocket(const ServerSocket& cpy);
    ~ServerSocket();

    ServerSocket&   operator = (const ServerSocket& sock);

public:

    void            init(const std::string& host, uint16_t port);
    int             accept(SA_IN& addr);

private:
    static A_INFO   *_get_addr_info(const char *name, const char *service, A_INFO **server_info);

public:
    class UninitialisedFail : std::exception { };

};
