#include <unittests.hpp>
#include <ServerSocket.hpp>
#include <ClientSocket.hpp>

#include <map>

#include <sys/poll.h>

#define private public

#include <Connection.hpp>

Route   *get_matching_route(void)
{
    Route   *route;

    route = new Route();
    return (route);
}

TEST(ConnectionConstruction, Default) {
    Connection  *conn;

    conn = new Connection();
    EXPECT_TRUE(!conn->m_sock);
	delete conn;
}

TEST(ConnectionChecking, SinglePOLLINAndPOLLOUT) {
    ServerSocket    server_socket;
    int             server_port;
    std::string     server_url;

    ClientSocket    *client_socket;
    int             client_fd;
    SA_IN           client_addr;

    std::string     curl_response;
    struct pollfd   pollfds[1];

    ConfigUtil      *util;
    Connection      *conn;
    Route           *matching_route;
    ConfigUtil::status_code_map_t   error_files;

    util = &ConfigUtil::getHandle();
    error_files = util->getStatusCodeMap();

    server_port = 8080;
    server_url = "http://localhost:" + std::to_string(server_port);

    server_socket.init("*", server_port);
    auto curl_future = std::async(sendEmptyRequest, server_url.c_str(), &curl_response);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    client_fd = server_socket.accept(client_addr);

    client_socket = new ClientSocket(client_fd, client_addr);
    conn = new Connection(client_socket);

    matching_route = get_matching_route();
    conn->setRoute(matching_route);

    pollfds->fd = client_fd;
    pollfds->events = (POLLIN | POLLOUT);
    do {
        /* block until ready to read from ClientSocket */
        EXPECT_TRUE(poll(pollfds, 1, -1) != SYS_ERROR);
    }
    while (!(pollfds->revents & POLLIN));

    conn->readSocket();

    do {
        /* block until ready to write to ClientSocket */
        EXPECT_TRUE(poll(pollfds, 1, -1) != SYS_ERROR);
    }
    while (!(pollfds->revents & POLLOUT));

    conn->sendResponse(&error_files);
    conn->close();
    delete conn;
    delete matching_route;
    EXPECT_EQ(curl_future.get(), EXIT_SUCCESS);
}

TEST(ConnectionChecking, MultiplePOLLINAndPOLLOUT) {
    ServerSocket    server_socket;
    int             server_port;
    std::string     server_url;

    ClientSocket    *client_socket;
    int             client_fd;
    SA_IN           client_addr;

    std::string     curl_response;
    struct pollfd   pollfds[1];

    ConfigUtil      *util;
    Connection      *conn;
    Route           *matching_route;
    ConfigUtil::status_code_map_t   error_files;

    util = &ConfigUtil::getHandle();
    error_files = util->getStatusCodeMap();

    server_port = 8080;
    server_url = "http://localhost:" + std::to_string(server_port);

    server_socket.init("*", server_port);
    auto curl_future = std::async(sendBigRequest, server_url.c_str(), &curl_response);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    client_fd = server_socket.accept(client_addr);

    client_socket = new ClientSocket(client_fd, client_addr);
    conn = new Connection(client_socket);

    matching_route = get_matching_route();
    conn->setRoute(matching_route);

    pollfds->fd = client_fd;
    pollfds->events = (POLLIN | POLLOUT);
    do {
        /* block until ready to read from ClientSocket */
        EXPECT_TRUE(poll(pollfds, 1, -1) != SYS_ERROR);
    }
    while (!(pollfds->revents & POLLIN));

    conn->readSocket();

    do {
        /* block until ready to read from ClientSocket */
        EXPECT_TRUE(poll(pollfds, 1, -1) != SYS_ERROR);
    }
    while (!(pollfds->revents & POLLIN));

    conn->readSocket();

    do {
        /* block until ready to write to ClientSocket */
        EXPECT_TRUE(poll(pollfds, 1, -1) != SYS_ERROR);
    }
    while (!(pollfds->revents & POLLOUT));

    conn->sendResponse(&error_files);
    conn->close();
    delete conn;
    delete matching_route;
    EXPECT_EQ(curl_future.get(), EXIT_SUCCESS);
}
