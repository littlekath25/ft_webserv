#include <gtest/gtest.h>

#include <utility>
#include <fstream>
#include <sys/stat.h>

#define private public
#include <FileParser.hpp>

#define SYS_ERROR (-1)

TEST(FileParserConstruct, Default)
{
    FileParser  parser("tests");

    EXPECT_TRUE(!parser.getFilePath().empty());
    EXPECT_TRUE(!parser.getCwd().empty());
    EXPECT_TRUE(parser.getConfigData().empty());
}

TEST(FileParserConstruct, Copy)
{
    FileParser  parser("tests");
    FileParser  cpy(parser);

    EXPECT_EQ(parser.getFilePath(), cpy.getFilePath());
    EXPECT_EQ(parser.getCwd(), cpy.getCwd());
    EXPECT_EQ(parser.getConfigData(), cpy.getConfigData());
}

TEST(FileParserContruct, OperatorEquals)
{
    FileParser  one("tests");
    FileParser  two("nope");

    two = one;
    EXPECT_EQ(one.getFilePath(), two.getFilePath());
    EXPECT_EQ(one.getCwd(), two.getCwd());
    EXPECT_EQ(one.getConfigData(), two.getConfigData());
}

class FileParserValidParameterizedFixture : public ::testing::TestWithParam<std::string>
{
protected:
    std::fstream    file;

};

TEST_P(FileParserValidParameterizedFixture, ValidInputLoad)
{
    FileParser      parser("tests");
    std::string     file_name("CONFIG_FILE");
    std::string     param;

    std::remove(file_name.c_str());
    file.open(file_name, std::ios::in | std::ios::out | std::ios::app);
    ASSERT_TRUE(file.good());
    param = GetParam();
    file << param;
    file.close();
    parser.setFilePath(file_name);
    EXPECT_NO_THROW(parser.loadFile());
    EXPECT_EQ(parser.getConfigData(), param);
    std::remove(file_name.c_str());
}

TEST_P(FileParserValidParameterizedFixture, ValidInputMapping)
{
    FileParser                  parser("tests");
    std::string                 file_name("CONFIG_FILE");
    FileParser::config_vec_t    config_ports;

    std::remove(file_name.c_str());
    file.open(file_name, std::ios::in | std::ios::out | std::ios::app);
    ASSERT_TRUE(file.good());
    file << GetParam();
    file.close();
    parser.setFilePath(file_name);
    parser.loadFile();
    EXPECT_NO_THROW(parser.mapTokens(config_ports));
    std::remove(file_name.c_str());
}

INSTANTIATE_TEST_SUITE_P(
    FileParserValid,
    FileParserValidParameterizedFixture,
    ::testing::Values(
        /* config with comments */
        "# start the configuration file. Only one http block is allowed\n"
        "http {\n"
        "\n"
        "\t# define a server block\n"
        "\tserver {\n"
        "\n"
        "\t\t# address to listen on in the form <address>:<port>. Only one listen is allowed\n"
        "\t\tlisten          *:8083\n"
        "\n"
        "\t\t# server_name(s) used for matching server using \"Host\" Request header\n"
        "\t\tserver_name     example.org www.example.org\n"
        "\n"
        "        # describes the maximum body size of a request in MB\n"
        "        client_max_body_size 10\n"
        "\n"
        "        # error page in the form \"error_page <http_error_number> <full_path_to_file>.html\"\n"
        "        error_page 400 /full/path/to/file.html\n"
        "\n"
        "        # setup a location with a base_pathname to match incoming request\n"
        "        location / {\n"
        "\n"
        "            # HTTP methods allowed for this (only GET POST DELETE are allowed)\n"
        "            allowed_methods GET POST DELETE\n"
        "\n"
        "            # sets the file_path to start looking for requested files\n"
        "            root            /data/w3\n"
        "\n"
        "            # provide directory listing if requested file is not found (either \"on\" or \"off\")\n"
        "            autoindex       on\n"
        "\n"
        "            # default file to answer if the request is a directory\n"
        "            index           index.html index.htm\n"
        "\n"
        "            # possible file extensions for which to pass to CGI\n"
        "            cgi_extensions  .php .py\n"
        "\n"
        "            # file_path of where to upload files. When set will allow uploading files\n"
        "            upload_path     ./\n"
        "\n"
        "            # return a redirect in the form \"return 3<xx> <redirect_url>\"\n"
        "            return          301 www.google.com\n"
        "        }\n"
        "\t}\n"
        "}\n",
        /* config without comments */
        "http {\n"
        "\tserver {\n"
        "\t\tlisten          *:8084\n"
        "\t\tserver_name     example.org www.example.org\n"
        "        client_max_body_size 10\n"
        "        error_page 400 /full/path/to/file.html\n"
        "        location / {\n"
        "            allowed_methods GET POST DELETE\n"
        "            root            /data/w3\n"
        "            autoindex       on\n"
        "            index           index.html index.htm\n"
        "            cgi_extensions  .php .py\n"
        "            upload_path     ./\n"
        "            return          301 www.google.com\n"
        "        }\n"
        "\t}\n"
        "}",
        /* using only defaults */
        "http {\n"
        "\tserver {\n"
        "        location /base {\n"
        "        }\n"
        "\t}\n"
        "}",
        /* using only defaults multiple routes */
        "http {\n"
        "\tserver {\n"
        "        listen\t8085"
        "        location /one {\n"
        "        }\n"
        "        location /two {\n"
        "        }\n"
        "        location /three {\n"
        "        }\n"
        "        location /four {\n"
        "        }\n"
        "\t}\n"
        "}"
    )
);

