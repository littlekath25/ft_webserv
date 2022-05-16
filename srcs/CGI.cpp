#include <CGI.hpp>

CGI::CGI() :
    m_argv(NULL),
    m_envp(NULL),
    m_fork_pid(UNSET_PID),
    m_done(false)
{
    std::memset(m_pipe_in, 0, sizeof(m_pipe_in));
    std::memset(m_pipe_out, 0, sizeof(m_pipe_out));
}

CGI::CGI(const CGI &cpy)
{
    *this = cpy;
}

CGI::~CGI()
{
    ft::freeCharArr(&m_envp);
    ft::freeCharArr(&m_argv);
}

CGI&                CGI::operator=(const CGI &rhs)
{
    if (this != &rhs)
    {
        m_program_path = rhs.m_program_path;
        m_environ = rhs.m_environ;
        ft::freeCharArr(&m_envp);
        m_envp = _environToEnvp();
        m_args = rhs.m_args;
        ft::freeCharArr(&m_argv);
        m_argv = _argsToArgv();
        m_pipe_in[0] = rhs.m_pipe_in[0];
        m_pipe_in[1] = rhs.m_pipe_in[1];
        m_pipe_out[0] = rhs.m_pipe_out[0];
        m_pipe_out[1] = rhs.m_pipe_out[1];
    }
    return (*this);
}

void                CGI::setProgramPath(const std::string& path)
{
    m_program_path = path;
}

void                CGI::setDone(void)
{
    m_done = true;
}

bool                CGI::isDone(void)
{
    return (m_done);
}

pid_t&              CGI::getForkedPid(void)
{
    return (m_fork_pid);
}

int&                CGI::getPipeReadFd(void)
{
    return (m_pipe_out[0]);
}

std::string         CGI::getResponse(void)
{
    return (m_response);
}

bool           CGI::readAndAppend()
{
    ssize_t bytes_read;
    char    *buff;

    buff = new char[RECV_SIZE + 1];
    if ((bytes_read = ::read(getPipeReadFd(), buff, RECV_SIZE)) == SYS_ERROR) {
        fprintf(stderr, "Failed to read from socket: %s\n", strerror(errno));
        return (false);
    }
    else
    {
        appendResponse(buff, bytes_read);
        if (bytes_read < RECV_SIZE)
            setDone();
    }
    delete [] buff;
    return (true);
}

pollfd         CGI::getPollFdStruct(void)
{
    struct pollfd new_pollfd;

    new_pollfd.fd = m_pipe_out[0];
    new_pollfd.events = (POLLIN);
    new_pollfd.revents = 0;
    return (new_pollfd);
}

/*
 * https://en.wikipedia.org/wiki/Common_Gateway_Interface
 *
 *  Server specific variables:
 *      SERVER_SOFTWARE:    name/version of HTTP server.
 *      SERVER_NAME:        host name of the server, may be dot-decimal IP address.
 *      GATEWAY_INTERFACE:  CGI/version.
 *  Request specific variables:
 *      SERVER_PROTOCOL:    HTTP/version.
 *      SERVER_PORT:        TCP port (decimal).
 *      REQUEST_METHOD:     name of HTTP method (see above).
 *      PATH_INFO:          path suffix, if appended to URL after program name and a slash.
 *      PATH_TRANSLATED:    corresponding full path as supposed by server, if PATH_INFO is present.
 *      SCRIPT_NAME:        relative path to the program, like /cgi-bin/script.cgi.
 *      QUERY_STRING:       the part of URL after ? character. The query string may be composed of
 *                          *name=value pairs separated with ampersands (such as var1=val1&var2=val2...)
 *                          when used to submit form data transferred via GET method as defined by HTML
 *                          application/x-www-form-urlencoded.
 *      REMOTE_HOST:        host name of the client, unset if server did not perform such lookup.
 *      REMOTE_ADDR:        IP address of the client (dot-decimal).
 *      AUTH_TYPE:          identification type, if applicable.
 *      REMOTE_USER:        used for certain AUTH_TYPEs.
 *      REMOTE_IDENT:       see ident, only if server performed such lookup.
 *      CONTENT_TYPE:       Internet media type of input data if PUT or POST method are used, as provided
 *                          via HTTP header.
 *      CONTENT_LENGTH:     similarly, size of input data (decimal, in octets) if provided via HTTP header.
 *
 *      Variables passed by user agent (HTTP_ACCEPT, HTTP_ACCEPT_LANGUAGE, HTTP_USER_AGENT, HTTP_COOKIE and
 *      possibly others) contain values of corresponding HTTP headers and therefore have the same sense.
 */
