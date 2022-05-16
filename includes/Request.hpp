#pragma once

#include <Webserv.hpp>
#include <StatusCodes.hpp>

#define DFL_TARGET      "/"

class Request
{
public:
    typedef std::map<std::string, std::string>  headers_t;

private:
    std::string		m_target;
    std::string		m_filename;
    std::string		m_query;
    headers_t 		m_headers;
    std::string		m_body;
    std::string		m_cgi_path;

    int				m_status;
    std::string 	m_method;
    std::string		m_version;
    int				m_port;

    bool		    m_cgi;
    bool            m_autoindex;

public:
    std::string     m_request;
    std::string     m_filesearch;

public:
    Request();
    Request(const Request &Copy);
    ~Request();

    Request& operator = (const Request &Copy);

    /*
     * Call this function when you're done appending raw
     * data to the Request instance using .appendRequestData.
     *
     * It will initialise all the other fields in the request object.
     */
    void            parse(void);
    void            appendRequestData(const char *data, ssize_t bytes_read);

    void 			parseAndSetStartLine();
    void 			parseAndSetHeaders();
    void			parseQuery(std::string url);
    void			parseFilenamesAndCGI();

    void			setStatus(int status);
    void			setCGI(bool status);
    void			setHost(void);
    void            setFilesearchPath(std::string path);
    void            setAutoIndex(bool status);
    void            checkIfChunkedRequest();

    int				&getStatus();
    int				&getPort();
    std::string		&getTarget();
    std::string		&getQuery();
    std::string		&getMethod();
    std::string		&getVersion();
    std::string		&getFilename();
    std::string		&getBody();
    std::string     &getCGIPath();
    headers_t 		&getHeaders();
    bool			&isCGI();
    bool            &isAutoIndex();

    void			decodeRequest();

    void			errorChecking();
    void			checkStatusLine();
    void			checkHeaders();
    void            checkFileType();

    void			printRequest();
};
