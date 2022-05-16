#include <Request.hpp>

Request::Request(void) :
    m_target(DFL_TARGET),
    m_status(HTTP_STATUS_OK),
    m_port(80),
    m_cgi(false),
    m_autoindex(false)
{}

Request::Request(const Request &copy)
{
    m_status = copy.m_status;
    m_target = copy.m_target;
    m_query = copy.m_query;
    m_method = copy.m_method;
    m_version = copy.m_version;
    m_request = copy.m_request;
    m_headers = copy.m_headers;
    m_port = copy.m_port;
    m_body = copy.m_body;
    m_cgi = copy.m_cgi;
    m_filename = copy.m_filename;
    m_filesearch = copy.m_filesearch;
    m_autoindex = copy.m_autoindex;
}

Request::~Request(void)
{}

Request & Request::operator = (const Request &copy)
{
    if (this != &copy)
    {
        m_status = copy.m_status;
        m_target = copy.m_target;
        m_query = copy.m_query;
        m_method = copy.m_method;
        m_version = copy.m_version;
        m_request = copy.m_request;
        m_headers = copy.m_headers;
        m_port = copy.m_port;
        m_body = copy.m_body;
        m_cgi = copy.m_cgi;
        m_filename = copy.m_filename;
        m_filesearch = copy.m_filesearch;
        m_autoindex = copy.m_autoindex;
    }
    return (*this);
}

void        Request::parse(void)
{
    std::cout << "[DEBUG] Building request object\n" << '\n';
    parseAndSetStartLine();
    parseAndSetHeaders();
    checkIfChunkedRequest();
    errorChecking();
    m_request.clear();
}

void        Request::appendRequestData(const char *data, ssize_t bytes_read)
{
    m_request.append(data, bytes_read);
}

void Request::setFilesearchPath(std::string path)
{
    m_filesearch = path;
}

void Request::setStatus(int status)
{
    m_status = status;
}

void Request::setCGI(bool status)
{
    m_cgi = status;
}

void Request::setAutoIndex(bool status)
{
    m_autoindex = status;
}

int	&Request::getStatus(void)
{
    return (m_status);
}

std::string	&Request::getTarget(void)
{
    return (m_target);
}

std::string	&Request::getQuery(void)
{
    return (m_query);
}

std::string	&Request::getMethod(void)
{
    return (m_method);
}

std::string	&Request::getVersion(void)
{
    return (m_version);
}

std::string &Request::getFilename(void)
{
    return (m_filename);
}

std::string &Request::getCGIPath(void)
{
    return (m_cgi_path);
}

int	&Request::getPort(void)
{
    return (m_port);
}

bool &Request::isCGI(void)
{
    return (m_cgi);
}

bool &Request::isAutoIndex(void)
{
    return (m_autoindex);
}

std::map<std::string, std::string>	&Request::getHeaders(void)
{
    return (m_headers);
}

std::string	&Request::getBody(void)
{
    return (m_body);
}