TEST(FileParserLoad, InvalidNotExists)
{
    FileParser  parser("tests");

    parser.setFilePath("NOT_EXISTENT");
    EXPECT_THROW(parser.loadFile(), FileParser::InvalidFile);
}

TEST(FileParserLoad, InvalidPermissions)
{
    FileParser  parser("tests");
    std::string file_name("INVALID_PERMISSIONS");

    std::remove(file_name.c_str());
    std::fstream    brackets_file(file_name, std::ios::in | std::ios::out | std::ios::app);
    parser.setFilePath(file_name);
    EXPECT_TRUE(chmod(file_name.c_str(), 0000) != SYS_ERROR);
    EXPECT_THROW(parser.loadFile(), FileParser::InvalidFile);
    std::remove(file_name.c_str());
}

class FileParserInValidParameterizedFixture : public ::testing::TestWithParam<std::string>
{
protected:
    std::fstream    file;

};

TEST_P(FileParserInValidParameterizedFixture, InvalidMapping)
{
    FileParser                  parser("tests");
    std::string                 file_name("INVALID_FILE");
    FileParser::config_vec_t    config_ports;

    std::remove(file_name.c_str());
    file.open(file_name, std::ios::in | std::ios::out | std::ios::app);
    ASSERT_TRUE(file.good());
    file << GetParam();
    file.close();
    parser.setFilePath(file_name);
    parser.loadFile();
    EXPECT_THROW(parser.mapTokens(config_ports), FileParser::MappingFailure);
    std::remove(file_name.c_str());
}

INSTANTIATE_TEST_SUITE_P(
    FileParserInValid,
    FileParserInValidParameterizedFixture,
    ::testing::Values(
        "http {\n"
        "    server {\n"
        "    \n"
        "}",
        "http {\n"
        "    server {\n"
        "\n"
        "        }\n"
        "    }\n"
        "}",
        "http {\n"
        "    server {\n"
        "\n"
        "            }\n"
        "        }\n"
        "    {\n"
        "    }\n"
        "}",
        "listen  *:8083\n"
        "http {\n"
        "\tserver {\n"
        "\t\tlisten          *:8082\n"
        "\t\tserver_name     example.org www.example.org\n"
        "        client_max_body_size 10\n"
        "        error_page 400 /full/path/to/file.html\n"
        "        location / {\n"
        "            allowed_methods GET POST DELETE\n"
        "            root            /data/w3\n"
        "            autoindex       on\n"
        "            index           index.html index.htm\n"
        "            cgi_extensions  .php .py\n"
        "            upload_path     ./\n"
        "            return          301 www.google.com\n"
        "        }\n"
        "\t}\n"
        "}",
        "http {\n"
        "\tlisten  *:8083\n"
        "\tserver {\n"
        "\t\tlisten          *:8082\n"
        "\t\tserver_name     example.org www.example.org\n"
        "        client_max_body_size 10\n"
        "        error_page 400 /full/path/to/file.html\n"
        "        location / {\n"
        "            allowed_methods GET POST DELETE\n"
        "            root            /data/w3\n"
        "            autoindex       on\n"
        "            index           index.html index.htm\n"
        "            cgi_extensions  .php .py\n"
        "            upload_path     ./\n"
        "            return          301 www.google.com\n"
        "        }\n"
        "\t}\n"
        "}",
        "http {\n"
        "\tserver {\n"
        "\t\tlisten          *:8082\n"
        "\t\tserver_name     example.org www.example.org\n"
        "        client_max_body_size 10\n"
        "        root            on\n"
        "        error_page 400 /full/path/to/file.html\n"
        "        location / {\n"
        "            allowed_methods GET POST DELETE\n"
        "            root            /data/w3\n"
        "            autoindex       on\n"
        "            index           index.html index.htm\n"
        "            cgi_extensions  .php .py\n"
        "            upload_path     ./\n"
        "            return          301 www.google.com\n"
        "        }\n"
        "\t}\n"
        "}",
        "http {\n"
        "\tserver {\n"
        "\t\tlisten          *:8082\n"
        "\t\tserver_name     example.org www.example.org\n"
        "        client_max_body_size 10\n"
        "        error_page 400 /full/path/to/file.html\n"
        "        location / {\n"
        "            allowed_methods GET POST DELETE\n"
        "            root            /data/w3\n"
        "            autoindex       on\n"
        "            index           index.html index.htm\n"
        "            server_name     lol.com\n"
        "            cgi_extensions  .php .py\n"
        "            upload_path     ./\n"
        "            return          301 www.google.com\n"
        "        }\n"
        "\t}\n"
        "}"
    )
);

