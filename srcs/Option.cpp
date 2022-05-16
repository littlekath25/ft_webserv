#include <FileParser.hpp>

/*
 * ( Option )
 */
FileParser::Option::Option(int parse_level) : m_parse_level(parse_level)
{
    
}

FileParser::Option::Option(const Option &cpy) : m_parse_level(cpy.m_parse_level)
{
    
}

FileParser::Option::~Option(void)
{
    
}

FileParser::Option&     FileParser::Option::operator = (const Option &rhs)
{
    if (this != &rhs)
        m_parse_level = rhs.m_parse_level;
    return (*this);
}

int                     FileParser::Option::getParseLevel(void)
{
    return (m_parse_level);
}

/*
 *                      ( OptionHttp)
 * Must be provided at least once for the config file since
 * all options are wrapped inside of it.
 */
FileParser::OptionHttp::OptionHttp(int parse_level) : Option(parse_level)
{

}

FileParser::OptionHttp::OptionHttp(const OptionHttp &cpy) : Option(cpy)
{

}

FileParser::OptionHttp::~OptionHttp(void)
{

}

FileParser::OptionHttp&     FileParser::OptionHttp::operator = (const FileParser::OptionHttp &rhs)
{
    Option::operator = (rhs);
    return (*this);
}

/* should sign the start of an http block. TODO there can only be one http block in the Config */
void                        FileParser::OptionHttp::parse(void *obj, tokens_t& tokens)
{
    (void)obj;
    (void)tokens;
}

/*
*                  ( OptionClientMaxBodySize )
 * Can change the value of Option::m_client_max_body_size
 */
FileParser::OptionClientMaxBodySize::OptionClientMaxBodySize(int parse_level) : Option(parse_level)
{

}

FileParser::OptionClientMaxBodySize::OptionClientMaxBodySize(const OptionClientMaxBodySize &cpy) : Option(cpy)
{

}

FileParser::OptionClientMaxBodySize::~OptionClientMaxBodySize(void)
{

}

FileParser::OptionClientMaxBodySize&    FileParser::OptionClientMaxBodySize::operator = (const OptionClientMaxBodySize &rhs)
{
    Option::operator = (rhs);
    return (*this);
}

void        FileParser::OptionClientMaxBodySize::parse(void *obj, tokens_t &tokens)
{
    ServerConfig    *server;
    int             size;

    (void)obj;
    if (tokens.size() < 2)
        throw MappingFailure("failed to parse client_max_body_size: Not enough arguments.");
    tokens.pop_front();
    server = (ServerConfig *)obj;
    size = ft::stringToInt(tokens.front());
    if (size == FAIL_CONVERSION)
        throw MappingFailure("Failed to parse client_max_body_size: Invalid size.");
    server->setClientMaxBodySize(size);
}

FileParser::OptionErrorPage::OptionErrorPage(int parse_level) : Option(parse_level)
{

}

FileParser::OptionErrorPage::OptionErrorPage(const OptionErrorPage &cpy) : Option(cpy)
{

}

FileParser::OptionErrorPage::~OptionErrorPage(void)
{

}

FileParser::OptionErrorPage&     FileParser::OptionErrorPage::operator = (const OptionErrorPage &rhs)
{
    Option::operator = (rhs);
    return (*this);
}

/* expects a Config* object and two arguments */
void            FileParser::OptionErrorPage::parse(void *obj, tokens_t &tokens)
{
    ServerConfig    *server;
    std::string     file_contents;
    std::ifstream   file_stream;
    int             status_code;

    server = (ServerConfig *)obj;
    tokens.pop_front();
    if (tokens.size() < 2)
        throw MappingFailure("Failed to parse error_page: Not enough arguments.");
    status_code = ft::stringToInt(tokens.front());
    if (status_code == FAIL_CONVERSION)
        throw MappingFailure("Failed to parse error_page: Invalid status_code.");
    tokens.pop_front();
    file_contents = ft::readFileContent(file_stream, tokens.front());
    if (!file_stream.good())
        std::cout << "[WARNING] Failed to load file: " << tokens.front() << '\n';
    else
        server->setStatusBody(status_code, file_contents);
}

FileParser::OptionServer::OptionServer(int parse_level) : Option(parse_level)
{

}

