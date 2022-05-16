#include <Connection.hpp>

Connection::Connection(void) :
    m_sock(NULL),
    m_route(NULL),
    m_cgi_added(false)
{
}

Connection::Connection(const Connection &cpy) :
    m_sock(NULL),
    m_route(NULL),
    m_cgi_added(false)
{
    *this = cpy;
}

Connection::Connection(ClientSocket *sock) :
    m_sock(sock),
    m_route(NULL),
    m_cgi_added(false)
{
}

Connection::~Connection(void)
{
    std::cout << "[DEBUG] Closing client connection" << '\n';
    if (m_sock)
    {
        delete m_sock;
    }
}

Connection&     Connection::operator = (const Connection &conn)
{
    if (this != &conn)
    {
        delete m_sock;
        m_sock = NULL;
        if (conn.m_sock)
            m_sock = new ClientSocket(*conn.m_sock);
        delete m_route;
        if (conn.m_route)
            m_route = new Route(*conn.m_route);
        m_request = conn.m_request;
        m_cgi = std::unique_ptr<CGI>(new CGI(*conn.m_cgi));
    }
    return (*this);
}

/*
 * ClientSocket being passed must be initialised before being
 * added to the Connection object.
 */
void            Connection::setSocket(ClientSocket *sock)
{
    m_sock = sock;
}

void       Connection::setErrorFiles(ConfigUtil::status_code_map_t *error_files)
{
    m_error_files = error_files;
}

CGI*       Connection::getCGI(void)
{
    return (m_cgi.get());
}

Request&       Connection::getRequest(void)
{
    return (m_request);
}

ClientSocket*   Connection::getSock(void)
{
    return (m_sock);
}

ConfigUtil::status_code_map_t   *Connection::getErrorFiles(void)
{
    return (m_error_files);
}

void            Connection::readSocket(void)
{
    char    *request_data;
    ssize_t bytes_read = 0;;

    if (!m_sock)
        throw NoSocketFail();
    request_data = m_sock->recv(&bytes_read);
    if (!request_data)
    {
        /* must be handled from outside */
        m_request.setStatus(HTTP_STATUS_INTERNAL_SERVER_ERROR);
        return ;
    }

    // std::cout << "[DEBUG] Read data from socket " << m_sock->getFd() << ":\n";
    // std::cout << request_data;

    m_request.appendRequestData(request_data, bytes_read);
    delete [] request_data;
}

void            Connection::parseRequest(void)
{
    if (!m_request.m_request.empty())
        m_request.parse();
    else
        m_request.setStatus(HTTP_STATUS_NO_CONTENT);
}

void            Connection::sendResponse(ConfigUtil::status_code_map_t *error_files)
{
    Response   *response;

    if (m_request.getStatus() == HTTP_STATUS_NO_CONTENT)
        return ;

    if ((m_request.getStatus() >= HTTP_STATUS_NOT_FOUND && m_request.getStatus() <= HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED)) 
    {
        response = new Response(m_request);
    }
    else
    {
        response = new Response(m_request, *m_route);
    }
    response->buildResponse(*error_files);
    m_sock->send(response->getResponse());
    delete response;
}

void            Connection::methodHandler(void)
{
    Handler    method_handler;

    if (m_request.getStatus() == HTTP_STATUS_OK)
    {
        if (m_request.getMethod() == "POST" \
        && method_handler.post_handler(m_request, m_route->getUploadPath()))
        {
            m_request.setStatus(HTTP_STATUS_CREATED);
        }
        else if (m_request.getMethod() == "DELETE" \
        && method_handler.delete_handler(m_request, m_route->getFileSearchPath()))
        {
            m_request.setStatus(HTTP_STATUS_OK);
        }
    }
}

void            Connection::close(void)
{
    if (!m_sock)
        throw NoSocketFail();
    m_sock->close();
}

void            Connection::setRoute(Route *route)
{
    m_route = route;
}

void Connection::checkRoute(void)
{
    checkAcceptedMethods();
    checkRedirects();
    if (m_route->hasAutoIndex())
        m_request.setAutoIndex(true);
    if (getRequest().getMethod() == "GET")
        checkFileSearchPath();
}

void Connection::checkAcceptedMethods(void)
{
    std::vector<std::string>::iterator it;

    if (m_request.getStatus() == HTTP_STATUS_OK)
    {
        it = m_route->getAcceptedMethods().begin();
        for ( ; it != m_route->getAcceptedMethods().end(); it++)
        {
            if (*it == m_request.getMethod())
            {
                return ;
            }
        }
        m_request.setStatus(HTTP_STATUS_METHOD_NOT_ALLOWED);
    }
}

void Connection::checkRedirects(void)
{
    if (m_request.getStatus() == HTTP_STATUS_OK)
    {
        if (m_route->getRedir() != NULL)
        {
            m_request.setStatus(m_route->getRedir()->status_code);
        }
    }
}

void Connection::checkFileSearchPath(void)
{
    if (m_route)
        m_request.setFilesearchPath(m_route->getFileSearchPath());
    if (m_request.getStatus() == HTTP_STATUS_OK)
    {
        if (m_request.getFilename().empty())
        {
            searchDefaultIndexPages();
        }
        else
        {
            if (!searchCGIExtensions())
            {
                searchFile();
            }
        }
    }
}

void Connection::checkBodySize(uint32_t max_size)
{
    uint32_t                        body_length_bytes;
    Request::headers_t::iterator    it;

    it = m_request.getHeaders().find("Content-Length");
    if (it != m_request.getHeaders().end())
    {
        body_length_bytes = std::stoi(it->second);
    }
    else
    {
        body_length_bytes = m_request.getBody().length();
    }
    if (body_length_bytes > (max_size * 1000000))
    {
        m_request.setStatus(HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE);
    }
}

void Connection::searchFile(void)
{
    std::string filepath;

    filepath.append(m_route->getFileSearchPath() + m_request.getFilename());
    if (open(filepath.c_str(), O_RDONLY) != -1)
    {
        m_request.setStatus(HTTP_STATUS_OK);
    }
    else
    {
        m_request.setStatus(HTTP_STATUS_NOT_FOUND);
    }
}

void Connection::searchDefaultIndexPages(void)
{
    std::string filepath;
    std::vector<std::string>::iterator it;

    for(it = m_route->getIndexFiles().begin(); it != m_route->getIndexFiles().end(); it++)
    {
        filepath.append(m_request.m_filesearch + *it);
        if (open(filepath.c_str(), O_RDONLY) != -1)
        {
            m_request.setStatus(HTTP_STATUS_OK);
            return ;
        }
        filepath.clear();
    }
    m_request.setStatus(HTTP_STATUS_NOT_FOUND);
}

bool Connection::searchCGIExtensions(void)
{
    std::string extension = m_request.getFilename().substr(m_request.getFilename().find('.'));
    std::vector<std::string>::iterator it;

    for(it = m_route->getCgiFileExtensions().begin(); it != m_route->getCgiFileExtensions().end(); it++)
    {
        if (extension == *it)
        {
            m_request.setStatus(HTTP_STATUS_OK);
            if (m_request.getMethod() != "GET")
                return (false);
            m_request.setCGI(true);
            return (true);
        }
    }
    m_request.setStatus(HTTP_STATUS_NOT_FOUND);
    return (false);
}

void Connection::initCGI(void)
{
    m_cgi = std::unique_ptr<CGI>(new CGI);
    m_cgi->init(m_request);
}
