#pragma once

#include <Webserv.hpp>
#include <Request.hpp>
#include <Response.hpp>
#include <CGI.hpp>
#include <ClientSocket.hpp>
#include <Handler.hpp>

#include <memory>

/*
 * Holds connection information while poll
 * returns POLLIN or a CGI process is still
 * running in the background.
 */
class Connection
{
public:
    typedef std::vector<Route*>             routes_t;

private:
    ClientSocket            *m_sock;
    Request                 m_request;
    Route                   *m_route;
    std::unique_ptr<CGI>    m_cgi;
    ConfigUtil::status_code_map_t   *m_error_files;

public:
    bool            m_cgi_added;

    Connection(void);

private:
    Connection(const Connection& cpy);

    Connection&     operator = (const Connection& conn);

public:
    explicit Connection(ClientSocket *sock);

    ~Connection(void);

    void            setSocket(ClientSocket *sock);
    void		    setRoute(Route *route);
    void            setErrorFiles(ConfigUtil::status_code_map_t *error_files);

    Request&        getRequest(void);
    CGI             *getCGI(void);
    ClientSocket    *getSock(void);
    ConfigUtil::status_code_map_t   *getErrorFiles(void);

    void            readSocket(void);
    void            parseRequest(void);
    void            sendResponse(ConfigUtil::status_code_map_t *error_files);

    void            methodHandler(void);

    void            close(void);

    void		    checkRoute(void);
    void		    checkAcceptedMethods(void);
    void		    checkRedirects(void);
    void		    checkFileSearchPath(void);
    void            checkBodySize(uint32_t max_size);

    void		    searchFile(void);
    void 		    searchDefaultIndexPages(void);
    bool		    searchCGIExtensions(void);

    void            initCGI(void);

public:
    class NoSocketFail : public std::exception { };

};