FileParser::OptionServer::OptionServer(const OptionServer &cpy) : Option(cpy)
{

}

FileParser::OptionServer::~OptionServer(void)
{

}

FileParser::OptionServer&   FileParser::OptionServer::operator = (const OptionServer &rhs)
{
    Option::operator = (rhs);
    return (*this);
}

void                        FileParser::OptionServer::parse(void *obj, tokens_t &tokens)
{
    (void)obj;
    (void)tokens;
}

FileParser::OptionListen::OptionListen(int parse_level) : Option(parse_level)
{

}

FileParser::OptionListen::OptionListen(const OptionListen &cpy) : Option(cpy)
{

}

FileParser::OptionListen::~OptionListen(void)
{

}

FileParser::OptionListen&     FileParser::OptionListen::operator = (const OptionListen &rhs)
{
    Option::operator = (rhs);
    return (*this);
}

/*
 * Sets the address and port for IP
 *
 * DEFAULTS to *:80
 *
 * Valid combinations are:
 * - *:<port>
 * - <address>:<port>
 * - <address>
 * - <port>
 *
 */
void                            FileParser::OptionListen::parse(void *obj, tokens_t &tokens)
{
    ServerConfig        *server;
    std::string         host;
    uint16_t            port;

    if (tokens.size() < 2)
        throw MappingFailure("Failed to parse listen directive: Not enough arguments");
    tokens.pop_front();
    server = (ServerConfig*)obj;
    if (server->getPort() != PORT_UNSET)
        throw MappingFailure("You can only specify `listen` ones per server block");
    try
    {
        _parseArg(tokens.front(), host, &port);
        server->setHost(host);
        server->setPort(port);
    }
    catch (const MappingFailure& e)
    {
        /* Failed to parse arguments */
    }
    catch (const std::runtime_error& e)
    {
        /* failed to create socket */
    }
}

void                            FileParser::OptionListen::_parseArg(const std::string &arg, std::string& address, uint16_t *sin_port)
{
    std::string             arg_cpy;
    std::stringstream       arg_ss;
    std::ostringstream      error_ss;
    std::string::size_type  pos;

    arg_ss << arg;
    arg_cpy = arg_ss.str();
    arg_ss.exceptions( std::ios::failbit | std::ios::badbit );
    switch (ft::count(arg_cpy.begin(), arg_cpy.end(), ':'))
    {
        case 0:
            try
            {
                arg_ss >> *sin_port;
                address += DFL_SERVER_HOST;
            }
            catch (const std::ios::failure& e)
            {
                *sin_port = DFL_SERVER_PORT;
                address += arg_ss.str();
            }
            break ;

        case 1:
            pos = arg_cpy.find(':');
            arg_ss.seekg(pos + 1);
            try
            {
                arg_ss >> *sin_port;
                address += arg_cpy.substr(0, pos);
            }
            catch (const std::ios::failure& e)
            {
                error_ss << "Failed to convert port in arg: " << arg_cpy;
                throw MappingFailure(error_ss.str());
            }
            break ;

        default:
            error_ss << "Directive " << arg_cpy << " is invalid";
            throw MappingFailure(error_ss.str());
    }
}

FileParser::OptionServerName::OptionServerName(int parse_level) : Option(parse_level)
{

}

FileParser::OptionServerName::OptionServerName(const OptionServerName &cpy) : Option(cpy)
{

}

FileParser::OptionServerName::~OptionServerName(void)
{

}

FileParser::OptionServerName&       FileParser::OptionServerName::operator = (const OptionServerName &rhs)
{
    Option::operator = (rhs);
    return (*this);
}

void                                FileParser::OptionServerName::parse(void *obj, tokens_t &tokens)
{
    ServerConfig  *server;

    tokens.pop_front();
    server = (ServerConfig*)obj;
    server->clearNames();
    while (!tokens.empty() && !(tokens.front() == "\n"))
    {
        server->addName(tokens.front());
        tokens.pop_front();
    }
    if (tokens.empty())
        throw MappingFailure("Invalid config file");
}

FileParser::OptionLocation::OptionLocation(int parse_level) : Option(parse_level)
{

}

FileParser::OptionLocation::OptionLocation(const OptionLocation &cpy) : Option(cpy)
{

}

