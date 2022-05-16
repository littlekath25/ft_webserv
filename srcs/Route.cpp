#include <Route.hpp>
#include <ConfigUtil.hpp>

Route::Route() :
    m_redirect(NULL),
    m_upload_path(DFL_UPLOAD_PATH),
    m_has_autoindex(false)
{
    const ConfigUtil::methods_t *methods;

    methods = &ConfigUtil::getHandle().getDefaultMethods();
    for (size_t idx = 0; idx < methods->size(); idx++)
        m_accepted_methods.push_back(methods->at(idx));
    m_file_search_path = DFL_FILE_SEARCH_PATH;
    m_index_files.push_back(DFL_INDEX_FILE);
}

Route::Route(const Route& route)
{
    *this = route;
}

Route::~Route()
{
    delete m_redirect;
}

Route&      Route::operator = (const Route &route)
{
    if (this != &route)
    {
        m_base_url = route.m_base_url;
        m_accepted_methods = route.m_accepted_methods;
        m_redirect = NULL;
        m_file_search_path = route.m_file_search_path;
        m_upload_path = route.m_upload_path;
        m_has_autoindex = route.m_has_autoindex;
        m_index_files = route.m_index_files;
        m_cgi_file_extensions = route.m_cgi_file_extensions;
        if (route.m_redirect)
        {
            m_redirect = new REDIR;
            m_redirect->status_code = route.m_redirect->status_code;
            m_redirect->url = route.m_redirect->url;
        }
    }
    return (*this);
}

std::string&                Route::getBaseUrl(void)
{
    return (m_base_url);
}

std::vector<std::string>&   Route::getAcceptedMethods(void)
{
    return (m_accepted_methods);
}

REDIR*                      Route::getRedir(void)
{
    return (m_redirect);
}

std::string&                Route::getFileSearchPath(void)
{
    return (m_file_search_path);
}

std::string&                Route::getUploadPath(void)
{
    return (m_upload_path);
}

bool&                       Route::hasAutoIndex(void)
{
    return (m_has_autoindex);
}

std::vector<std::string>&   Route::getIndexFiles(void)
{
    return (m_index_files);
}

std::vector<std::string>&   Route::getCgiFileExtensions(void)
{
    return (m_cgi_file_extensions);
}

void                        Route::setBaseUrl(const std::string &base_url)
{
    m_base_url = base_url;
}

void                        Route::setAcceptedMethods(const std::vector<std::string> &accepted_methods)
{
    m_accepted_methods = accepted_methods;
}

void                        Route::setFileSearchPath(const std::string &path)
{
    m_file_search_path = path;
}

void                        Route::setUploadPath(const std::string &path)
{
    m_upload_path = path;
}

void                        Route::setAutoIndex(bool on)
{
    m_has_autoindex = on;
}

void                        Route::setRedirect(int status_code, const std::string &url)
{
    if (!m_redirect) m_redirect = new REDIR;
    m_redirect->status_code = status_code;
    m_redirect->url = url;
}

void                        Route::setCgiFileExtensions(const std::vector<std::string> &cgi_file_extensions)
{
    m_cgi_file_extensions = cgi_file_extensions;
}
