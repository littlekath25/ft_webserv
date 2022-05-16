#include <Response.hpp>

std::string	Response::_buildFilePath(const std::string &filename, int file_flag)
{
	std::string path;

	if (file_flag < HTTP_STATUS_BAD_REQUEST)
	{
		if (m_request.getMethod() == "GET")
			path = m_request.m_filesearch + filename;
		else if (m_request.getMethod() == "POST")
			path = DFL_PATH + filename;
		else if (m_request.getMethod() == "DELETE")
			path = DFL_PATH + filename;
	}
	else
		path = DFL_ERROR_PAGES_PATH + filename + FILE_EXTENSION;

	return (path);
}

int			Response::_readFileIntoString(const std::string &path)
{
	std::ifstream input_file(path.c_str());
	if (!input_file.is_open())
		return (0);

	m_body = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
	return (1);
}

void		Response::_buildBodyGet()
{
	std::string path;
	std::vector<std::string>			path_vector;
	std::vector<std::string>::iterator	iter;

	if (m_request.getFilename().empty())
	{
		if (m_request.isAutoIndex())
		{
			buildAutoIndex();
		}
		else
		{
			path_vector = m_route.getIndexFiles();
			for (iter = path_vector.begin(); iter != path_vector.end(); ++iter)
			{
				path = _buildFilePath(path_vector.at(iter - path_vector.begin()), m_status_code);
				if (_readFileIntoString(path))
					break;	
			}
		}
	}
	else
	{
		path = _buildFilePath(m_request.getFilename(), m_status_code);
		_readFileIntoString(path);	
	}
}

void			Response::_buildBodyPost(void)
{
	std::string path;

	path = _buildFilePath(POST_FILENAME, m_status_code);
	_readFileIntoString(path);
}

void			Response::_buildBodyDelete(void)
{
	std::string path;

	path = _buildFilePath(DELETE_FILENAME, m_status_code);
	_readFileIntoString(path);
}

void			Response::_buildBodyError(ConfigUtil::status_code_map_t& m_error_files)
{
	status_code_map_t::iterator iter;

	iter = m_error_files.find(m_status_code);
	if (iter->second.second.empty())
	{
		std::string path;

		path = _buildFilePath(ft::intToString(m_status_code), m_status_code);
		_readFileIntoString(path);
	}
	else
		m_body = iter->second.second;
}

void			Response::buildBody(ConfigUtil::status_code_map_t& m_error_files)
{
	if (m_status_code < HTTP_STATUS_BAD_REQUEST)
	{
		if (m_request.getMethod() == "GET")
			_buildBodyGet();
		else if (m_request.getMethod() == "POST")
			_buildBodyPost();
		else if (m_request.getMethod() == "DELETE")
			_buildBodyDelete();
	}
	else
		_buildBodyError(m_error_files);
}
