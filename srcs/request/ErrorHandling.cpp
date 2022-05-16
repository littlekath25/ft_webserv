#include "Request.hpp"

void    Request::errorChecking()
{
    std::string query;

    if (getStatus() == HTTP_STATUS_OK)
        checkStatusLine();
    if (getStatus() == HTTP_STATUS_OK)
        checkHeaders();
    if (getStatus() == HTTP_STATUS_OK)
        checkFileType();
}

void    Request::checkStatusLine()
{
    if (m_method.empty() || m_version.empty())
    {
        setStatus(HTTP_STATUS_BAD_REQUEST);
    }
    else if (m_method != "GET" && m_method != "POST" && m_method != "DELETE")
    {
        setStatus(HTTP_STATUS_METHOD_NOT_ALLOWED);
    }
    else if (m_version != HTTP_VERSION)
    {
        setStatus(HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED);
    }
}

void    Request::checkHeaders()
{
    if (m_headers.find("Host") == m_headers.end())
    {
        setStatus(HTTP_STATUS_BAD_REQUEST);
    }
    else if (m_headers.find("Expect") != m_headers.end() && m_headers["Expect"] != "100-continue")
    {
        setStatus(HTTP_STATUS_EXPECTATION_FAILED);
    }
    if (m_headers.find("Content-Type") != m_headers.end())
    {
        if (m_headers["Content-Type"].find("multipart/form") != std::string::npos)
            setStatus(HTTP_STATUS_TEAPOT);
    }
}

void    Request::checkFileType()
{
    if (m_headers.find("Content-Type") != m_headers.end())
    {
        std::string type;
        type = m_headers.find("Content-Type")->second.substr(0, type.find('/'));
        if (type == "video" || type == "multipart")
        {
            setStatus(HTTP_STATUS_TEAPOT);
        }
    }
}
