#include <Response.hpp>

void					Response::buildStartLine(ConfigUtil::status_code_map_t& m_error_files)
{
	std::string									str_status_code;
	std::string 								reason_phrase;
	std::string									white_space;
	ConfigUtil::status_code_map_t::iterator		it;

	m_status_code = m_request.getStatus();
	str_status_code = ft::intToString(m_status_code);

	it = m_error_files.find(m_status_code);
	if (it != m_error_files.end())
		reason_phrase = it->second.first;
	else
		throw ResponseFailure();

	white_space = " ";
	m_start_line = HTTP_VERSION
				+ white_space
				+ str_status_code
				+ white_space
				+ reason_phrase
				+ (CR LF);
}
