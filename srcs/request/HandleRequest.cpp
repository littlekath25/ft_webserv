#include <Request.hpp>
#include <Socket.hpp>

void Request::parseFilenamesAndCGI(void)
{
	if (std::count(m_target.begin(), m_target.end(), '.') > 1)
	{
		setStatus(HTTP_STATUS_BAD_REQUEST);
	}
    else if (std::count(m_target.begin(), m_target.end(), '.') == 1)
    {
		ssize_t idx;
		std::string tmp;

		idx = m_target.find_first_of('.');
		tmp = m_target.substr(idx);
		if (tmp.find('/') != std::string::npos)
			idx += tmp.find('/');
		else
			idx += tmp.length();
		tmp = m_target.substr(0, idx);
		m_filename = tmp;
		m_target.erase(0, idx);
		if (!m_target.empty())
			m_cgi_path = m_target;
		m_target = m_filename.substr(0, m_filename.find_last_of('/') + 1);
		m_filename.erase(0, m_filename.find_last_of('/') + 1);
    }
}

void Request::parseQuery(std::string url)
{
	if (!url.empty())
	{
		if (url.find('?') != std::string::npos)
		{
			m_query = url.substr(url.find('?') + 1);
			url.erase(url.find('?'));
		}
	}
	if (!url.empty())
	{
		m_target = url;
	}
}

void Request::parseAndSetStartLine(void)
{
	m_method = m_request.substr(0, m_request.find(' '));
	m_request.erase(0, m_request.find(' ') + 1);
	m_request.erase(0, m_request.find_first_not_of(' '));
	std::string url = m_request.substr(0, m_request.find(' '));
	if (url.at(url.length() - 1) == '/')
		url.erase(url.length() - 1);
	m_request.erase(0, m_request.find(' ') + 1);
	m_request.erase(0, m_request.find_first_not_of(' '));
	m_version = m_request.substr(0, m_request.find(CR));
	m_request.erase(0, m_request.find(LF) + 1);

	parseQuery(url);
	parseFilenamesAndCGI();
}

void Request::setHost(void)
{
	if (m_headers.find("Host") == m_headers.end())
	{
		setStatus(HTTP_STATUS_BAD_REQUEST);
	}
	else 
	{
		std::string	host = m_headers["Host"];
		if (host.find(':') != std::string::npos)
		{
			host = host.substr(host.find(':') + 1, host.length() - 1);
			std::istringstream(host) >> m_port;
		}
	}
}

void Request::parseAndSetHeaders(void)
{
	std::string token;
	std::string key;
	std::string value;

	while (m_request[0] != '\r' && !m_request.empty())
	{
		token = m_request.substr(0, m_request.find(CR));
		key = m_request.substr(0, m_request.find(':'));
		m_request.erase(0, m_request.find(':') + 2);
		value = m_request.substr(0, m_request.find(CR));
		m_headers.insert(std::pair<std::string, std::string>(key, value));
		m_request.erase(0, m_request.find(LF) + 1);
	}
	setHost();
	if (!m_request.empty())
    {
        m_request.erase(0, 2);
        m_body = m_request;
    }
}

void Request::checkIfChunkedRequest(void)
{
	headers_t::iterator it;

	it = getHeaders().find("Content-Encoding");
    if (it != getHeaders().end() && it->second == "chunked")
    {
        decodeRequest();
    }
}

void Request::printRequest(void)
{
	std::cout << "------------------ START LINE ------------------" << std::endl;
	std::cout << "Status: " << m_status << std::endl;
	std::cout << "Method: " << m_method << std::endl;
	std::cout << "Target: " << m_target << std::endl;
	std::cout << "Filename: " << m_filename << std::endl;
	std::cout << "CGI: " << m_cgi_path << std::endl;
	std::cout << "Query: " << m_query << std::endl;
	std::cout << "Version: " << m_version << std::endl;
	std::cout << "Port: " << m_port << std::endl;

	std::cout << "------------------ HEADERS ------------------" << std::endl;
	for (std::map<std::string, std::string>::iterator it = m_headers.begin(); it != m_headers.end(); ++it)
		std::cout << "{" << it->first << "} {" << it->second << "}" << std::endl;

	std::cout << "------------------ BODY ------------------" << std::endl;
	std::cout << m_body << std::endl;
	std::cout << "------------------ END ------------------" << std::endl;
}
