#include <ClientSocket.hpp>

ClientSocket::ClientSocket(int fd, SA_IN& address) : Socket(fd, address)
{

}

ClientSocket::ClientSocket(const ClientSocket &cpy) : Socket(cpy)
{

}

ClientSocket::~ClientSocket(void)
{

}

ClientSocket&   ClientSocket::operator = (const ClientSocket &sock)
{
    Socket::operator = (sock);
    return (*this);
}

/*
 * read at max RECV_SIZE bytes from socket and
 * return pointer to new buffer to user. Buffer
 * is heap allocated and must be freed by the
 * caller of the function.
 */
char            *ClientSocket::recv(ssize_t *bytes_read)
{
    char    *buff;

    buff = new char[RECV_SIZE + 1];
    if (( *bytes_read = ::recv(getFd(), buff, RECV_SIZE, 0) ) == SYS_ERROR) {
        fprintf(stderr, "Failed to read from socket: %s\n", strerror(errno));
        return (NULL);
    }
    buff[*bytes_read] = '\0';
    // std::cout << "[DEBUG] Read " << *bytes_read << " amount of bytes from socket " << getFd() << '\n';
    return (buff);
}

void            ClientSocket::send(std::string response)
{
    if (::send(getFd(), response.data(), response.size(), 0) == SYS_ERROR) {
        fprintf(stderr, "Failed to write to socket: %s\n", strerror(errno));
    }
}
