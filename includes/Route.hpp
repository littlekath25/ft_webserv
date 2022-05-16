#pragma once

#define DFL_FILE_SEARCH_PATH   "html/"
#define DFL_INDEX_FILE         "index.html"

#include <Webserv.hpp>

struct s_redirect
{
    std::string url;
    int         status_code;
};

typedef struct s_redirect REDIR;

class Route
{
private:
    std::string                 m_base_url;
    std::vector<std::string>    m_accepted_methods;
    REDIR                       *m_redirect;
    std::string                 m_file_search_path;
    std::string                 m_upload_path;
    bool                        m_has_autoindex;
    std::vector<std::string>    m_index_files;
    std::vector<std::string>    m_cgi_file_extensions;

public:
    Route(void);
    Route(const Route& route);
    ~Route(void);

    Route&  operator = (const Route& route);

    std::string&                getBaseUrl(void);
    std::vector<std::string>&   getAcceptedMethods(void);
    REDIR*                      getRedir(void);
    std::string&                getFileSearchPath(void);
    std::string&                getUploadPath(void);
    bool&                       hasAutoIndex(void);
    std::vector<std::string>&   getIndexFiles(void);
    std::vector<std::string>&   getCgiFileExtensions(void);

    void                        setBaseUrl(const std::string& base_url);
    void                        setAcceptedMethods(const std::vector<std::string>& accepted_methods);
    void                        setFileSearchPath(const std::string& path);
    void                        setUploadPath(const std::string& path);
    void                        setAutoIndex(bool on);
    void                        setRedirect(int status_code, const std::string& url);
    void                        setCgiFileExtensions(const std::vector<std::string>& cgi_file_extensions);

};
