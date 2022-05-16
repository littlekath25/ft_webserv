#include <ConfigUtil.hpp>

ConfigUtil  *ConfigUtil::m_handle = NULL;

ConfigUtil::ConfigUtil(void) : m_signal(0)
{
    /*
     * use DFL_ERROR_PAGES_PATH from project directory to find .html files
     * - files are named <status_code>.html
     * - if file not found ignore it
     * - read each file into std::string and update
     * - Server has `m_error_pages` for custom pages
     * - If an error_page is request, Server first checks itself, then Config::getStatusCodeMap
     * - Config has static std::map from getStatusCodeMap for defaults
     * - this Config::Config sets defaults in std::map in Config
     * - while loading custom error_pages into Server.m_error_pages give
     *   warning if unable to find file
     */
    _setDefaultMethods();
    _setDefaultStatusCodes();
    _loadDefaultErrorFiles();
    pthread_mutex_init( &m_signal_lock, NULL );
}

ConfigUtil::~ConfigUtil(void)
{
    delete ConfigUtil::m_handle;
}

ConfigUtil&                             ConfigUtil::getHandle(void)
{
    if (!m_handle) m_handle = new ConfigUtil;
    return (*m_handle);
}

const ConfigUtil::methods_t&            ConfigUtil::getDefaultMethods(void)
{
    return (m_methods);
}

ConfigUtil::status_code_map_t&          ConfigUtil::getStatusCodeMap(void)
{
    return (m_status_codes);
}

void                                    ConfigUtil::_setDefaultMethods(void)
{
    m_methods.push_back("GET");
    m_methods.push_back("POST");
    m_methods.push_back("DELETE");
}

