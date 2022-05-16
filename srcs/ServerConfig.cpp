#include <Config.hpp>

ServerConfig::ServerConfig(void) : m_port(PORT_UNSET), m_client_max_body_size(DFL_MAX_BODY_SIZE)
{
    m_names.push_back(DFL_SERVER_NAME);
    m_error_files = ConfigUtil::getHandle().getStatusCodeMap();
}

ServerConfig::ServerConfig(const ServerConfig &cpy) :
    m_host(cpy.m_host),
    m_port(cpy.m_port),
    m_names(cpy.m_names),
    m_routes(cpy.m_routes),
    m_error_files(cpy.m_error_files),
    m_client_max_body_size(cpy.m_client_max_body_size)
{

}

ServerConfig::~ServerConfig(void)
{
    routes_t::iterator  it;
    Route               *route;

    for ( it = m_routes.begin() ; it != m_routes.end() ; ++it )
    {
        route = (*it);
        delete route;
    }
}

ServerConfig&       ServerConfig::operator = (const ServerConfig &rhs)
{
    if (this != &rhs)
    {
        m_host = rhs.m_host;
        m_port = rhs.m_port;
        m_names = rhs.m_names;
        m_routes = rhs.m_routes;
        m_error_files = rhs.m_error_files;
        m_client_max_body_size = rhs.m_client_max_body_size;
    }
    return (*this);
}

void                ServerConfig::addName(const std::string &name)
{
    m_names.push_back(name);
}

void                ServerConfig::addRoute(Route *route)
{
    m_routes.push_back(route);
}

void                ServerConfig::setHost(const std::string &host)
{
    m_host = host;
}

void                ServerConfig::setPort(uint16_t &port)
{
    m_port = port;
}

void                ServerConfig::setClientMaxBodySize(int &size)
{
    m_client_max_body_size = size;
}

void                ServerConfig::setStatusBody(int code, const std::string &body)
{
    status_code_map_t::iterator it;

    it = m_error_files.find(code);
    if (it != m_error_files.end())
    {
        m_error_files[code] = std::make_pair(it->second.first, body);
    }
}

std::string&        ServerConfig::getHost(void)
{
    return (m_host);
}

uint16_t&           ServerConfig::getPort(void)
{
    return (m_port);
}

ServerConfig::routes_t&     ServerConfig::getRoutes(void)
{
    return (m_routes);
}

std::vector<std::string>&   ServerConfig::getNames(void)
{
    return (m_names);
}

uint32_t                    ServerConfig::getMaxBodySize(void)
{
    return (m_client_max_body_size);
}

ServerConfig::status_code_map_t&    ServerConfig::getErrorFiles(void)
{
    return (m_error_files);
}

void                ServerConfig::clearNames(void)
{
    return (m_names.clear());
}

bool                ServerConfig::hasRoutes(void)
{
    return (!m_routes.empty());
}

size_t              ServerConfig::routeAmount(void)
{
    return (m_routes.size());
}
