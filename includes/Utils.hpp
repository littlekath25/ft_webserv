#ifndef UTILS_HPP

# define UTILS_HPP
# define FAIL_CONVERSION (-1)

# include <Webserv.hpp>

namespace ft
{
	std::string		intToString(int num);
	int             stringToInt(const std::string& str);
	std::string     readFileContent(std::ifstream& file_stream, const std::string& file_name);
	void            freeCharArr(char ***arr);

    char            *strdup(const char *str);

    template< class InputIt, class T >
    size_t  count( InputIt first, InputIt last, const T& value )
    {
        size_t  counter;

        counter = 0;
        while (first != last)
        {
            if (*first == value)
                counter++;
            first++;
        }
        return (counter);
    }
}

#endif
