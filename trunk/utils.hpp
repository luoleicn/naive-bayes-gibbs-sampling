#ifndef _UTILS_H
#define _UTILS_H

#include <iostream>
#include <vector>
#include <string>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <memory>
#include <time.h>
#include <ext/hash_map>

class Utils
{
    private: 

        static time_t m_tDate; 
        static std::string m_strCurrentTime; 

    public:
        static std::string getTime();
    
};

struct eqstr
{
    bool operator()(const std::string s1, const std::string s2)const
    {
        return s1 == s2;
    }
};

struct hash_str
{
    size_t operator()(const std::string s)const
    {
        return __gnu_cxx::__stl_hash_string(s.c_str());
    }
};

//遍历文件夹下所有文件
bool getfiles(const std::string, std::vector<std::string>&);
//切分string
std::shared_ptr<std::vector<std::string> > split(std::string s, std::string regex);


#endif//_UTILS_H
