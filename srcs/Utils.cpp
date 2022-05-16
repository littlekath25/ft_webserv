#include <Utils.hpp>

namespace ft
{

	/* simple function to convert num into string */
	std::string		intToString(int num)
	{
	    std::ostringstream ss;

	    ss << num;
		return (ss.str());
	}

	int             stringToInt(const std::string& str)
    {
	    std::stringstream   num_stream;
	    int                 num_out;

	    num_stream.exceptions( std::ios::badbit | std::ios::failbit );
	    num_stream << str;
	    try
        {
            num_stream >> num_out;
        }
        catch (std::ios::failure& e)
        {
            num_out = FAIL_CONVERSION;
        }
	    return (num_out);
    }

    /* generalised function that completely reads out a file. file_stream.good() can be check for error */
    std::string     readFileContent(std::ifstream& file_stream, const std::string& file_name)
    {
	    std::string         file_contents;
        std::stringstream   file_content_stream;

        try
        {
            file_stream.open(file_name.c_str());
            file_content_stream << file_stream.rdbuf();
            file_contents = file_content_stream.str();
        }
        catch (const std::ios::failure& e)
        {
            std::cout << "[WARN] Could not find error_page file: " << file_name << '\n';
        }
        catch (...)
        {
            std::cerr << "[ERROR] Error opening file: " << file_name << '\n';
        }
        return (file_contents);
    }

    char            *strdup(const char *str)
    {
        size_t  idx;
        char    *dup;

        dup = new(std::nothrow) char[std::strlen(str) + 1];
        if (!dup)
            return (NULL);
        idx = 0;
        while (str[idx])
        {
            dup[idx] = str[idx];
            idx++;
        }
        dup[idx] = '\0';
        return (dup);
    }

    void            freeCharArr(char ***arr)
    {
        char    **p;

        if (!*arr)
            return ;
        p = *arr;
        for (int idx = 0; p[idx]; ++idx)
            delete [] p[idx];
        delete [] p;
        *arr = NULL;
    }
}
