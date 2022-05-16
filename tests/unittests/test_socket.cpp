#include <unittests.hpp>

#include <ServerSocket.hpp>
#include <ClientSocket.hpp>

#include <thread>
#include <chrono>

/*
 * Can only construct ServerSocket with default constructor.
 */
TEST(SocketConstruction, DefaultConstruct) {
    ServerSocket  sock;

    EXPECT_EQ(sock.getFd(), SOCKET_UNSET);
    EXPECT_EQ(sock.getAddress(), 0);
    EXPECT_EQ(sock.getPort(), 0);
}

TEST(SocketConstruction, CopyConstructServer) {
    ServerSocket    s_sock;
    ServerSocket    s_cpy(s_sock);

    EXPECT_EQ(s_sock.getFd(), s_cpy.getFd());
    EXPECT_EQ(s_sock.getAddress(), s_cpy.getAddress());
    EXPECT_EQ(s_sock.getPort(), s_cpy.getPort());
}

TEST(SocketConstruction, FromAcceptClient) {
    SA_IN   address;
    short   port;
    int     fd;

    fd = 4;
    port = 80;
    std::memset(&address, 0, sizeof(address));
    address.sin_family      = AF_INET;          /* IPv4 */
    address.sin_port        = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;       /* listen for anything 0.0.0.0 */
    ClientSocket    sock(fd, address);

    EXPECT_EQ(sock.getFd(), fd);
    EXPECT_EQ(sock.getAddress(), INADDR_ANY);
    EXPECT_EQ(sock.getPort(), port);
}

TEST(SocketConstruction, CopyConstructClient) {
    ServerSocket    c_sock;
    ServerSocket    c_cpy(c_sock);

    EXPECT_EQ(c_sock.getFd(), c_cpy.getFd());
    EXPECT_EQ(c_sock.getAddress(), c_cpy.getAddress());
    EXPECT_EQ(c_sock.getPort(), c_cpy.getPort());
}

TEST(SocketInitialization, InitServer) {
    ServerSocket    sock;
    int             port;

    port = 8080;
    sock.init("*", port);
    EXPECT_TRUE(sock.getFd() != SOCKET_UNSET);
    EXPECT_EQ(sock.getPort(), port);
    EXPECT_EQ(sock.getAddress(), 0);
}

TEST(SocketInitialization, Close) {
    ServerSocket    sock;
    int             port;

    port = 8080;
    sock.init("*", port);
    sock.close();
    EXPECT_EQ(sock.getFd(), SOCKET_UNSET);
    EXPECT_EQ(sock.getAddress(), INADDR_ANY);
}

/*
 * can fail in the following ways:
 * 1. port already in use
 * 2. invalid requested address
 */
TEST(SocketInitialization, InitServerFail) {
    ServerSocket    a;
    ServerSocket    b;
    ServerSocket    c;
    short           port;

    port = 8080;
    a.init("*", port);
    EXPECT_THROW(b.init("*", port), Socket::InitFail);
    EXPECT_THROW(c.init("www.google.com", port), Socket::InitFail);
}

/*
 * Test ServerSocket listening on port being send
 * a request and creating a new ClientSocket on
 * which to .recv and .send to.
 */
TEST(SocketConnection, AcceptRecvSend) {
    ServerSocket    a;
    int             port;
    SA_IN           addr;
    int             fd;
    char            *request_data;
    std::string     url;
    std::string     response;
    std::string     curl_response;
    ssize_t         bytes_read;

    port = 8080;
    a.init("*", port);
    url = "http://localhost:" + std::to_string(port);
    auto curl_future = std::async(sendEmptyRequest, url.c_str(), &curl_response);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    fd = a.accept(addr);
    ASSERT_TRUE(fd != SYS_ERROR);
    ClientSocket    b(fd, addr);

    request_data = b.recv(&bytes_read);
    (void)request_data;
    response += "HTTP/1.1 200 OK\r\n";
    response += "Date: Wed, 21 Oct 2015 07:28:00 GMT\r\n";
    response += "\r\n";
    response += "Hello there ;)";
    b.send(response);
    b.close();
    EXPECT_EQ(curl_future.get(), EXIT_SUCCESS);
    EXPECT_EQ(response, curl_response);
}
