#include "utils.hpp"
#include <string.h>
#include <boost/regex.hpp>

time_t Utils::m_tDate;
std::string Utils::m_strCurrentTime;

bool getfiles(std::string dir, std::vector<std::string>& container)
{
    DIR * pdir;
    struct dirent * entry;

    pdir = opendir(dir.c_str());

    while ((entry = readdir(pdir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0
                || strcmp(entry->d_name, "..") == 0)
            continue;

        if (entry->d_type == DT_DIR)
        {
            if (dir[dir.length()-1] == '/')
                getfiles(dir + entry->d_name, container);
            else
                getfiles(dir + "/" + entry->d_name, container);
        }
        else
        {
            container.push_back(entry->d_name);
        }
    }

    return true;
}

std::shared_ptr<std::vector<std::string> > split(std::string s, std::string regex)
{
    std::shared_ptr<std::vector<std::string> > ptr(new std::vector<std::string>);

    boost::regex reg(regex);
    boost::smatch m;

    std::string::const_iterator it = s.begin();
    std::string::const_iterator end = s.end();

    while(boost::regex_search(it, end, m, reg))
    {
        if (it != m[0].first)
            ptr->push_back(std::string(it, m[0].first));
        it = m[0].second;
    }
    if (it != end)
        ptr->push_back(std::string(it, end));

    return ptr;
}

std::string Utils::getTime()
{
    time(&m_tDate);
    m_strCurrentTime.resize(256);
    strftime((char*)m_strCurrentTime.c_str(),   255,   "%Y/%m/%d %H:%M:%S",   localtime(&m_tDate)); 

    return   m_strCurrentTime; 
}
