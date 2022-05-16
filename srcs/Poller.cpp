#include <Poller.hpp>
#include <Handler.hpp>

Poller::Poller(PortConfig *port_config) :
    m_port_config(port_config),
    m_new_connection(std::make_pair(0, (Connection*)NULL))
{}

/* shallow copy of port config */
Poller::Poller(const Poller &cpy) :
    m_pfds(cpy.m_pfds),
    m_port_config(cpy.m_port_config),
    m_dropped_fds(cpy.m_dropped_fds),
    m_new_connection(cpy.m_new_connection)
{}

Poller::~Poller(void)
{
    delete m_port_config;
}

Poller&         Poller::operator = (const Poller &rhs)
{
    if (this != &rhs)
    {
        m_pfds = rhs.m_pfds;
        m_port_config = rhs.m_port_config;
        m_dropped_fds = rhs.m_dropped_fds;
        m_new_connection = rhs.m_new_connection;
    }
    return (*this);
}

Connection          *Poller::_searchCorrectConnection(int socket_fd)
{
    clients_t::iterator it;

    it = m_clients.find(socket_fd);
    return ((it != m_clients.end()) ? it->second : NULL);
}

void            *Poller::pollPort(void *instance)
{
    ConfigUtil      *util;
    Poller          *poller;
    PortConfig      *port_config;
    int             fds_with_events;
    size_t          active_connections;

    util = &ConfigUtil::getHandle();
    poller = Poller::_initPoller(&port_config, instance);
    for ( ; ; )
    {
        poller->m_new_connection = std::make_pair(0, (Connection*)NULL);

        active_connections = poller->m_pfds.size();
        fds_with_events = poll(&poller->m_pfds.front(), active_connections, POLL_TIMEOUT_MS);
        if (fds_with_events == SYS_ERROR) {
            util->setSignalled(SIGINT);
        }
        if (util->isSignalled()) {
            break ;
        }
        if (!fds_with_events) {
            continue ;
        }
        for (size_t idx = 0; idx < active_connections; ++idx)
        {
            if (!poller->m_pfds[idx].revents)
                continue ;

            if (!fds_with_events)
                break ;

            fds_with_events--;
            if (poller->m_pfds[idx].revents & (POLLHUP)) {
                poller->m_dropped_fds.push(poller->m_pfds[idx].fd);
            }
            else if (poller->m_pfds[idx].revents & (POLLIN))
            {
                if (poller->m_pfds[idx].fd == port_config->getSocket()->getFd()) {
                    poller->_getNewConnection();
                }
                else if (poller->_checkIfCGIFd(poller->m_pfds[idx].fd))
                {
                    poller->_readCGIData(poller->m_pfds[idx].fd);
                }
                else {
                    poller->_readConnectionData(poller->m_pfds[idx].fd);
                }
                usleep(POLLIN_SLEEP_MS);
            }
            else
            {
                if (poller->m_pfds[idx].revents & (POLLOUT))
                {
                    poller->_parse(poller->m_pfds[idx].fd);
                    poller->_matchRoute(poller->m_pfds[idx].fd);
                    if (poller->_checkIfCGIConnection(poller->m_pfds[idx].fd))
                    {
                        poller->_initAndExecCGI(poller->m_pfds[idx].fd);
                    }
                    else
                    {
                        poller->_respond(poller->m_pfds[idx].fd);
                        poller->m_dropped_fds.push(poller->m_pfds[idx].fd);
                    }
                }
            }
        }
        poller->_deletePollFds();
        poller->_addPollFds();
    }
    delete poller;
    return (NULL);
}

/*
 * Initialise the poller:
 * - convert (void*) to Poller*
 * - set and init port_config
 * - return new Poller instance
 */
Poller            *Poller::_initPoller(PortConfig **port_config, void *instance)
{
    Poller          *poller;
    PortConfig      *pc;
    struct pollfd   listener;

    poller = static_cast<Poller*>(instance);
    pc = poller->m_port_config;
    pc->initSocket();

    listener.fd = pc->getSocket()->getFd();
    listener.events = POLLIN;
    listener.revents = 0;
    poller->m_pfds.push_back(listener);

    *port_config = pc;
    return (poller);
}

/*
 * Update the internal vector<struct pollfd> and client map
 * - add new connection if available in m_new_connection
 * - empty m_dropped_clients and remove from vector and map
 */
void            Poller::_addPollFds(void)
{
    struct pollfd       client;
    clients_t::iterator client_it;
    Connection          *connection = NULL;

    if (m_new_connection.second)
    {
        client.fd = m_new_connection.first;
        client.events = (POLLIN | POLLOUT);
        client.revents = 0;
        m_pfds.push_back(client);
        m_clients.insert(m_new_connection);
    }
    for (client_it = m_clients.begin() ; client_it != m_clients.end() ; ++client_it)
    {
        connection = client_it->second;
        if (connection->getCGI() != NULL && connection->m_cgi_added == false)
        {
            m_pfds.push_back(connection->getCGI()->getPollFdStruct());
            connection->m_cgi_added = true;
        }
    }
}

