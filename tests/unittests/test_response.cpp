# include <unittests.hpp>
# include <Response.hpp>
# include <Request.hpp>
# include <Route.hpp>
# include <Config.hpp>

# include <vector>
# include <string>

// Request setup

// Basic GET Request
static std::string basic_get_request_string =
    	"GET / HTTP/1.1\r\n"
    	"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.51 Safari/537.36\r\n"
    	"Host: localhost:8082\r\n"
    	"Accept-Language: en-us\r\n"
    	"Accept-Encoding: gzip, deflate\r\n\r\n";

// Basic POST Request
static std::string  basic_post_request_string =
        "POST /index.html HTTP/1.1\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.51 Safari/537.36\r\n"
        "Host: localhost:8082\r\n"
        "Content-Type: text/xml; charset=utf-8\r\n"
        "Content-Length: 35\r\n"
        "Accept-Language: en-us\r\n"
        "Accept-Encoding: gzip, deflate\r\n\r\n"
        "<html><h1>Goodbye World</h1></html>";

// Basic DELETE Request
static std::string  basic_delete_request_string =
        "DELETE /index.html HTTP/1.1\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.51 Safari/537.36\r\n"
        "Host: localhost\r\n\r\n";

// Basic invalid GET Request
static std::string  basic_get_request_wrong_string =
        "GET / HTTP/1.0\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.51 Safari/537.36\r\n"
        "Host: localhost:8080\r\n"
        "Accept-Language: en-us\r\n"
        "Accept-Encoding: gzip, deflate\r\n\r\n";

// Route setup

// Default Route 

static std::string default_m_base_url = "/";
static std::string default_m_file_search_path = "/html";
static std::string default_m_upload_path = "/html";
static bool default_m_has_autoindex = true;

std::vector<std::string> default_m_accepted_methods{ "GET", "POST", "DELETE" };

std::vector<std::string> default_m_index_files{ "index.html", "index.php" };

std::vector<std::string> default_m_cgi_file_extensions{ ".php", ".py" };

class TestResponse : public ::testing::Test
{
    public:
        Response            res;
        Request             req;
        Route               rou;
    protected:
        void setRequestData(const std::string& data)
        {
            req.m_request = data;
            req.setFilesearchPath("html/");
            req.parse();
            req.errorChecking();
        }

        void setRouteData(const std::string& base_url, const std::string& file_search_path, const std::string& upload_path, 
            const bool& has_autoindex, const std::vector<std::string>& accepted_methods, const std::vector<std::string>& cgi_file_extensions)
        {
            rou.setBaseUrl(base_url);
            rou.setFileSearchPath(file_search_path);
            rou.setUploadPath(upload_path);
            rou.setAutoIndex(has_autoindex);
            rou.setAcceptedMethods(accepted_methods);
            rou.setCgiFileExtensions(cgi_file_extensions);
        }

        void setResponseData()
        {
            res.setRequest(req);
            res.setRoute(rou);
        }

