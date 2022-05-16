#include <FileParser.hpp>

static FileParser::OptionHttp                   option_http(FileParser::BASE);
static FileParser::OptionServer                 option_server(FileParser::HTTP);
static FileParser::OptionListen                 option_listen(FileParser::SERVER);
static FileParser::OptionClientMaxBodySize      option_client_max_body_size(FileParser::SERVER);
static FileParser::OptionErrorPage              option_error_page(FileParser::SERVER);
static FileParser::OptionServerName             option_server_name(FileParser::SERVER);
static FileParser::OptionLocation               option_location(FileParser::SERVER);
static FileParser::OptionAllowedMethods         option_allowed_methods(FileParser::LOCATION);
static FileParser::OptionRoot                   option_root(FileParser::LOCATION);
static FileParser::OptionAutoIndex              option_autoindex(FileParser::LOCATION);
static FileParser::OptionIndex                  option_index(FileParser::LOCATION);
static FileParser::OptionCgiExtension           option_cgi_extension(FileParser::LOCATION);
static FileParser::OptionUploadPath             option_upload_path(FileParser::LOCATION);
static FileParser::OptionReturn                 option_return(FileParser::LOCATION);

FileParser::Option                              *FileParser::m_option_http = &option_http;
FileParser::Option                              *FileParser::m_option_client_max_body_size = &option_client_max_body_size;
FileParser::Option                              *FileParser::m_option_error_page = &option_error_page;
FileParser::Option                              *FileParser::m_option_server = &option_server;
FileParser::Option                              *FileParser::m_option_listen = &option_listen;
FileParser::Option                              *FileParser::m_option_server_name = &option_server_name;
FileParser::Option                              *FileParser::m_option_location = &option_location;
FileParser::Option                              *FileParser::m_option_allowed_methods = &option_allowed_methods;
FileParser::Option                              *FileParser::m_option_root = &option_root;
FileParser::Option                              *FileParser::m_option_autoindex = &option_autoindex;
FileParser::Option                              *FileParser::m_option_index = &option_index;
FileParser::Option                              *FileParser::m_option_cgi_extension = &option_cgi_extension;
FileParser::Option                              *FileParser::m_option_upload_path = &option_upload_path;
FileParser::Option                              *FileParser::m_option_return = &option_return;

/*
 * Simple exception used to communicate the error caught
 * when parsing the config file fails.
 */
FileParser::MappingFailure::MappingFailure(const std::string& info) : std::exception(), _info(info)
{

}

FileParser::MappingFailure::~MappingFailure() throw()
{

}

const std::string&  FileParser::MappingFailure::info(void) const
{
    return (_info);
}

/*
 * Set paths to absolute path.
 */
FileParser::FileParser(const char *exec_path)
{
    char        abspath[PATH_MAX + 1];
    std::string abspath_s;

    if (!realpath(exec_path, abspath))
    {
        /* some error */
    }
    abspath_s += abspath;
    abspath_s = abspath_s.substr(0, abspath_s.find_last_of('/'));
    abspath_s += "/";
#ifdef TESTRUN
    /* has to do with executable located in cmake-build-debug/test/test */
    abspath_s += "../../";
#endif
    m_cwd = abspath_s;
    m_file_path = abspath_s + DFL_CONFIG_FILE_PATH;
}

FileParser::FileParser(const FileParser &cpy) :
    m_cwd(cpy.m_cwd),
    m_file_path(cpy.m_file_path)
{

}

FileParser::~FileParser(void)
{

}

FileParser&         FileParser::operator = (const FileParser &rhs)
{
    if (this != &rhs)
    {
        m_cwd = rhs.m_cwd;
        m_file_path = rhs.m_file_path;
    }
    return (*this);
}

std::string&        FileParser::getFilePath(void)
{
    return (m_file_path);
}

std::string&        FileParser::getCwd(void)
{
    return (m_cwd);
}

std::string&        FileParser::getConfigData(void)
{
    return (m_config_data);
}

void                FileParser::setFilePath(const std::string &path)
{
    m_file_path = path;
}

void                FileParser::loadFile(void)
{
    std::ifstream           file_handle;
    std::stringstream       buffer;

    file_handle.open(m_file_path.c_str());
    if (file_handle.fail())
        throw InvalidFile();

    buffer << file_handle.rdbuf();
    m_config_data = buffer.str();

    file_handle.close();
}

void                FileParser::mapTokens(config_vec_t& port_configs)
{
    std::stack<ServerConfig*>   server_blocks;
    tokens_t                    tokens;

    if (m_config_data.empty())
        throw MappingFailure("Config data is empty. Load the file first");

    /* tokenize the string */
    std::replace(m_config_data.begin(), m_config_data.end(), '\t', ' ');
    _stringReplaceMultiple(m_config_data, "\n", " \n ");
    _tokenizeString(tokens, m_config_data, " ");

    try
    {
        /*
         * parse the tokens into the ServerConfig objects
         * and group them into PortConfig objects.
         */
        _mapTokens(tokens, server_blocks);
        _groupServerConfigs(port_configs, server_blocks);
    }
    catch (const std::bad_alloc& alloc_err)
    {
        /* special case if run out of memory */

    }
}

/*
 * Gradually add PortConfig objects to the map using
 * the provided tokens read from the file.
 *
 * Will throw MappingFailure in the following cases:
 * - Invalid brackets (i.e. not closing or invalid order)
 * - Unknown keyword or keyword in wrong parse level
 * - Could not convert string if numeric argument expected
 */
