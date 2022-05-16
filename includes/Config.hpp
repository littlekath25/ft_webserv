#pragma once

#include <Webserv.hpp>
#include <ConfigUtil.hpp>
#include <ServerSocket.hpp>
#include <Connection.hpp>
#include <Route.hpp>

#define DFL_MAX_BODY_SIZE   1       /* in Megabytes */
#define DFL_SERVER_NAME     ""

#define PORT_UNSET          0
#define POLL_TIMEOUT_MS     500
#define POLLIN_SLEEP_MS     2000

/*
 * A ServerConfig object encapsulates a server
 * block and its settings in the configuration file.
 */
class ServerConfig
{
public:
    typedef ConfigUtil::status_code_map_t   status_code_map_t;
    typedef std::vector<Route*>             routes_t;

private:
    /* configuration options */
    std::string                 m_host;
    uint16_t                    m_port;
    std::vector<std::string>    m_names;
    routes_t                    m_routes;
    status_code_map_t           m_error_files;
    uint32_t                    m_client_max_body_size;

public:
    ServerConfig(void);
    ServerConfig(const ServerConfig& cpy);
    ~ServerConfig(void);

    ServerConfig&               operator = (const ServerConfig& rhs);

    void                        addName(const std::string& name);
    void                        addRoute(Route *route);
    void                        setHost(const std::string& host);
    void                        setPort(uint16_t& port);
    void                        setClientMaxBodySize(int& size);
    void                        setStatusBody(int code, const std::string &body);

    std::string&                getHost(void);
    uint16_t&                   getPort(void);
    routes_t&                   getRoutes(void);
    std::vector<std::string>&   getNames(void);
    status_code_map_t&          getErrorFiles(void);
    uint32_t                    getMaxBodySize(void);

    void                        clearNames(void);
    bool                        hasRoutes(void);
    size_t                      routeAmount(void);

};

/*
 * A PortConfig encapsulates a ServerSocket and
 * its corresponding ServerConfig(s).
 */
class PortConfig
{
public:
    typedef std::vector<struct pollfd>      pollfd_t;
    typedef std::vector<ServerConfig*>      server_t;
    typedef std::map<int, Connection*>      clients_t;

private:
    ServerSocket                *m_sock;
    server_t                    m_server_blocks;

    std::string                 m_host;
    uint16_t                    m_port;

private:
    PortConfig(void);

public:
    PortConfig(const std::string& host, uint16_t& port);
    PortConfig(const PortConfig& cpy);
    ~PortConfig(void);

    PortConfig&                 operator = (const PortConfig& rhs);

    void                        initSocket(void);
    ServerSocket                *getSocket(void);

    uint16_t                    getPort(void);
    server_t&                   getServers(void);

    /*
     * Function that matches a Request object to a Route and sets
     * the error_files pointer to the corresponding ServerConfig its
     * custom error file map.
     */
    Route                       *getMatchingRoute(Request& request, ConfigUtil::status_code_map_t **error_files, uint32_t *max_body_size);

private:
    ServerConfig                *_getMatchingServerBlock(Request& request, ConfigUtil::status_code_map_t **error_files, uint32_t *max_body_size);

};