static std::string  basic_conf = ""
     "# start the configuration file. Only one http block is allowed\n"
     "http {\n"
     "\n"
     "\t# define a server block\n"
     "\tserver {\n"
     "\n"
     "\t\t# address to listen on in the form <address>:<port>. Only one listen is allowed\n"
     "\t\tlisten          *:8082\n"
     "\n"
     "\t\t# server_name(s) used for matching server using \"Host\" Request header\n"
     "\t\tserver_name     example.org www.example.org\n"
     "\n"
     "        # describes the maximum body size of a request in MB\n"
     "        client_max_body_size 10\n"
     "\n"
     "        # error page in the form \"error_page <http_error_number> <full_path_to_file>.html\"\n"
     "        error_page 400 /full/path/to/file.html\n"
     "\n"
     "        # setup a location with a base_pathname to match incoming request\n"
     "        location / {\n"
     "\n"
     "            # HTTP methods allowed for this (only GET POST DELETE are allowed)\n"
     "            allowed_methods GET POST DELETE\n"
     "\n"
     "            # sets the file_path to start looking for requested files\n"
     "            root            /\n"
     "\n"
     "            # provide directory listing if requested file is not found (either \"on\" or \"off\")\n"
     "            autoindex       on\n"
     "\n"
     "            # default file to answer if the request is a directory\n"
     "            index           index.html index.htm\n"
     "\n"
     "            # possible file extensions for which to pass to CGI\n"
     "            cgi_extensions  .php .py\n"
     "\n"
     "            # file_path of where to upload files. When set will allow uploading files\n"
     "            upload_path     /\n"
     "\n"
     "            # return a redirect in the form \"return 3<xx> <redirect_url>\"\n"
     "            return          301 www.google.com\n"
     "        }\n"
     "\t}\n"
     "}\n";

/*
 * TODO
 * 1. execute from different locations to test .realpath()
 */

TEST(FileParserFilesValues, ParseValidSingle)
{
    std::string                 file_name("BRACKETS_FILE");
    FileParser                  parser("tests");
    FileParser::config_vec_t    port_configs;
    std::vector<std::string>    cmp_names = {"example.org", "www.example.org"};
    std::vector<std::string>    cmp_methods = {"POST", "GET", "DELETE"};
    std::vector<std::string>    cmp_files = {"index.html", "index.htm"};
    std::vector<std::string>    cmp_ext = {".php", ".py"};

    std::remove(file_name.c_str());
    std::fstream    brackets_file(file_name, std::ios::in | std::ios::out | std::ios::app);

    brackets_file << basic_conf;
    brackets_file.close();

    parser.setFilePath(file_name);
    ASSERT_NO_THROW(parser.loadFile());
    ASSERT_EQ(parser.m_config_data, basic_conf);
    ASSERT_NO_THROW(parser.mapTokens(port_configs));

    for (auto config : port_configs)
    {
        ASSERT_EQ(config->getServers().size(), 1);
        for (auto server : config->getServers())
        {
            EXPECT_STREQ(server->m_host.c_str(), "*");
            EXPECT_EQ(server->m_port, 8082);
            EXPECT_EQ(server->m_names.size(), 2);
            for (auto& name : server->m_names)
            {
                EXPECT_TRUE(std::find(cmp_names.begin(), cmp_names.end(), name) != cmp_names.end());
            }
            EXPECT_EQ(server->m_client_max_body_size, 10);
            EXPECT_EQ(server->m_routes.size(), 1);
            for (auto& route : server->m_routes)
            {
                EXPECT_EQ(route->m_accepted_methods.size(), 3);
                for (auto& method : route->m_accepted_methods)
                {
                    EXPECT_TRUE(std::find(cmp_methods.begin(), cmp_methods.end(), method) != cmp_methods.end());
                }
                EXPECT_STREQ(route->m_base_url.c_str(), "/");
                EXPECT_STREQ(route->m_file_search_path.c_str(), DFL_FILE_SEARCH_PATH);
                EXPECT_EQ(route->m_has_autoindex, true);
                EXPECT_EQ(route->m_index_files.size(), 2);
                for (auto& file : route->m_index_files)
                {
                    EXPECT_TRUE(std::find(cmp_files.begin(), cmp_files.end(), file) != cmp_files.end());
                }
                EXPECT_EQ(route->m_cgi_file_extensions.size(), 2);
                for (auto& ext : route->m_cgi_file_extensions)
                {
                    EXPECT_TRUE(std::find(cmp_ext.begin(), cmp_ext.end(), ext) != cmp_ext.end());
                }
                EXPECT_STREQ(route->m_upload_path.c_str(), DFL_UPLOAD_PATH);
                EXPECT_TRUE(route->m_redirect);
                EXPECT_EQ(route->m_redirect->status_code, 301);
                EXPECT_STREQ(route->m_redirect->url.c_str(), "www.google.com");
            }
        }
    }
    std::remove(file_name.c_str());
}

TEST(FileParserFilesValues, ParseValidMultiple)
{

}

/*
 * Servers with the same port should be grouped inside
 * of the same ConfigPort object.
 */
TEST(FileParserFilesValues, ParseValidGrouped)
{

}
