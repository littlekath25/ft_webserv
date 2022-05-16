#include <Handler.hpp>

bool Handler::post_handler(Request &request, std::string uploadPath)
{
    std::string filename;
    std::ofstream newFile;

    if (!request.getFilename().empty())
    {
        filename = uploadPath + request.getFilename();
    }
    else if (request.getHeaders().find("Content-Type") != request.getHeaders().end())
    {
        std::string extension = request.getHeaders().find("Content-Type")->second;
        extension = extension.substr(extension.find('/') + 1);
        filename = uploadPath + "test." + extension;
    }
    else
    {
        request.setStatus(HTTP_STATUS_BAD_REQUEST);
        return (false);
    }
    newFile.open(filename, std::ofstream::out | std::ofstream::binary);
    if (!newFile.is_open())
        return (false);
    newFile << request.getBody();
    newFile.close();
    return (true);
}