void                CGI::init(Request& request)
{
    std::string host;
    if (request.getHeaders().find("Host") != request.getHeaders().end())
        host = request.getHeaders().find("Host")->second;

    /* set environment variables for the CGI request */
    m_environ["SERVER_SOFTWARE"] = PROG_NAME;
    if (!host.empty())
    {
        std::string hostName = host.substr(0, host.find(":"));
        m_environ["SERVER_NAME"] = hostName;
    }
    m_environ["GATEWAY_INTERFACE"] = CGI_VERSION;
    m_environ["SERVER_PROTOCOL"] = request.getVersion();
    if (!host.empty())
    {
        std::string hostPort = host.substr(host.find(":") + 1);
        m_environ["SERVER_PORT"] = hostPort;
    }
    else
    {
        m_environ["SERVER_PORT"] = "8080";
    }
    m_environ["REQUEST_METHOD"] = request.getMethod();
    m_environ["PATH_INFO"] = request.getCGIPath();
    m_environ["SCRIPT_NAME"] = request.getFilename();
    m_environ["QUERY_STRING"] = request.getQuery();
    m_environ["REMOTE_HOST"] = "";
    m_environ["REMOTE_ADDR"] = "";
    m_environ["CONTENT_TYPE"] = _getMapValueWithDefault(
            request.getHeaders(),
            std::string("Content-Type"),
            std::string("text/html"));
    m_environ["CONTENT_LENGTH"] = _getMapValueWithDefault(
            request.getHeaders(),
            std::string("Content-Length"),
            ft::intToString(request.getBody().size())
    );
    m_envp = _environToEnvp();
    if (!m_envp)
        throw std::runtime_error("Failed to allocate for GGI::m_envp");

    m_args.push_back(m_program_path);
    m_argv = _argsToArgv();
    if (!m_argv)
        throw std::runtime_error("Failed to allocate for CGI::m_argv");

    /* create the two pipes needed for .exec and make output pipe's read part `non_blocking` */
    if (pipe(m_pipe_in) == SYS_ERROR || pipe(m_pipe_out) == SYS_ERROR)
        throw std::runtime_error("Failed to create pipes for CGI object");
    fcntl(m_pipe_out[0], F_SETFL, O_NONBLOCK);
    m_request_body = request.getBody();
}

static void cgi_exit(void)
{
    std::string err("ERROR");

    write(STDOUT_FILENO, err.c_str(), err.size());
    exit(EXIT_FAILURE);
}

/*
 * The forked process will use execve to run the CGI program.
 * If the request body has a size large than MAX_PIPE_SIZE, then
 * a file will be used instead since this has no performance impact.
 *
 * https://unix.stackexchange.com/questions/11946/how-big-is-the-pipe-buffer
 *
 */
int                 CGI::exec(void)
{
    pid_t   pid;

    if (m_pipe_in[0] == 0 || m_pipe_in[1] == 0)
        throw std::invalid_argument("CGI class not initialised");

    if ((pid = fork()) == SYS_ERROR)
        return (SYS_ERROR);

    if (pid == 0)
    {
        /* use dup to make sure the CGI program reads from the pipe */
        if (dup2(m_pipe_in[0], STDIN_FILENO) == SYS_ERROR)
            cgi_exit();

        /* write request body to write part of the pipe so CGI can read it */
        if (write(m_pipe_in[1], m_request_body.c_str(), m_request_body.size()) == SYS_ERROR)
            cgi_exit();
        close(m_pipe_in[1]);

        /* use dup to make sure the CGI program writes to the out pipe instead of STDOUT */
        if (dup2(m_pipe_out[1], STDOUT_FILENO) == SYS_ERROR)
            cgi_exit();

        /* call the CGI program using provided arguments. This call will take over child process */
        if (execve(m_program_path.c_str(), m_argv, m_envp) == SYS_ERROR)
        {
            /*
             * write the error description to the write end of the out pipe
             * so that the parent still gets the POLL_IN notification from poll.
             * exit(EXIT_FAILURE) should tell the parent process that it failed
             */
            cgi_exit();
        }
    }
    else
    {
        m_fork_pid = pid;
    }
    return (EXIT_SUCCESS);
}

void                CGI::reset(void)
{
    m_program_path.clear();
    m_request_body.clear();
    ft::freeCharArr(&m_envp);
    ft::freeCharArr(&m_argv);
    m_environ.clear();
    m_fork_pid = UNSET_PID;
    std::memset(m_pipe_in, 0, sizeof(m_pipe_in));
    std::memset(m_pipe_out, 0, sizeof(m_pipe_out));
}

char                **CGI::_environToEnvp(void)
{
    int             idx;
    env_t::iterator it;
    std::string     tmp;
    char            **envp;

    envp = new(std::nothrow) char*[m_environ.size() + 1];
    if (!envp)
        return (NULL);
    it = m_environ.begin();
    for (idx = 0; it != m_environ.end(); ++it, ++idx)
    {
        tmp = it->first + "=" + it->second;
        envp[idx] = ft::strdup(tmp.c_str());
        if (!envp[idx])
        {
            while (idx > 0)
            {
                delete envp[idx];
                --idx;
            }
            delete [] envp;
            return (NULL);
        }
    }
    envp[idx] = NULL;
    return (envp);
}

char                **CGI::_argsToArgv(void)
{
    size_t  idx;
    char    **argv;

    argv = new(std::nothrow) char*[m_args.size() + 1];
    if (!argv)
        return (NULL);
    for (idx = 0; idx < m_args.size(); ++idx)
    {
        argv[idx] = ft::strdup(m_args[idx].c_str());
        if (!argv[idx])
        {
            while (idx > 0)
            {
                delete argv[idx];
                --idx;
            }
            delete [] argv;
            return (NULL);
        }
    }
    argv[idx] = NULL;
    return (argv);
}

void       CGI::appendResponse(char *response, ssize_t size)
{
    m_response.append(response, size);
}
