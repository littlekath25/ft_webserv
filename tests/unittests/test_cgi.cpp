#include <gtest/gtest.h>
#include <sys/poll.h>
#ifdef __linux__
# include <sys/wait.h>
# include <limits.h>
#endif

#define private         public
#define POLL_NO_TIMEOUT 0
#define BUFF_TEST_SIZE  1024

#include <CGI.hpp>

const std::string basic_post = ""
       "POST /test?field1=10&field2=20 HTTP/1.1\r\n"
       "Host: foo.example\r\n"
       "Content-Type: application/x-www-form-urlencoded\r\n"
       "Content-Length: 15\r\n"
       "\r\n"
       "num1=10&num2=20";

class TestCGIFixture : public ::testing::Test
{
public:
    std::string program_path;

protected:
    std::string getProgramPath(const char *script_name)
    {
        std::string relative_path;

#ifdef TESTRUN
        relative_path += "../../";
#endif
        relative_path += DFL_CGI_DIR;
        relative_path += "/";
        relative_path += script_name;
        return (relative_path);
    }
};

TEST_F(TestCGIFixture, SimpleInit)
{
    CGI       simple_cgi;
    Request   request;

    request.appendRequestData(basic_post.c_str(), basic_post.size());
    request.parse();
    simple_cgi.setProgramPath(getProgramPath("sleep.py"));
    simple_cgi.init(request);
    EXPECT_EQ(simple_cgi.m_environ["CONTENT_TYPE"], request.m_headers["Content-Type"]);
    EXPECT_EQ(simple_cgi.m_environ["CONTENT_LENGTH"], request.m_headers["Content-Length"]);
}

/*
 * ( Check failing CGI )
 * - Invalid program_path
 * - Invalid script
 */

TEST_F(TestCGIFixture, SimpleExecWithSleep)
{
    CGI             simple_cgi;
    Request         request;
    struct pollfd   pollfds[1];

    char            buff[BUFF_TEST_SIZE];
    const std::string   out = ""
      "HTTP/1.1 200 OK\n"
      "Content-type:text/html\r\n\r\n\n"
      "<h1>Feeling sleepy...</h1>\n";

    request.appendRequestData(basic_post.c_str(), basic_post.size());
    request.parse();
    simple_cgi.setProgramPath(getProgramPath("sleep.py"));
    simple_cgi.init(request);
    ASSERT_TRUE(simple_cgi.exec() != SYS_ERROR);

    pollfds->fd = simple_cgi.m_pipe_out[0];
    pollfds->events = POLL_IN;
    pollfds->revents = 0;
    for ( ;; )
    {
        ASSERT_TRUE(poll(pollfds, 1, POLL_NO_TIMEOUT) != SYS_ERROR);

        usleep(1000000);
        if (pollfds->revents > 0)
        {
            ASSERT_TRUE(waitpid(simple_cgi.m_fork_pid, NULL, 0) != SYS_ERROR);
            ssize_t ret = read(simple_cgi.m_pipe_out[0], buff, BUFF_TEST_SIZE);
            buff[ret] = '\0';
            //EXPECT_EQ(std::string(buff), out);
            break ;
        }
    }
}

TEST_F(TestCGIFixture, SimpleExecHelloWorld)
{
    CGI             simple_cgi;
    Request   request;
    struct pollfd   pollfds[1];

    char            buff[BUFF_TEST_SIZE];
    const std::string   out = ""
      "HTTP/1.1 200 OK\n"
      "Content-type:text/html\r\n\n"
      "<html>\n"
      "<head>\n"
      "<title>Hello World - First CGI Program</title>\n"
      "</head>\n"
      "<body>\n"
      "<h2>Hello World! This is my first CGI program</h2>\n"
      "</body>\n"
      "</html>\n";

    request.appendRequestData(basic_post.c_str(), basic_post.size());
    request.parse();
    simple_cgi.setProgramPath(getProgramPath("hello_world.py"));
    simple_cgi.init(request);
    ASSERT_TRUE(simple_cgi.exec() != SYS_ERROR);

    pollfds->fd = simple_cgi.m_pipe_out[0];
    pollfds->events = POLL_IN;
    pollfds->revents = 0;
    for ( ;; )
    {
        ASSERT_TRUE(poll(pollfds, 1, POLL_NO_TIMEOUT) != SYS_ERROR);

        usleep(1000000);
        if (pollfds->revents > 0)
        {
            ASSERT_TRUE(waitpid(simple_cgi.m_fork_pid, NULL, 0) != SYS_ERROR);
            ssize_t ret = read(simple_cgi.m_pipe_out[0], buff, BUFF_TEST_SIZE);
            buff[ret] = '\0';
            //EXPECT_STREQ(buff, out.c_str());
            break ;
        }
    }
}

TEST_F(TestCGIFixture, SimpleExecFormInput)
{
    CGI             simple_cgi;
    Request   		request;
    struct pollfd   pollfds[1];

    char            buff[BUFF_TEST_SIZE];
    const std::string   out = ""
      "HTTP/1.1 200 OK\n"
      "Content-Type:text/html\r\n\r\n\n"
      "<h1>Addition Results</h1>\n"
      "<output>10 + 20 = 30</output>\n";

    request.appendRequestData(basic_post.c_str(), basic_post.size());
    request.parse();
    simple_cgi.setProgramPath(getProgramPath("form_input.py"));
    simple_cgi.init(request);
    ASSERT_TRUE(simple_cgi.exec() != SYS_ERROR);

    pollfds->fd = simple_cgi.m_pipe_out[0];
    pollfds->events = POLL_IN;
    pollfds->revents = 0;
    for ( ;; )
    {
        ASSERT_TRUE(poll(pollfds, 1, POLL_NO_TIMEOUT) != SYS_ERROR);

        usleep(1000000);
        if (pollfds->revents > 0)
        {
            ASSERT_TRUE(waitpid(simple_cgi.m_fork_pid, NULL, 0) != SYS_ERROR);
            ssize_t ret = read(simple_cgi.m_pipe_out[0], buff, BUFF_TEST_SIZE);
            buff[ret] = '\0';
            //EXPECT_STREQ(buff, out.c_str());
            break ;
        }
    }
}
