#include <Handler.hpp>

bool Handler::delete_handler(Request &request, std::string searchPath)
{
    std::string     filename;
    std::fstream    fileStream;

    filename = searchPath + request.getFilename();
    if (request.getFilename().empty())
    {
        request.setStatus(HTTP_STATUS_BAD_REQUEST);
        return (false);
    }
    if (!filename.empty())
    {
        fileStream.open(filename);
        if (fileStream.fail()) {
            request.setStatus(HTTP_STATUS_NOT_FOUND);
            return (false);
        }
        if (std::remove(filename.c_str()) != 0)
        {
            request.setStatus(HTTP_STATUS_INTERNAL_SERVER_ERROR);
            return (false);
        }
    }
    return (true);
}