FileParser::OptionLocation::~OptionLocation(void)
{

}

FileParser::OptionLocation&     FileParser::OptionLocation::operator = (const OptionLocation &rhs)
{
    Option::operator = (rhs);
    return (*this);
}

void                            FileParser::OptionLocation::parse(void *obj, tokens_t &tokens)
{
    Route   *route;

    if (tokens.size() < 2)
        throw MappingFailure("failed to parse location directive: Not enough arguments");
    tokens.pop_front();
    route = (Route *)obj;
    route->setBaseUrl(tokens.front());
}

FileParser::OptionAllowedMethods::OptionAllowedMethods(int parse_level) : Option(parse_level)
{

}

FileParser::OptionAllowedMethods::OptionAllowedMethods(const OptionAllowedMethods &cpy) : Option(cpy)
{

}

FileParser::OptionAllowedMethods::~OptionAllowedMethods(void)
{

}

FileParser::OptionAllowedMethods&       FileParser::OptionAllowedMethods::operator = (const OptionAllowedMethods &rhs)
{
    Option::operator = (rhs);
    return (*this);
}

void                                    FileParser::OptionAllowedMethods::parse(void *obj, tokens_t &tokens)
{
    Route                           *route;
    int                             count;
    const ConfigUtil::methods_t     *methods;

    count = 0;
    tokens.pop_front();
    route = (Route *)obj;
    route->getAcceptedMethods().clear();
    methods = &ConfigUtil::getHandle().getDefaultMethods();
    while (!tokens.empty() && !(tokens.front() == "\n" || count >= 3))
    {
        if (std::find(methods->begin(), methods->end(), tokens.front()) == methods->end())
            throw MappingFailure("Failed to parse allowed_methods");
        route->getAcceptedMethods().push_back(tokens.front());
        tokens.pop_front();
        count++;
    }
    if (tokens.empty())
        throw MappingFailure("Invalid config file");
}

FileParser::OptionRoot::OptionRoot(int parse_level) : Option(parse_level)
{

}

FileParser::OptionRoot::OptionRoot(const OptionRoot &cpy) : Option(cpy)
{

}

FileParser::OptionRoot::~OptionRoot(void)
{

}

FileParser::OptionRoot&     FileParser::OptionRoot::operator = (const OptionRoot &rhs)
{
    Option::operator = (rhs);
    return (*this);
}

/* should sign the start of an http block. TODO there can only be one http block in the Config */
void                        FileParser::OptionRoot::parse(void *obj, tokens_t &tokens)
{
    Route   *route;

    tokens.pop_front();
    route = (Route *)obj;
    if (tokens.front() == "/")
    {
        route->setFileSearchPath(DFL_FILE_SEARCH_PATH);
    }
    else
    {
        if (tokens.front().at(0) == '/')
            tokens.front().erase(0, 1);
        else if (tokens.front().at(0) == '.' && tokens.front().at(1) == '/')
            tokens.front().erase(0, 2);
        route->setFileSearchPath(DFL_FILE_SEARCH_PATH + tokens.front());
    }
}

FileParser::OptionAutoIndex::OptionAutoIndex(int parse_level) : Option(parse_level)
{

}

FileParser::OptionAutoIndex::OptionAutoIndex(const OptionAutoIndex &cpy) : Option(cpy)
{

}

FileParser::OptionAutoIndex::~OptionAutoIndex(void)
{

}

FileParser::OptionAutoIndex&     FileParser::OptionAutoIndex::operator = (const OptionAutoIndex &rhs)
{
    Option::operator = (rhs);
    return (*this);
}

/* should sign the start of an http block. TODO there can only be one http block in the Config */
void                            FileParser::OptionAutoIndex::parse(void *obj, tokens_t &tokens)
{
    Route   *route;

    tokens.pop_front();
    route = (Route *)obj;
    if (!(tokens.front() == "on" || tokens.front() == "off"))
        throw MappingFailure("Failed to parse autoindex directive: Invalid argument.");
    route->setAutoIndex(tokens.front() == "on");
}

FileParser::OptionIndex::OptionIndex(int parse_level) : Option(parse_level)
{

}

FileParser::OptionIndex::OptionIndex(const OptionIndex &cpy) : Option(cpy)
{

}

