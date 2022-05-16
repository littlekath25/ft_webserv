#pragma once

#define DFL_CONFIG_PATH         "conf"
#define DFL_ERROR_PAGES_PATH    DFL_CONFIG_PATH "/error_pages/"

#include <Webserv.hpp>
#include <Utils.hpp>

class ConfigUtil
{
public:
    typedef std::vector<std::string>            methods_t;
    typedef std::pair<std::string, std::string> status_code_body_t;
    typedef std::map<int, status_code_body_t>   status_code_map_t;

private:
    status_code_map_t           m_status_codes;
    methods_t                   m_methods;

    int                         m_signal;
    pthread_mutex_t             m_signal_lock;

    static ConfigUtil           *m_handle;

public:
    ~ConfigUtil();

private:
    ConfigUtil();
    ConfigUtil(const ConfigUtil& cpy);

public:
    static ConfigUtil&          getHandle(void);

    const methods_t&            getDefaultMethods(void);
    status_code_map_t&          getStatusCodeMap(void);

    void                        setSignalled(int sig);
    bool                        isSignalled(void);

private:
    void                        _setDefaultStatusCodes(void);
    void                        _setDefaultMethods(void);
    void                        _loadDefaultErrorFiles(void);

};
