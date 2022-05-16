#pragma once

#include <Webserv.hpp>
#include <Config.hpp>

#include <deque>

#include <dirent.h>
#include <cerrno>

#define DFL_CONFIG_PATH         "conf"
#define DFL_CONFIG_FILE_PATH    DFL_CONFIG_PATH "/default.conf"
#define DFL_SERVER_PORT         8080    /* HTTP default port */

/*
 * Config file parser. Parses the server blocks inside of
 * the config file into ServerConfig objects and groups
 * them inside of PortConfig objects.
 */
class FileParser
{
public:
    typedef enum e_levels
    {
        BASE,
        HTTP,
        SERVER,
        LOCATION
    } t_levels;

    typedef std::deque<std::string>     tokens_t;
    typedef std::vector<PortConfig*>    config_vec_t;

public:
    class InvalidFile       : std::exception { };
    class MappingFailure    : std::exception
    {
    private:
        const std::string   _info;

    public:
        explicit MappingFailure(const std::string& info);
        ~MappingFailure(void) throw();

    public:
        const std::string&  info(void) const;

    };

private:
    /*
     * Current working directory so that based
     * on relative path files can be found.
     */
    std::string         m_cwd;
    std::string         m_file_path;
    std::string         m_config_data;

private:
    FileParser(void);

public:
    explicit FileParser(const char *exec_path);
    FileParser(const FileParser& cpy);
    ~FileParser(void);

    FileParser&         operator = (const FileParser& rhs);

    std::string&        getFilePath(void);
    std::string&        getCwd(void);
    std::string&        getConfigData(void);

    void                setFilePath(const std::string& path);

    /*
     * Will fill the map with new objects parses from
     * the configuration file.
     */
    void                loadFile(void);
    void                mapTokens(config_vec_t& port_configs);

public:
    class Option
    {
    private:
        /* parse_level at which to find option */
        int     m_parse_level;

    public:
        typedef std::map<std::string, Option*>  map_config_t;

    public:
        explicit Option(int parse_level);
        Option(const Option& cpy);
        ~Option();

        Option&         operator = (const Option& rhs);

        int             getParseLevel(void);


        virtual void    parse(void *obj, tokens_t& tokens) = 0;

    };

private:
    void                _mapTokens(tokens_t& tokens, std::stack<ServerConfig*>& server_blocks);
    static void         _groupServerConfigs(config_vec_t& port_configs, std::stack<ServerConfig*>& server_blocks);

    static tokens_t&    _tokenizeString(tokens_t& queue, std::string& str, const std::string& delims);
    static std::string& _stringReplaceMultiple(std::string& str, const std::string& from, const std::string& to);

public:
    /*
     *          ( Option )
     * All subclass based on abstract Option
     */
    class OptionHttp : public Option
    {
    public:
        OptionHttp(int parse_level);
        OptionHttp(const OptionHttp& cpy);
        ~OptionHttp();

        OptionHttp&     operator = (const OptionHttp& rhs);

        void            parse(void *obj, tokens_t& tokens);

    };

    class OptionClientMaxBodySize : public Option
    {
    public:
        OptionClientMaxBodySize(int parse_level);
        OptionClientMaxBodySize(const OptionClientMaxBodySize& cpy);
        ~OptionClientMaxBodySize();

        OptionClientMaxBodySize&     operator = (const OptionClientMaxBodySize& rhs);

        void            parse(void *obj, tokens_t& tokens);

    };

    class OptionErrorPage : public Option
    {
    public:
        OptionErrorPage(int parse_level);
        OptionErrorPage(const OptionErrorPage& cpy);
        ~OptionErrorPage();

        OptionErrorPage&     operator = (const OptionErrorPage& rhs);

        void            parse(void *obj, tokens_t& tokens);

    };

    class OptionServer : public Option
    {
    public:
        OptionServer(int parse_level);
        OptionServer(const OptionServer& cpy);
        ~OptionServer();

        OptionServer&     operator = (const OptionServer& rhs);

