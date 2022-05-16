#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <Webserv.hpp>
# include <Request.hpp>
# include <ConfigUtil.hpp>
# include <Utils.hpp>
# include <Route.hpp>
# include <StatusCodes.hpp>

# include <time.h>
# include <ctime>
# include <cstdlib>

# define RETRY_AFTER_SEC	"120"
# define DFL_PATH			"html/"
# define FILE_EXTENSION		".html"
# define POST_FILENAME		"post.html"
# define DELETE_FILENAME	"delete.html"

typedef std::pair<std::string, std::string> status_code_body_t;
typedef std::map<int, status_code_body_t>   status_code_map_t;

class Response
{
	public:
		class ResponseFailure : std::exception { };
	private:
		Request 								m_request;
		Route									m_route;
		int										m_status_code;
		std::string								m_start_line;
		std::map<std::string, std::string>		m_headers_map;
		std::string								m_headers_str;
		std::string								m_body;
		std::string 							m_response;

	public:
		Response();
		Response(Request &re);
		Response(Request &re, Route &ro);
		Response(const Response &copy);
		~Response();

		Response& operator = (const Response &copy);

		Request const &											getRequest() const;
		Route const &											getRoute() const;
		int const &												getStatusCode() const;					
		std::string const &										getStartLine() const;
		std::map<std::string, std::string> const &				getHeadersMap() const;
		std::string	const &										getHeadersStr() const;
		std::string	const &										getBody() const;
		std::string const &										getResponse() const;

		void			setRequest(Request& req);
		void			setRoute(Route& rou);

		void			buildStartLine(ConfigUtil::status_code_map_t& m_error_files);
		void			buildHeaders();
		void			buildBody(ConfigUtil::status_code_map_t& m_error_files);
		void		 	buildAutoIndex();
		void			buildResponse(ConfigUtil::status_code_map_t& m_error_files);
		void			resetResponse();

	private:
		status_code_body_t	_buildDate();
		status_code_body_t	_buildLocation();
		status_code_body_t	_buildRetryAfter();
		status_code_body_t	_buildAllow();
		status_code_body_t	_buildServer();
		status_code_body_t	_buildConnection();
		status_code_body_t	_buildContentLength();
		status_code_body_t	_buildContentType();
		status_code_body_t	_buildTransferEncoding();
		void				_buildBodyGet();
		void				_buildBodyPost();
		void				_buildBodyDelete();
		void				_buildBodyError(ConfigUtil::status_code_map_t& m_error_files);
		std::string			_buildFilePath(const std::string &filename, int file_flag);
		int					_readFileIntoString(const std::string &path);
		void 				_sortAndAddToBody(std::vector<std::string> directories, std::vector<std::string> files);
};

#endif
