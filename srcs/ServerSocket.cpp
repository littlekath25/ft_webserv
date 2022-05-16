#include <ServerSocket.hpp>

ServerSocket::ServerSocket(void) : Socket()
{

}

ServerSocket::ServerSocket(const ServerSocket &cpy) : Socket(cpy)
{

}

ServerSocket::~ServerSocket(void)
{

}

ServerSocket&   ServerSocket::operator = (const ServerSocket &sock)
{
    Socket::operator = (sock);
    return (*this);
}

void        ServerSocket::init(const std::string& host, uint16_t port)
{
    A_INFO      *server_info;
    A_INFO      *result;
    std::string service;
    std::string name;
    int         sock_fd;

    name = host;
    if (name == DFL_SERVER_HOST)
        name = "0.0.0.0";
    service = ft::intToString(port);
    if (!( result = _get_addr_info(name.c_str(), service.c_str(), &server_info ))) {
        freeaddrinfo(server_info);
        throw InitFail();
    }

    /* create socket object using getaddrinfo data */
    if (( sock_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol) ) == SYS_ERROR) {
        freeaddrinfo(server_info);
        throw InitFail();
    }
    setFd(sock_fd);

    int yes = 1;
    /* prevent the kernel from hogging the port after it's closed */
    if (setsockopt(getFd(), SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == SYS_ERROR) {
        freeaddrinfo(server_info);
        throw InitFail();
    }

    /* bind address to nameless socket */
    if (bind(getFd(), result->ai_addr, result->ai_addrlen) == SYS_ERROR) {
        fprintf(stderr, "Failed to bind to socket: %s\n", std::strerror(errno));
        freeaddrinfo(server_info);
        throw InitFail();
    }

    /* set the socket to be non-blocking so recv() and send() functions don't block */
#ifndef TESTRUN
    if (fcntl(getFd(), F_SETFL, O_NONBLOCK) == SYS_ERROR) {
        freeaddrinfo(server_info);
        throw InitFail();
    }
#endif

    if (listen(getFd(), DFL_BACKLOG) == SYS_ERROR) {
        freeaddrinfo(server_info);
        throw InitFail();
    }
    Socket::_cpy_addr_in((SA_IN *)result->ai_addr, &getAddr());
    freeaddrinfo(server_info);
}

A_INFO      *ServerSocket::_get_addr_info(const char *name, const char *service, A_INFO **server_info)
{
    int     status;
    A_INFO  hints, *result;

    std::memset(&hints, 0, sizeof(A_INFO));
    hints.ai_family     = AF_UNSPEC;    /* don't care if IPv4 or IPv6 */
    hints.ai_socktype   = SOCK_STREAM;
    hints.ai_flags      = AI_PASSIVE;   /* fill in my IP for me */

    if (( status = getaddrinfo(name, service, &hints, server_info )) == SYS_ERROR) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return (NULL);
    }

    /* find the first IPv4 address and return the pointer */
    for (result = *server_info; result != NULL; result = result->ai_next) {
        if (result->ai_family == AF_INET) {
            break ;
        }
    }
    return (result);
}

int         ServerSocket::accept(SA_IN& addr)
{
    int sock_fd;

    if (getFd() == SOCKET_UNSET) {
        throw UninitialisedFail();
    }
    int len = sizeof(addr);
    if (( sock_fd = ::accept(getFd(), (SA *)&addr, (socklen_t *)&len) ) == SYS_ERROR) {
        fprintf(stderr, "Failed to accept new connection: %s\n", std::strerror(errno));
    }
    if (fcntl(getFd(), F_SETFL, O_NONBLOCK) == SYS_ERROR)
    {
        fprintf(stderr, "[ERROR] Fcntl fail\n");
        std::exit(EXIT_FAILURE);
    }

    int enable = 1;
    if (setsockopt(getFd(), SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        fprintf(stderr, "[ERROR] Setsockopt fail\n");
        std::exit(EXIT_FAILURE);
    }
    std::cout << "[DEBUG] Creating server socket " << sock_fd << '\n';
    return (sock_fd);
}