        void            parse(void *obj, tokens_t& tokens);

    };

    class OptionListen : public Option
    {
    public:
        OptionListen(int parse_level);
        OptionListen(const OptionListen& cpy);
        ~OptionListen();

        OptionListen&   operator = (const OptionListen& rhs);

        void            parse(void *obj, tokens_t& tokens);

    private:
        void            _parseArg(const std::string &arg, std::string& address, uint16_t *sin_port);

    };

    class OptionServerName : public Option
    {
    public:
        OptionServerName(int parse_level);
        OptionServerName(const OptionServerName& cpy);
        ~OptionServerName();

        OptionServerName&     operator = (const OptionServerName& rhs);

        void            parse(void *obj, tokens_t& tokens);

    };

    class OptionLocation : public Option
    {
    public:
        OptionLocation(int parse_level);
        OptionLocation(const OptionLocation& cpy);
        ~OptionLocation();

        OptionLocation&     operator = (const OptionLocation& rhs);

        void            parse(void *obj, tokens_t& tokens);

    };

    class OptionAllowedMethods : public Option
    {
    public:
        OptionAllowedMethods(int parse_level);
        OptionAllowedMethods(const OptionAllowedMethods& cpy);
        ~OptionAllowedMethods();

        OptionAllowedMethods&     operator = (const OptionAllowedMethods& rhs);

        void            parse(void *obj, tokens_t& tokens);

    };

    class OptionRoot : public Option
    {
    public:
        OptionRoot(int parse_level);
        OptionRoot(const OptionRoot& cpy);
        ~OptionRoot();

        OptionRoot&     operator = (const OptionRoot& rhs);

        void            parse(void *obj, tokens_t& tokens);

    };

    class OptionAutoIndex : public Option
    {
    public:
        OptionAutoIndex(int parse_level);
        OptionAutoIndex(const OptionAutoIndex& cpy);
        ~OptionAutoIndex();

        OptionAutoIndex&     operator = (const OptionAutoIndex& rhs);

        void            parse(void *obj, tokens_t& tokens);

    };

    class OptionIndex : public Option
    {
    public:
        OptionIndex(int parse_level);
        OptionIndex(const OptionIndex& cpy);
        ~OptionIndex();

        OptionIndex&     operator = (const OptionIndex& rhs);

        void            parse(void *obj, tokens_t& tokens);

    };

    class OptionCgiExtension : public Option
    {
    public:
        OptionCgiExtension(int parse_level);
        OptionCgiExtension(const OptionCgiExtension& cpy);
        ~OptionCgiExtension();

        OptionCgiExtension&     operator = (const OptionCgiExtension& rhs);

        void            parse(void *obj, tokens_t& tokens);

    };

    class OptionUploadPath : public Option
    {
    public:
        OptionUploadPath(int parse_level);
        OptionUploadPath(const OptionUploadPath& cpy);
        ~OptionUploadPath();

        OptionUploadPath&     operator = (const OptionUploadPath& rhs);

        void            parse(void *obj, tokens_t& tokens);

    };

    class OptionReturn : public Option
    {
    public:
        OptionReturn(int parse_level);
        OptionReturn(const OptionReturn& cpy);
        ~OptionReturn();

        OptionReturn&     operator = (const OptionReturn& rhs);

        void            parse(void *obj, tokens_t& tokens);

    };

    static Option                   *m_option_http;
    static Option                   *m_option_client_max_body_size;
    static Option                   *m_option_error_page;
    static Option                   *m_option_server;
    static Option                   *m_option_listen;
    static Option                   *m_option_server_name;
    static Option                   *m_option_location;
    static Option                   *m_option_allowed_methods;
    static Option                   *m_option_root;
    static Option                   *m_option_autoindex;
    static Option                   *m_option_index;
    static Option                   *m_option_cgi_extension;
    static Option                   *m_option_upload_path;
    static Option                   *m_option_return;

    static Option::map_config_t&    _getConfigMap(void);

};
