#ifndef _NAIVEBAYESMODEL_H
#define _NAIVEBAYESMODEL_H

#include <string>
#include <vector>
#include <hash_map>

#include "utils.hpp"

class NaiveBayesModel
{
    typedef __gnu_cxx::hash_map<std::string, std::vector<double>, hash_str, eqstr> HashMap;

    public:
    NaiveBayesModel(int, std::vector<std::string>&, HashMap&);
    void save(std::string);
    static NaiveBayesModel load(std::string);

    std::string predict(std::vector<std::string>&);
    std::string predict(std::string);

    private:

    int numCategories_;
    std::vector<std::string> labelName_;
    HashMap theta_;

};
#endif//_NAIVEBAYESMODEL_H