FileParser::OptionIndex::~OptionIndex(void)
{

}

FileParser::OptionIndex&    FileParser::OptionIndex::operator = (const OptionIndex &rhs)
{
    Option::operator = (rhs);
    return (*this);
}

void                        FileParser::OptionIndex::parse(void *obj, tokens_t &tokens)
{
    Route   *route;

    tokens.pop_front();
    route = (Route *)obj;
    route->getIndexFiles().clear();
    while (!tokens.empty() && tokens.front() != "\n")
    {
        route->getIndexFiles().push_back(tokens.front());
        tokens.pop_front();
    }
    if (tokens.empty())
        throw MappingFailure("Invalid config file");
}

FileParser::OptionCgiExtension::OptionCgiExtension(int parse_level) : Option(parse_level)
{

}

FileParser::OptionCgiExtension::OptionCgiExtension(const OptionCgiExtension &cpy) : Option(cpy)
{

}

FileParser::OptionCgiExtension::~OptionCgiExtension(void)
{

}

FileParser::OptionCgiExtension&     FileParser::OptionCgiExtension::operator = (const OptionCgiExtension &rhs)
{
    Option::operator = (rhs);
    return (*this);
}

void            FileParser::OptionCgiExtension::parse(void *obj, tokens_t &tokens)
{
    Route   *route;

    tokens.pop_front();
    route = (Route *)obj;
    while (!tokens.empty() && tokens.front() != "\n")
    {
        route->getCgiFileExtensions().push_back(tokens.front());
        tokens.pop_front();
    }
    if (tokens.empty())
        throw MappingFailure("Invalid config file");
}

FileParser::OptionUploadPath::OptionUploadPath(int parse_level) : Option(parse_level)
{

}

FileParser::OptionUploadPath::OptionUploadPath(const OptionUploadPath &cpy) : Option(cpy)
{

}

FileParser::OptionUploadPath::~OptionUploadPath(void) { }

FileParser::OptionUploadPath&     FileParser::OptionUploadPath::operator = (const OptionUploadPath &rhs)
{
    Option::operator = (rhs);
    return (*this);
}

void            FileParser::OptionUploadPath::parse(void *obj, tokens_t &tokens)
{
    Route               *route;
    DIR                 *upload_dir;
    std::string         upload_path;
    std::ostringstream  error_ss;

    tokens.pop_front();
    route = (Route *)obj;
    if (tokens.front() == "/")
    {
        upload_path = DFL_UPLOAD_PATH;
    }
    else
    {
        if (tokens.front().at(0) == '/')
            tokens.front().erase(0, 1);
        else if (tokens.front().at(0) == '.' && tokens.front().at(1) == '/')
            tokens.front().erase(0, 2);
        upload_path = DFL_UPLOAD_PATH + tokens.front();
    }
    upload_dir = opendir(upload_path.c_str());
    if (upload_dir)
        closedir(upload_dir);
    else if (errno == ENOENT)
    {
        error_ss << "Directory " << upload_path << " does not exist.";
        throw MappingFailure(error_ss.str());
    }
    else
    {
        error_ss << "Invalid directory " << tokens.front() << ": " << strerror(errno) << '\n';
        throw MappingFailure(error_ss.str());
    }
    route->setUploadPath(upload_path);
}

FileParser::OptionReturn::OptionReturn(int parse_level) : Option(parse_level)
{

}

FileParser::OptionReturn::OptionReturn(const OptionReturn &cpy) : Option(cpy)
{

}

FileParser::OptionReturn::~OptionReturn(void)
{

}

FileParser::OptionReturn&     FileParser::OptionReturn::operator = (const OptionReturn &rhs)
{
    Option::operator = (rhs);
    return (*this);
}

void            FileParser::OptionReturn::parse(void *obj, tokens_t &tokens)
{
    Route           *route;
    int             status_code;

    tokens.pop_front();
    route = (Route *)obj;
    if (tokens.size() < 2)
        throw MappingFailure("Failed to parse return: Not enough arguments.");
    status_code = ft::stringToInt(tokens.front());
    if (status_code == FAIL_CONVERSION)
        throw MappingFailure("Failed to parse return: Invalid status_code.");
    tokens.pop_front();
    route->setRedirect(status_code, tokens.front());
}
