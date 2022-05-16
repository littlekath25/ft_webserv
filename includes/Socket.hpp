#pragma once

#include <Webserv.hpp>

#define A_INFO          struct addrinfo
#define SA_IN           struct sockaddr_in
#define SA              struct sockaddr
#define SOCKET_UNSET    (-2)
#define DFL_BACKLOG     100

/*
 * Simple wrapper around a unix socket. Should support
 * both Client and Server type sockets.
 */
class Socket
{
private:
    int         m_fd;
    SA_IN       m_address;

public:
    Socket();
    Socket(const Socket& cpy);

protected:
    Socket(int fd, SA_IN& address);

public:
    virtual ~Socket();

    Socket&         operator = (const Socket& sock);

    int&            getFd(void);
    in_addr_t&      getAddress(void);
    uint16_t        getPort(void) const;

protected:
    void            setFd(int fd);
    SA_IN&          getAddr(void);

public:
    /* close the socket */
    void            close(void);

protected:
    static void     _cpy_addr_in(const SA_IN *from, SA_IN *to);

public:
    /* exceptions */
    class InitFail : public std::exception { };

};