        status_code_map_t setDefaultStatusCodes(void)
        {
            status_code_map_t m_status_codes;

            //------ 1xx - Informational ------
            m_status_codes.insert(std::make_pair(100, std::make_pair("Continue", "")));
            m_status_codes.insert(std::make_pair(101, std::make_pair("Switching Protocols", "")));
            m_status_codes.insert(std::make_pair(102, std::make_pair("Processing", "")));
            m_status_codes.insert(std::make_pair(103, std::make_pair("Early Hints", "")));

            //------ 2xx - Successful ------
            m_status_codes.insert(std::make_pair(200, std::make_pair("OK", "")));
            m_status_codes.insert(std::make_pair(201, std::make_pair("Created", "")));
            m_status_codes.insert(std::make_pair(202, std::make_pair("Accepted", "")));
            m_status_codes.insert(std::make_pair(203, std::make_pair("Non-Authoritative Information", "")));
            m_status_codes.insert(std::make_pair(204, std::make_pair("No Content", "")));
            m_status_codes.insert(std::make_pair(205, std::make_pair("Reset Content", "")));
            m_status_codes.insert(std::make_pair(206, std::make_pair("Partial Content", "")));
            m_status_codes.insert(std::make_pair(207, std::make_pair("Multi-Status", "")));
            m_status_codes.insert(std::make_pair(208, std::make_pair("Already Reported", "")));
            m_status_codes.insert(std::make_pair(226, std::make_pair("Im Used", "")));

            //------ 3xx - Redirection ------
            m_status_codes.insert(std::make_pair(300, std::make_pair("Multiple Choices", "")));
            m_status_codes.insert(std::make_pair(301, std::make_pair("Moved Permanently", "")));
            m_status_codes.insert(std::make_pair(302, std::make_pair("Found", "")));
            m_status_codes.insert(std::make_pair(303, std::make_pair("See Other", "")));
            m_status_codes.insert(std::make_pair(304, std::make_pair("Not Modified", "")));
            m_status_codes.insert(std::make_pair(305, std::make_pair("Use Proxy", "")));
            m_status_codes.insert(std::make_pair(307, std::make_pair("Temporary Redirect", "")));
            m_status_codes.insert(std::make_pair(308, std::make_pair("Permanent Redirect", "")));

            //------ 4xx - Client Error ------
            m_status_codes.insert(std::make_pair(400, std::make_pair("Bad Request", "")));
            m_status_codes.insert(std::make_pair(401, std::make_pair("Unauthorized", "")));
            m_status_codes.insert(std::make_pair(402, std::make_pair("Payment Required", "")));
            m_status_codes.insert(std::make_pair(403, std::make_pair("Forbidden", "")));
            m_status_codes.insert(std::make_pair(404, std::make_pair("Not Found", "")));
            m_status_codes.insert(std::make_pair(405, std::make_pair("Method Not Allowed", "")));
            m_status_codes.insert(std::make_pair(406, std::make_pair("Not Acceptable", "")));
            m_status_codes.insert(std::make_pair(407, std::make_pair("Proxy Authentication Required", "")));
            m_status_codes.insert(std::make_pair(408, std::make_pair("Request Timeout", "")));
            m_status_codes.insert(std::make_pair(409, std::make_pair("Conflict", "")));
            m_status_codes.insert(std::make_pair(410, std::make_pair("Gone", "")));
            m_status_codes.insert(std::make_pair(411, std::make_pair("Length Required", "")));
            m_status_codes.insert(std::make_pair(412, std::make_pair("Precondition Failed", "")));
            m_status_codes.insert(std::make_pair(413, std::make_pair("Content Too Large", "")));
            m_status_codes.insert(std::make_pair(414, std::make_pair("URI Too Long", "")));
            m_status_codes.insert(std::make_pair(415, std::make_pair("Unsupported Media Type", "")));
            m_status_codes.insert(std::make_pair(416, std::make_pair("Range Not Satisfiable", "")));
            m_status_codes.insert(std::make_pair(417, std::make_pair("Expectation Failed", "")));
            m_status_codes.insert(std::make_pair(418, std::make_pair("I'm a teapot", "")));
            m_status_codes.insert(std::make_pair(421, std::make_pair("Misdirected Request", "")));
            m_status_codes.insert(std::make_pair(422, std::make_pair("Unprocessable Content", "")));
            m_status_codes.insert(std::make_pair(423, std::make_pair("Locked", "")));
            m_status_codes.insert(std::make_pair(424, std::make_pair("Failed Dependency", "")));
            m_status_codes.insert(std::make_pair(425, std::make_pair("Too Early", "")));
            m_status_codes.insert(std::make_pair(426, std::make_pair("Upgrade Required", "")));
            m_status_codes.insert(std::make_pair(428, std::make_pair("Precondition Required", "")));
            m_status_codes.insert(std::make_pair(429, std::make_pair("Too Many Requests", "")));
            m_status_codes.insert(std::make_pair(431, std::make_pair("Request Header Fields Too Large", "")));
            m_status_codes.insert(std::make_pair(451, std::make_pair("Unavailable For Legal Reasons", "")));

            //------ 5xx - Server Error ------
            m_status_codes.insert(std::make_pair(500, std::make_pair("Internal Server Error", "")));
            m_status_codes.insert(std::make_pair(501, std::make_pair("Not Implemented", "")));
            m_status_codes.insert(std::make_pair(502, std::make_pair("Bad Gateway", "")));
            m_status_codes.insert(std::make_pair(503, std::make_pair("Service Unavailable", "")));
            m_status_codes.insert(std::make_pair(504, std::make_pair("Gateway Timeout", "")));
            m_status_codes.insert(std::make_pair(505, std::make_pair("HTTP Version Not Supported", "")));
            m_status_codes.insert(std::make_pair(506, std::make_pair("Variant Also Negotiates", "")));
            m_status_codes.insert(std::make_pair(507, std::make_pair("Insufficient Storage", "")));
            m_status_codes.insert(std::make_pair(508, std::make_pair("Loop Detected", "")));
            m_status_codes.insert(std::make_pair(510, std::make_pair("Not Extended", "")));
            m_status_codes.insert(std::make_pair(511, std::make_pair("Network Authentication Required", "")));

            return(m_status_codes);
        }
};