void                    FileParser::_mapTokens(tokens_t& tokens, std::stack<ServerConfig*>& server_blocks)
{
    ServerConfig                    *server_block;
    Route                           *route;
    Option                          *option;
    Option::map_config_t            config;
    Option::map_config_t::iterator  it;
    std::ostringstream              error_ss;

    std::stack<char>                brackets;
    bool                            http_flag_set;
    int                             parse_level;

    parse_level = BASE;
    http_flag_set = false;
    config = _getConfigMap();
    while (!tokens.empty())
    {
        if (tokens.front() == "#")
        {
            while (!tokens.empty() && tokens.front() != "\n")
                tokens.pop_front();
        }
        else if (tokens.front() == "{")
        {
            brackets.push(tokens.front().at(0));
            parse_level++;
        }
        else if (tokens.front() == "}")
        {
            if (brackets.empty() || brackets.top() != '{')
                throw MappingFailure("Invalid brackets in config file");
            if (parse_level == HTTP && server_blocks.empty())
                throw MappingFailure("http block requires at least one server");
            if (parse_level == SERVER && !server_block->hasRoutes())
                throw MappingFailure("server block requires at least one route");
            if (parse_level == SERVER)
                server_blocks.push(server_block);
            if (parse_level == LOCATION)
                server_block->addRoute(route);
            brackets.pop();
            parse_level--;
        }
        else
        {
            /* parse potential option */
            if (!(tokens.front() == "\n"))
            {
                it = config.find(tokens.front());
                if (it == config.end())
                {
                    error_ss << "Invalid keyword '" << tokens.front() << "'";
                    throw MappingFailure(error_ss.str());
                }
                else
                {
                    option = (*it).second;
                    if (parse_level != option->getParseLevel())
                    {
                        error_ss << "Option '" << tokens.front() << "' at wrong parse level";
                        throw MappingFailure(error_ss.str());
                    }
                    switch (parse_level)
                    {
                        case (BASE):
                        case (HTTP):
                            if (tokens.front() == "server")
                            {
                                if (!http_flag_set)
                                    throw MappingFailure("http directive must be set first");
                                server_block = new ServerConfig();
                            }
                            else
                                http_flag_set = true;
                            option->parse(this, tokens);
                            break ;

                        case (SERVER):
                            if (tokens.front() == "location")
                            {
                                route = new Route();
                                option->parse(route, tokens);
                            }
                            else
                                option->parse(server_block, tokens);
                            break ;

                        case (LOCATION):
                            option->parse(route, tokens);
                            break ;

                        default:
                            break ;
                    }
                }
            }
        }
        tokens.pop_front();
    }
    if (!brackets.empty())
        throw MappingFailure("Invalid brackets in config file");
}

void                    FileParser::_groupServerConfigs(config_vec_t& port_configs, std::stack<ServerConfig*>& server_blocks)
{
    PortConfig              *port;
    ServerConfig            *server;
    config_vec_t::iterator  it;

    while (!server_blocks.empty())
    {
        server = server_blocks.top();
        for (it = port_configs.begin() ; it != port_configs.end() ; ++it )
        {
            port = (*it);
            /* add server_block to existing PortConfig if a match */
            if (port->getPort() == server->getPort())
            {
                port->getServers().push_back(server);
                break ;
            }
        }
        /*
         * Create a new PortConfig if no match, add the server block
         * to it and add the new PortConfig to the port_configs vector.
         */
        if (it == port_configs.end())
        {
            port = new PortConfig(server->getHost(),server->getPort());
            port->getServers().push_back(server);
            port_configs.push_back(port);
        }
        server_blocks.pop();
    }
}

/*
 * Split the string based on the delimiter(s) and push the
 * individual tokens into the specified container.
 */
FileParser::tokens_t&   FileParser::_tokenizeString(tokens_t& queue, std::string& str, const std::string& delims)
{
    std::string             sub;
    std::string::size_type  pos, prev;

    prev = 0;
    while ((pos = str.find_first_of(delims, prev)) != std::string::npos)
    {
        if (pos > prev)
        {
            sub = str.substr(prev, pos - prev);
            if (queue.empty() || !(sub == "\n" && queue.back() == "\n"))
                queue.push_back(str.substr(prev, (pos - prev)));
        }
        prev = (pos + 1);
    }
    if (prev < str.length())
        queue.push_back(str.substr(prev, std::string::npos));
    return (queue);
}

/*
 * Replace a substring in a string with another substring using
 * built-in string methods find() and replace().
 */
std::string&    FileParser::_stringReplaceMultiple(std::string& str, const std::string& from, const std::string& to)
{
    std::string::size_type  index;

    index = 0;
    while (true)
    {
        index = str.find(from, index);
        if (index == std::string::npos)
            break;
        str.replace(index, from.size(), to);
        index += to.size();
    }
    return (str);
}

FileParser::Option::map_config_t&   FileParser::_getConfigMap(void)
{
    static Option::map_config_t     config_map;

    if (config_map.empty())
    {
        config_map["http"] = m_option_http;
        config_map["client_max_body_size"] = m_option_client_max_body_size;
        config_map["error_page"] = m_option_error_page;
        config_map["server"] = m_option_server;
        config_map["listen"] = m_option_listen;
        config_map["server_name"] = m_option_server_name;
        config_map["location"] = m_option_location;
        config_map["allowed_methods"] = m_option_allowed_methods;
        config_map["root"] = m_option_root;
        config_map["autoindex"] = m_option_autoindex;
        config_map["index"] = m_option_index;
        config_map["cgi_extensions"] = m_option_cgi_extension;
        config_map["upload_path"] = m_option_upload_path;
        config_map["return"] = m_option_return;
    }
    return (config_map);
}