void            Poller::_deletePollFds(void)
{
    int                 dropped_client_fd;
    clients_t::iterator client_it;

    while (!m_dropped_fds.empty())
    {
        dropped_client_fd = m_dropped_fds.top();

        /* remove from pollfd vector */
        for (std::vector<struct pollfd>::iterator it = m_pfds.begin() ; it != m_pfds.end() ; ++it)
        {
            if (it->fd == dropped_client_fd)
            {
                m_pfds.erase(it);
                break ;
            }
        }

        /* remove from client map */
        client_it = m_clients.find(dropped_client_fd);
        if (client_it != m_clients.end())
        {
            delete client_it->second;
            m_clients.erase(client_it);
        }
        m_dropped_fds.pop();
    }
}

void            Poller::_parse(int &socket_fd)
{
    Connection          *connection;

    connection = _searchCorrectConnection(socket_fd);
    connection = m_clients.find(socket_fd)->second;
    if (connection->getRequest().getHeaders().empty())
        connection->parseRequest();    
}

void            Poller::_respond(int &socket_fd)
{
    Connection                      *connection;

    connection = _searchCorrectConnection(socket_fd);
    connection->sendResponse(connection->getErrorFiles());
}

bool            Poller::_checkIfCGIConnection(int socket_fd)
{
    Connection          *connection;

    connection = _searchCorrectConnection(socket_fd);
    return (connection->getRequest().isCGI());
}

bool            Poller::_initAndExecCGI(int socket_fd)
{
    Connection          *connection;

    connection = _searchCorrectConnection(socket_fd);
    if (!connection->getCGI())
    {
        connection->initCGI();
        connection->getCGI()->setProgramPath(DFL_CGI_DIR + connection->getRequest().getFilename());
        connection->getCGI()->exec();
    }
    return (true);
}

void            Poller::_getNewConnection(void)
{
    int             fd;
    SA_IN           addr;
    ClientSocket    *socket;
    Connection      *connection;

    connection = NULL;
    fd = m_port_config->getSocket()->accept(addr);
    socket      = new ClientSocket(fd, addr);
    connection  = new Connection(socket);
    m_new_connection = std::make_pair(fd, connection);
}

void            Poller::_matchRoute(int socket_fd)
{
    Connection                      *connection;
    Route                           *matched_route;
    ConfigUtil::status_code_map_t   *error_files = NULL;
    uint32_t                         max_body_size;

    connection = _searchCorrectConnection(socket_fd);
    matched_route   = m_port_config->getMatchingRoute(connection->getRequest(), &error_files, &max_body_size);
    if (!matched_route)
    {
        connection->setErrorFiles(&ConfigUtil::getHandle().getStatusCodeMap());
    }
    else
    {
        if (connection->getRequest().getStatus() == HTTP_STATUS_OK)
        {
            connection->setRoute(matched_route);
            connection->setErrorFiles(error_files);
            if (connection->getRequest().getMethod() == "POST")
                connection->checkBodySize(max_body_size);
            connection->checkRoute();
            connection->methodHandler();
        }
    }
}

void            Poller::_readConnectionData(int &socket_fd)
{
    Connection              *connection;
    
    connection = _searchCorrectConnection(socket_fd);
    connection->readSocket();
    if (connection->getRequest().m_request.empty())
        m_dropped_fds.push(socket_fd);
}

void            Poller::_readCGIData(int socket_fd)
{
    Connection              *connection;
    clients_t::iterator     client_it;
    int                     w_status;

    for (client_it = m_clients.begin() ; client_it != m_clients.end() ; ++client_it)
    {
        if (client_it->second->getCGI() != NULL && client_it->second->getCGI()->getPipeReadFd() == socket_fd)
        {
            connection = client_it->second;
            if (!connection->getCGI()->readAndAppend())
            {
                connection->getCGI()->setDone();
                connection->getRequest().setStatus(HTTP_STATUS_INTERNAL_SERVER_ERROR);
            }
            if (connection->getCGI()->isDone())
            {
                waitpid(connection->getCGI()->getForkedPid(), &w_status, 0);
                if (WEXITSTATUS(w_status) == EXIT_FAILURE)
                {
                    connection->getRequest().setStatus(HTTP_STATUS_INTERNAL_SERVER_ERROR);
                    connection->sendResponse(connection->getErrorFiles());
                }
                else
                    connection->getSock()->send(connection->getCGI()->getResponse());
                m_dropped_fds.push(socket_fd);
                m_dropped_fds.push(connection->getSock()->getFd());
            }
            break ;
        }
    }
}

bool            Poller::_checkIfCGIFd(int socket_fd)
{
    clients_t::iterator             client_it;

    for (client_it = m_clients.begin() ; client_it != m_clients.end() ; ++client_it)
    {
        if (client_it->second->getCGI() != NULL && client_it->second->getCGI()->getPipeReadFd() == socket_fd)
        {
            return (true);
        }
    }
    return (false);
}