std::string readFileIntoString(const std::string &path)
{
    std::string m_body;

	std::ifstream input_file(path.c_str());
	if (!input_file.is_open())
		return (m_body);

	m_body = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    return (m_body);
}


TEST_F(TestResponse, GetRequestBasicConfStartLine)
{
    setRequestData(basic_get_request_string);
    setRouteData(default_m_base_url, default_m_file_search_path, default_m_upload_path, default_m_has_autoindex,
        default_m_accepted_methods, default_m_cgi_file_extensions);
    setResponseData();

    status_code_map_t   m_status_codes = setDefaultStatusCodes();

    res.buildResponse(m_status_codes);

    EXPECT_TRUE(res.getStartLine() == "HTTP/1.1 200 OK\r\n");
}

TEST_F(TestResponse, GetRequestBasicConfBody)
{
    setRequestData(basic_get_request_string);
    setRouteData(default_m_base_url, default_m_file_search_path, default_m_upload_path, default_m_has_autoindex,
        default_m_accepted_methods, default_m_cgi_file_extensions);
    setResponseData();

    status_code_map_t   m_status_codes = setDefaultStatusCodes();

    res.buildResponse(m_status_codes);

    std::string correct_response_body = readFileIntoString("html/index.html");

    EXPECT_TRUE(res.getBody() == correct_response_body);
}

TEST_F(TestResponse, PostRequestBasicConfStartLine)
{
    setRequestData(basic_post_request_string);
    setRouteData(default_m_base_url, default_m_file_search_path, default_m_upload_path, default_m_has_autoindex,
        default_m_accepted_methods, default_m_cgi_file_extensions);
    setResponseData();

    status_code_map_t   m_status_codes = setDefaultStatusCodes();

    res.buildResponse(m_status_codes);

    EXPECT_TRUE(res.getStartLine() == "HTTP/1.1 200 OK\r\n");
}

TEST_F(TestResponse, PostRequestBasicConfBody)
{
    setRequestData(basic_post_request_string);
    setRouteData(default_m_base_url, default_m_file_search_path, default_m_upload_path, default_m_has_autoindex,
        default_m_accepted_methods, default_m_cgi_file_extensions);
    setResponseData();

    status_code_map_t   m_status_codes = setDefaultStatusCodes();

    res.buildResponse(m_status_codes);

    std::string correct_response_body = readFileIntoString("html/post.html");

    EXPECT_TRUE(res.getBody() == correct_response_body);
}

TEST_F(TestResponse, DeleteRequestBasicConfStartLine)
{
    setRequestData(basic_delete_request_string);
    setRouteData(default_m_base_url, default_m_file_search_path, default_m_upload_path, default_m_has_autoindex,
        default_m_accepted_methods, default_m_cgi_file_extensions);
    setResponseData();

    status_code_map_t   m_status_codes = setDefaultStatusCodes();

    res.buildResponse(m_status_codes);

    EXPECT_TRUE(res.getStartLine() == "HTTP/1.1 200 OK\r\n");
}

TEST_F(TestResponse, DeleteRequestBasicConfBody)
{
    setRequestData(basic_delete_request_string);
    setRouteData(default_m_base_url, default_m_file_search_path, default_m_upload_path, default_m_has_autoindex,
        default_m_accepted_methods, default_m_cgi_file_extensions);
    setResponseData();

    status_code_map_t   m_status_codes = setDefaultStatusCodes();

    res.buildResponse(m_status_codes);

    std::string correct_response_body = readFileIntoString("html/delete.html");

    EXPECT_TRUE(res.getBody() == correct_response_body);
}

TEST_F(TestResponse, InvalidGetRequestBasicConfStartLine)
{
    setRequestData(basic_get_request_wrong_string);
    setRouteData(default_m_base_url, default_m_file_search_path, default_m_upload_path, default_m_has_autoindex,
        default_m_accepted_methods, default_m_cgi_file_extensions);
    setResponseData();

    status_code_map_t   m_status_codes = setDefaultStatusCodes();

    res.buildResponse(m_status_codes);

    EXPECT_TRUE(res.getStartLine() == "HTTP/1.1 505 HTTP Version Not Supported\r\n");
}

TEST_F(TestResponse, InvalidGetRequestBasicConfBody)
{
    setRequestData(basic_get_request_wrong_string);
    setRouteData(default_m_base_url, default_m_file_search_path, default_m_upload_path, default_m_has_autoindex,
        default_m_accepted_methods, default_m_cgi_file_extensions);
    setResponseData();

    status_code_map_t   m_status_codes = setDefaultStatusCodes();

    res.buildResponse(m_status_codes);

    std::string correct_response_body = readFileIntoString("conf/error_pages/505.html");

    EXPECT_TRUE(res.getBody() == correct_response_body);
}
