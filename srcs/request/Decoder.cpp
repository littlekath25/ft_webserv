#include <Request.hpp>

void    Request::decodeRequest()
{
    if (!m_body.empty())
    {
        std::string new_body;
        int chunked_size;

        std::istringstream(m_body.substr(0, m_body.find('\r'))) >> std::hex >> chunked_size;
        while(chunked_size > 0)
        {
            m_body.erase(0, m_body.find_first_of(LF) + 1);
            new_body.append(m_body.substr(0, chunked_size));
            m_body.erase(0, chunked_size + 1);
            m_body.erase(0, m_body.find_first_of(LF) + 1);
            std::istringstream(m_body.substr(0, m_body.find(CR))) >> std::hex >> chunked_size;
        }
        m_body.clear();
        m_body = new_body;
    }
}
