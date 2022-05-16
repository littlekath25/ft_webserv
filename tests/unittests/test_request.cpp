#include "unittests.hpp"

#define private public

#include <Request.hpp>

// Good requests
static std::string  basic_get_request =
        "GET /?parameter1=waarde1&parameter2=waarde2&parameter3=waarde3 HTTP/1.1\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.51 Safari/537.36\r\n"
        "Host: localhost\r\n"
        "Accept-Language: en-us\r\n"
        "Accept-Encoding: gzip, deflate\r\n\r\n";

static std::string  basic_get_request2 =
        "GET /upload HTTP/1.1\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.51 Safari/537.36\r\n"
        "Host: localhost\r\n"
        "Accept-Language: en-us\r\n"
        "Accept-Encoding: gzip, deflate\r\n\r\n";

static std::string  basic_post_request =
        "POST /index.html HTTP/1.1\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.51 Safari/537.36\r\n"
        "Host: localhost:8082\r\n"
        "Content-Type: text/xml; charset=utf-8\r\n"
        "Content-Length: 35\r\n"
        "Accept-Language: en-us\r\n"
        "Accept-Encoding: gzip, deflate\r\n\r\n"
        "<html><h1>Goodbye World</h1></html>";

static std::string  basic_delete_request =
        "DELETE /index.html HTTP/1.1\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.51 Safari/537.36\r\n"
        "Host: localhost\r\n\r\n";

// Bad requests
static std::string  basic_get_request_wrong =
        "GET / HTTP/1.0\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.51 Safari/537.36\r\n"
        "Host: localhost:8080\r\n"
        "Accept-Language: en-us\r\n"
        "Accept-Encoding: gzip, deflate\r\n\r\n";
static std::string  basic_post_request_wrong =
        "POST /index.html HTTP/1.1\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.51 Safari/537.36\r\n"
        "Content-Type: text/xml; charset=utf-8\r\n"
        "Content-Length: 35\r\n"
        "Accept-Language: en-us\r\n"
        "Accept-Encoding: gzip, deflate\r\n\r\n"
        "<html><h1>Goodbye World</h1></html>";
static std::string  basic_delete_request_wrong =
        "DELETe /index.html HTTP/1.1\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.51 Safari/537.36\r\n"
        "Host: localhost\r\n\r\n";

// Advanced requests
static std::string  advanced_get_request =
        "GET /script.py/html/index HTTP/1.1\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.51 Safari/537.36\r\n"
        "Host: localhost\r\n"
        "Accept-Language: en-us\r\n"
        "Accept-Encoding: gzip, deflate\r\n\r\n";

// Chunked requests
static std::string chunked_post_request =
        "POST /test.html HTTP/1.1\r\n"
        "Host: localhost:8082\r\n"
        "Content-Encoding: chunked\r\n\r\n"
        "4\r\n"
        "Wiki\r\n"
        "6\r\n"
        "pedia \r\n"
        "E\r\n"
        "in \r\n\r\n"
        "chunks.\r\n"
        "0\r\n\r\n";

static std::string chunked_post_request2 =
        "POST /test.html HTTP/1.1\r\n"
        "Host: localhost:8082\r\n"
        "Content-Encoding: chunked\r\n\r\n"
        "B\r\n"
        "Bring this \r\n"
        "5\r\n"
        "chunk\r\n"
        "3\r\n"
        "ed \r\n"
        "6\r\n"
        "to me \r\n"
        "8\r\n"
        "please a\r\n"
        "1\r\n"
        "n\r\n"
        "2\r\n"
        "d \r\n"
        "6\r\n"
        "thank \r\n"
        "3\r\n"
        "you\r\n"
        "0\r\n"
        "\r\n";

static std::string chunked_post_request3 =
        "POST /test.html HTTP/1.1\r\n"
        "Host: localhost:8082\r\n"
        "Content-Encoding: chunked\r\n\r\n"
        "19\r\n"
        "this is a very long test \r\n"
        "8\r\n"
        "request \r\n"
        "26\r\n"
        "i don't know what to say to be honest.\r\n"
        "0\r\n\r\n";

static std::string empty_chunked_post_request =
        "POST /test.html HTTP/1.1\r\n"
        "Host: localhost:8082\r\n"
        "Content-Encoding: chunked\r\n\r\n";

// Basic requests that would succeed
class TestBasicRequest : public ::testing::Test
{
public:
    Request req;
protected:
    void setRequestData(const std::string& data)
    {
        req.m_request = data;
        req.parse();
        req.errorChecking();
    }
};

