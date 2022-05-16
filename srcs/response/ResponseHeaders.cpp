#include <Response.hpp>

status_code_body_t			Response::_buildDate()
{
	std::time_t		rawtime;
	struct std::tm	*ptm;
	char			buf[50];

	time(&rawtime);
	ptm = gmtime(&rawtime);
	strftime(buf, 500, "%a, %d %b %Y %H:%M:%S GMT", ptm);
	return (std::make_pair("Date", std::string(buf)));
}

status_code_body_t			Response::_buildLocation()
{
	std::string	location_url;

	if (m_status_code == HTTP_STATUS_CREATED)
	{
		location_url = m_route.getUploadPath();
	}
	else
	{
		REDIR		*redirect;

		redirect = m_route.getRedir();
		location_url = "http://" + redirect->url;
	}
	return (std::make_pair("Location", location_url));
}

status_code_body_t			Response::_buildRetryAfter()
{
	return (std::make_pair("Retry-After", RETRY_AFTER_SEC));
}

status_code_body_t			Response::_buildAllow()
{
	std::string							allowed_methods;
	std::vector<std::string>			accepted_methods;
	std::vector<std::string>::iterator	iter;

	accepted_methods = m_route.getAcceptedMethods();
	for (iter = accepted_methods.begin(); iter < accepted_methods.end(); iter++)
		allowed_methods.append(*iter + ", ");
	allowed_methods.erase(allowed_methods.length() - 2, allowed_methods.length() - 1);
	return (std::make_pair("Allow", allowed_methods));
}

status_code_body_t			Response::_buildServer()
{
	return (std::make_pair("Server", "Websurf/1.0.0 (Unix)"));
}

status_code_body_t			Response::_buildConnection()
{
    return (std::make_pair("Connection", "close"));
}

status_code_body_t			Response::_buildTransferEncoding()
{
	return (std::make_pair("Transfer-Encoding", "chunked"));
}

status_code_body_t			Response::_buildContentLength()
{
	int			string_size;
	std::string	str_string_size;

	string_size = m_body.size();
	str_string_size = ft::intToString(string_size);
	return (std::make_pair("Content-Length", str_string_size));
}

status_code_body_t			Response::_buildContentType()
{
	std::string	content_type;

	if (m_request.getFilename().find(".jpeg") != std::string::npos || m_request.getFilename().find(".jpg") != std::string::npos)
		content_type = "image/jpeg";
	else if (m_request.getFilename().find(".png") != std::string::npos)
		content_type = "image/png";
	else
		content_type = "text/html; charset=\"utf-8\"";
	return(std::make_pair("Content-Type", content_type));
}

void					Response::buildHeaders()
{
	std::map<std::string, std::string>::iterator	it;

	m_headers_map.insert(_buildDate());
	if (m_status_code == HTTP_STATUS_CREATED ||
		(m_status_code >= HTTP_STATUS_MULTIPLE_CHOICES && m_status_code < HTTP_STATUS_BAD_REQUEST))
		m_headers_map.insert(_buildLocation());
	if (m_status_code == HTTP_STATUS_SERVICE_UNAVAILABLE ||
		(m_status_code >= HTTP_STATUS_MULTIPLE_CHOICES && m_status_code < HTTP_STATUS_BAD_REQUEST))
		m_headers_map.insert(_buildRetryAfter());
	if (m_status_code == HTTP_STATUS_METHOD_NOT_ALLOWED)
		m_headers_map.insert(_buildAllow());
	m_headers_map.insert(_buildServer());
	m_headers_map.insert(_buildConnection());
	m_headers_map.insert(_buildContentLength());
	m_headers_map.insert(_buildContentType());

	for (it = m_headers_map.begin(); it != m_headers_map.end(); ++it)
		m_headers_str += (it->first + ": " + it->second + (CR LF));

	m_headers_str += "\n";
}
