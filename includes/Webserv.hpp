#pragma once

/*
 * You can convert numbers to strings using Stringizing:
 * https://gcc.gnu.org/onlinedocs/cpp/Stringizing.html
 */
#define STR(x)          #x

#define Webserv_VERSION_MAJOR STR(1)
#define Webserv_VERSION_MINOR STR(1)

#define PROG_NAME       "Websurf/" Webserv_VERSION_MAJOR "." Webserv_VERSION_MINOR " (Unix)"

#define SYS_ERROR       (-1)
#define HTTP_VERSION    "HTTP/1.1"
#define CR              "\r"
#define LF              "\n"

#define HTTP_VERSION   "HTTP/1.1"
#define CGI_VERSION    "CGI/1.1"

#define DFL_CGI_DIR    "cgi-bin/"
#define RECV_SIZE      2048

#define DFL_FILE_SEARCH_PATH   "html/"
#ifdef TESTRUN
# define DFL_UPLOAD_PATH        "../../html/"
#else
# define DFL_UPLOAD_PATH        "html/"
#endif
#define DFL_INDEX_FILE         "index.html"

#define DFL_SERVER_HOST "*"     /* translates to INADDR_ANY 0.0.0.0 */

#include <exception>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <stack>

#include <cstring>
#include <cerrno>
#include <cstdlib>

#include <unistd.h>
#include <pthread.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <csignal>
#include <sys/poll.h>

typedef void            *(*THREAD_FUNC_PTR)(void *);
