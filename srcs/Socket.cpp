#include <Socket.hpp>

Socket::Socket(void) : m_fd(SOCKET_UNSET)
{
    std::memset(&m_address, 0, sizeof(SA_IN));
}

/*
 * USE WITH CAUTION
 * there can only be one Socket object pointing to an internal socket
 */
Socket::Socket(const Socket &cpy) : m_fd(cpy.m_fd)
{
    std::memset(&m_address, 0, sizeof(SA_IN));
    _cpy_addr_in(&cpy.m_address, &m_address);
}

/*
 * This constructor is used for creating
 * sockets for clients that connect. Used
 * only by ClientSocket type sockets.
 */
Socket::Socket(int fd, SA_IN& address) : m_fd(fd)
{
    std::memset(&m_address, 0, sizeof(SA_IN));
    _cpy_addr_in(&address, &m_address);
}

Socket::~Socket()
{
    if (m_fd != SOCKET_UNSET) {
        std::cout << "[DEBUG] Close socket " << m_fd << '\n';
        ::close(m_fd);
    }
}

/*
 * USE WITH CAUTION
 * there can only be one Socket object pointing to an internal socket
 */
Socket&         Socket::operator = (const Socket &sock)
{
    if (this != &sock && m_fd != sock.m_fd)
    {
        ::close(m_fd);
        std::memset(&m_address, 0, sizeof(SA_IN));
        _cpy_addr_in(&sock.m_address, &m_address);
    }
    return (*this);
}

int&            Socket::getFd(void)
{
    return (m_fd);
}

uint16_t        Socket::getPort(void) const
{
    return (ntohs(m_address.sin_port));
}

in_addr_t&      Socket::getAddress(void)
{
    return (m_address.sin_addr.s_addr);
}

void            Socket::setFd(int fd)
{
    m_fd = fd;
}

SA_IN&          Socket::getAddr(void)
{
    return (m_address);
}

void            Socket::close(void)
{
    if (m_fd != SOCKET_UNSET)
    {
        ::close(m_fd);
        m_fd = SOCKET_UNSET;
        std::memset(&m_address, 0, sizeof(SA_IN));
    }
}

void                Socket::_cpy_addr_in(const SA_IN *from, SA_IN *to)
{
    std::memcpy(to->sin_zero, from->sin_zero, 8);
    to->sin_addr = from->sin_addr;
    to->sin_family = from->sin_family;
    to->sin_port = from->sin_port;
#ifdef __APPLE__
    to->sin_len = from->sin_len;
#endif
}
