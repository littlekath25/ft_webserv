#include <unittests.hpp>

size_t  writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data)
{
    data->append((char*) ptr, size * nmemb);
    return (size * nmemb);
}

int     sendEmptyRequest(const char *url, std::string* data)
{
    CURL        *curl;
    std::string response_data;
    std::string header_data;

    curl = curl_easy_init();
    if (!curl)
        return (SYS_ERROR);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_data);

    usleep(SOCKET_SETUP_TIME);
    curl_easy_perform(curl);

    data->append(header_data);
    data->append(response_data);

    curl_easy_cleanup(curl);
    return (EXIT_SUCCESS);
}

static size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
    char		*s;
    size_t		nleft;

    if (src == NULL || dst == NULL)
        return (0);
    s = (char *)src;
    nleft = size;
    if (nleft != 0)
    {
        nleft -= 1;
        while (*s != '\0' && nleft != 0)
        {
            *dst = *s;
            dst++;
            s++;
            nleft--;
        }
    }
    if (size != 0)
        *dst = '\0';
    while (*s)
        s++;
    return (s - src);
}

size_t readFunction(char *buffer, size_t size, size_t nitems, std::string* userdata)
{
    size_t ret = ft_strlcpy(buffer, userdata->c_str(), (size * nitems));
    return (ret);
}

int     sendBigRequest(const char *url, std::string* data)
{
    CURL        *curl;
    std::string response_data;
    std::string header_data;
    std::string read_data( 2048 * 2, '-' );

    curl = curl_easy_init();
    if (!curl)
        return (SYS_ERROR);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, readFunction);
    curl_easy_setopt(curl, CURLOPT_READDATA, &read_data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_data);

    usleep(SOCKET_SETUP_TIME);
    curl_easy_perform(curl);

    data->append(header_data);
    data->append(response_data);

    curl_easy_cleanup(curl);
    return (EXIT_SUCCESS);
}