void                                    ConfigUtil::_setDefaultStatusCodes(void)
{
    //------ 1xx - Informational ------
    m_status_codes.insert(std::make_pair(100, std::make_pair("Continue", "")));
    m_status_codes.insert(std::make_pair(101, std::make_pair("Switching Protocols", "")));
    m_status_codes.insert(std::make_pair(102, std::make_pair("Processing", "")));
    m_status_codes.insert(std::make_pair(103, std::make_pair("Early Hints", "")));

    //------ 2xx - Successful ------
    m_status_codes.insert(std::make_pair(200, std::make_pair("OK", "")));
    m_status_codes.insert(std::make_pair(201, std::make_pair("Created", "")));
    m_status_codes.insert(std::make_pair(202, std::make_pair("Accepted", "")));
    m_status_codes.insert(std::make_pair(203, std::make_pair("Non-Authoritative Information", "")));
    m_status_codes.insert(std::make_pair(204, std::make_pair("No Content", "")));
    m_status_codes.insert(std::make_pair(205, std::make_pair("Reset Content", "")));
    m_status_codes.insert(std::make_pair(206, std::make_pair("Partial Content", "")));
    m_status_codes.insert(std::make_pair(207, std::make_pair("Multi-Status", "")));
    m_status_codes.insert(std::make_pair(208, std::make_pair("Already Reported", "")));
    m_status_codes.insert(std::make_pair(226, std::make_pair("Im Used", "")));

    //------ 3xx - Redirection ------
    m_status_codes.insert(std::make_pair(300, std::make_pair("Multiple Choices", "")));
    m_status_codes.insert(std::make_pair(301, std::make_pair("Moved Permanently", "")));
    m_status_codes.insert(std::make_pair(302, std::make_pair("Found", "")));
    m_status_codes.insert(std::make_pair(303, std::make_pair("See Other", "")));
    m_status_codes.insert(std::make_pair(304, std::make_pair("Not Modified", "")));
    m_status_codes.insert(std::make_pair(305, std::make_pair("Use Proxy", "")));
    m_status_codes.insert(std::make_pair(307, std::make_pair("Temporary Redirect", "")));
    m_status_codes.insert(std::make_pair(308, std::make_pair("Permanent Redirect", "")));

    //------ 4xx - Client Error ------
    m_status_codes.insert(std::make_pair(400, std::make_pair("Bad Request", "")));
    m_status_codes.insert(std::make_pair(401, std::make_pair("Unauthorized", "")));
    m_status_codes.insert(std::make_pair(402, std::make_pair("Payment Required", "")));
    m_status_codes.insert(std::make_pair(403, std::make_pair("Forbidden", "")));
    m_status_codes.insert(std::make_pair(404, std::make_pair("Not Found", "")));
    m_status_codes.insert(std::make_pair(405, std::make_pair("Method Not Allowed", "")));
    m_status_codes.insert(std::make_pair(406, std::make_pair("Not Acceptable", "")));
    m_status_codes.insert(std::make_pair(407, std::make_pair("Proxy Authentication Required", "")));
    m_status_codes.insert(std::make_pair(408, std::make_pair("Request Timeout", "")));
    m_status_codes.insert(std::make_pair(409, std::make_pair("Conflict", "")));
    m_status_codes.insert(std::make_pair(410, std::make_pair("Gone", "")));
    m_status_codes.insert(std::make_pair(411, std::make_pair("Length Required", "")));
    m_status_codes.insert(std::make_pair(412, std::make_pair("Precondition Failed", "")));
    m_status_codes.insert(std::make_pair(413, std::make_pair("Content Too Large", "")));
    m_status_codes.insert(std::make_pair(414, std::make_pair("URI Too Long", "")));
    m_status_codes.insert(std::make_pair(415, std::make_pair("Unsupported Media Type", "")));
    m_status_codes.insert(std::make_pair(416, std::make_pair("Range Not Satisfiable", "")));
    m_status_codes.insert(std::make_pair(417, std::make_pair("Expectation Failed", "")));
    m_status_codes.insert(std::make_pair(418, std::make_pair("I'm a teapot", "")));
    m_status_codes.insert(std::make_pair(421, std::make_pair("Misdirected Request", "")));
    m_status_codes.insert(std::make_pair(422, std::make_pair("Unprocessable Content", "")));
    m_status_codes.insert(std::make_pair(423, std::make_pair("Locked", "")));
    m_status_codes.insert(std::make_pair(424, std::make_pair("Failed Dependency", "")));
    m_status_codes.insert(std::make_pair(425, std::make_pair("Too Early", "")));
    m_status_codes.insert(std::make_pair(426, std::make_pair("Upgrade Required", "")));
    m_status_codes.insert(std::make_pair(428, std::make_pair("Precondition Required", "")));
    m_status_codes.insert(std::make_pair(429, std::make_pair("Too Many Requests", "")));
    m_status_codes.insert(std::make_pair(431, std::make_pair("Request Header Fields Too Large", "")));
    m_status_codes.insert(std::make_pair(451, std::make_pair("Unavailable For Legal Reasons", "")));

    //------ 5xx - Server Error ------
    m_status_codes.insert(std::make_pair(500, std::make_pair("Internal Server Error", "")));
    m_status_codes.insert(std::make_pair(501, std::make_pair("Not Implemented", "")));
    m_status_codes.insert(std::make_pair(502, std::make_pair("Bad Gateway", "")));
    m_status_codes.insert(std::make_pair(503, std::make_pair("Service Unavailable", "")));
    m_status_codes.insert(std::make_pair(504, std::make_pair("Gateway Timeout", "")));
    m_status_codes.insert(std::make_pair(505, std::make_pair("HTTP Version Not Supported", "")));
    m_status_codes.insert(std::make_pair(506, std::make_pair("Variant Also Negotiates", "")));
    m_status_codes.insert(std::make_pair(507, std::make_pair("Insufficient Storage", "")));
    m_status_codes.insert(std::make_pair(508, std::make_pair("Loop Detected", "")));
    m_status_codes.insert(std::make_pair(510, std::make_pair("Not Extended", "")));
    m_status_codes.insert(std::make_pair(511, std::make_pair("Network Authentication Required", "")));
}

void                                    ConfigUtil::_loadDefaultErrorFiles(void)
{
    std::stringstream           file_name_ss;
    std::string                 file_name;
    std::string                 buffer;
    std::string                 file_contents;
    std::ifstream               file_stream;

    status_code_map_t::iterator it = m_status_codes.begin();
    for ( status_code_map_t::value_type code ;  it != m_status_codes.end() ; ++it )
    {

        /* will translate to <location_of_executable>/conf/error_pages/<status_code>.html */
        file_name_ss << "../../" << DFL_ERROR_PAGES_PATH << "/" << code.first << ".html";
        file_name = file_name_ss.str();
        file_name_ss.str( std::string() );

        file_contents = ft::readFileContent(file_stream, file_name);
        if (!file_stream.good() || file_contents.empty())
            continue ;
        m_status_codes[code.first] = std::make_pair(code.second.first, file_contents);
    }
}

void                                    ConfigUtil::setSignalled(int sig)
{
    pthread_mutex_lock( &m_signal_lock );
    m_signal = sig;
    pthread_mutex_unlock( &m_signal_lock );
}

bool                                    ConfigUtil::isSignalled(void)
{
    bool    ret;

    pthread_mutex_lock( &m_signal_lock );
    ret = (m_signal != 0);
    pthread_mutex_unlock( &m_signal_lock );
    return (ret);
}