TEST_F(TestBasicRequest, GetRequest)
{
    setRequestData(basic_get_request);
    EXPECT_TRUE(req.getMethod() == "GET");
    EXPECT_TRUE(req.getTarget() == "/");
    EXPECT_TRUE(req.getQuery() == "parameter1=waarde1&parameter2=waarde2&parameter3=waarde3");
    EXPECT_TRUE(req.getVersion() == HTTP_VERSION);
    EXPECT_TRUE(req.getPort() == 80);
    EXPECT_TRUE(req.getStatus() == HTTP_STATUS_OK);
    setRequestData(basic_get_request);
    std::map<std::string, std::string> TestMap =
    {
        {"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.51 Safari/537.36"},
        {"Host", "localhost"},
        {"Accept-Language", "en-us"},
        {"Accept-Encoding", "gzip, deflate"}
    };
    EXPECT_TRUE(req.getHeaders() == TestMap);
    EXPECT_TRUE(req.getBody().empty());
}

TEST_F(TestBasicRequest, GetRequest2)
{
    setRequestData(basic_get_request2);
    EXPECT_TRUE(req.getMethod() == "GET");
    EXPECT_TRUE(req.getTarget() == "/upload");
    EXPECT_TRUE(req.getFilename().empty());
}


TEST_F(TestBasicRequest, PostRequest)
{
    setRequestData(basic_post_request);
    EXPECT_TRUE(req.getMethod() == "POST");
    EXPECT_TRUE(req.getTarget() == "/");
    EXPECT_TRUE(req.getFilename() == "index.html");
    EXPECT_TRUE(req.getQuery().empty());
    EXPECT_TRUE(req.getVersion() == HTTP_VERSION);
    EXPECT_TRUE(req.getPort() == 8082);
    std::map<std::string, std::string> TestMap =
    {
        {"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.51 Safari/537.36"},
        {"Host", "localhost:8082"},
        {"Content-Type", "text/xml; charset=utf-8"},
        {"Content-Length", "35"},
        {"Accept-Language", "en-us"},
        {"Accept-Encoding", "gzip, deflate"}
    };
    EXPECT_TRUE(req.getHeaders() == TestMap);
    EXPECT_TRUE(req.getBody() == "<html><h1>Goodbye World</h1></html>");
    EXPECT_TRUE(req.getStatus() == HTTP_STATUS_OK);
}

TEST_F(TestBasicRequest, DeleteRequest)
{
    setRequestData(basic_delete_request);
    EXPECT_TRUE(req.getMethod() == "DELETE");
    EXPECT_TRUE(req.getTarget() == "/");
    EXPECT_TRUE(req.getFilename() == "index.html");
    EXPECT_TRUE(req.getQuery().empty());
    EXPECT_TRUE(req.getVersion() == HTTP_VERSION);
    EXPECT_TRUE(req.getPort() == 80);
    std::map<std::string, std::string> TestMap =
    {
        {"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.51 Safari/537.36"},
        {"Host", "localhost"}
    };
    EXPECT_TRUE(req.getHeaders() == TestMap);
    EXPECT_TRUE(req.getBody().empty());
    EXPECT_TRUE(req.getStatus() == HTTP_STATUS_OK);
}

TEST_F(TestBasicRequest, WrongGetRequest)
{
    setRequestData(basic_get_request_wrong);
    EXPECT_TRUE(req.getStatus() == HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED);
}

TEST_F(TestBasicRequest, WrongPostRequest)
{
    setRequestData(basic_post_request_wrong);
    EXPECT_TRUE(req.getStatus() == HTTP_STATUS_BAD_REQUEST);
}

TEST_F(TestBasicRequest, WrongDeleteRequest)
{
    setRequestData(basic_delete_request_wrong);
    EXPECT_TRUE(req.getStatus() == HTTP_STATUS_METHOD_NOT_ALLOWED);
}

TEST_F(TestBasicRequest, AdvancedGetRequest)
{
    setRequestData(advanced_get_request);
    EXPECT_TRUE(req.getMethod() == "GET");
    EXPECT_TRUE(req.getTarget() == "/");
    EXPECT_TRUE(req.getFilename() == "script.py");
    EXPECT_TRUE(req.getQuery().empty());
    EXPECT_TRUE(req.getVersion() == HTTP_VERSION);
    EXPECT_TRUE(req.getCGIPath() == "/html/index");
    EXPECT_TRUE(req.getPort() == 80);
}

TEST_F(TestBasicRequest, ChunkedPostRequest)
{
    setRequestData(chunked_post_request);
    EXPECT_EQ(req.getBody(), "Wikipedia in \r\n\r\nchunks.");
}

TEST_F(TestBasicRequest, ChunkedPostRequest2)
{
    setRequestData(chunked_post_request2);
    EXPECT_EQ(req.getBody(), "Bring this chunked to me please and thank you");
}

TEST_F(TestBasicRequest, ChunkedPostRequest3)
{
    setRequestData(chunked_post_request3);
    EXPECT_EQ(req.getBody(), "this is a very long test request i don't know what to say to be honest.");
}

TEST_F(TestBasicRequest, EmptyChunkedPostRequest)
{
    setRequestData(empty_chunked_post_request);
    EXPECT_TRUE(req.getBody().empty());
}
