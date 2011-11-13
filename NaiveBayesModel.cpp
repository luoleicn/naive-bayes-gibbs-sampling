#include <cfloat>
#include <fstream>
#include <memory>

#include "NaiveBayesModel.hpp"


NaiveBayesModel::NaiveBayesModel(int n, std::vector<std::string>& names, HashMap& theta)
{
    numCategories_ = n;

    for (int i=0; i<names.size(); i++)
        labelName_.push_back(names[i]);

    HashMap::iterator hashIter = theta.begin();
    for (; hashIter!=theta.end(); hashIter++)
    {
        std::string word = hashIter->first;
        std::vector<double> vec = hashIter->second;
        std::vector<double> copyVec;
        for (int i=0; i<vec.size(); i++)
            copyVec.push_back(vec[i]);

        theta_[word] = copyVec;
    }
}

void NaiveBayesModel::save(std::string file)
{
    std::cout << "saving model..." << std::endl;
    std::ofstream out(file, std::ios_base::out|std::ios_base::binary);

    out << numCategories_ << "\n";

    for (std::vector<std::string>::iterator iter=labelName_.begin(); iter!=labelName_.end(); iter++)
        out << *iter << " ";
    out << "\n";

    HashMap::iterator hashIter = theta_.begin();
    for (; hashIter!=theta_.end(); hashIter++)
    {
        out << hashIter->first << " ";
        std::vector<double> dblvec = hashIter->second;
        for (int i=0; i<numCategories_; i++)
            out << dblvec[i] << " ";
        out << "\n";
    }
    out.close();
    std::cout << "Done..." << std::endl;
}

NaiveBayesModel NaiveBayesModel::load(std::string file)
{
    std::cout << "loading model..." << std::endl;
    std::string line;
    int numCategories;
    std::ifstream fin(file);
    getline(fin, line);
    numCategories = atoi(line.c_str());
    
    getline(fin, line);
    std::shared_ptr<std::vector<std::string>> ptr = split(line, "\\s+");
    std::vector<std::string> labelName;
    for (int c=0; c<numCategories; c++)
    {
        labelName.push_back(ptr->at(c));
    }

    __gnu_cxx::hash_map<std::string, std::vector<double>, hash_str, eqstr> theta;
    while (getline(fin, line))
    {
        ptr = split(line, "\\s+");
        std::vector<double> vec(numCategories);
        for (int c=0; c<numCategories; c++)
            vec[c] = atof((ptr->at(c+1)).c_str());
        
        theta[ptr->at(0)] = vec;
    }
    fin.close();
    std::cout << "Done..." << std::endl;
    return NaiveBayesModel(numCategories, labelName, theta);
}

std::string NaiveBayesModel::predict(std::vector<std::string>& words)
{
    std::vector<double> probs(numCategories_, 0.0);
    for (int i=words.size()-1; i>=0; i--)
    {
        if (theta_.find(words[i]) == theta_.end())
            continue;

        std::vector<double> thetaList = theta_[words[i]];
        for (int c=0; c<numCategories_; c++)
            probs[c] += thetaList[c];
    }
    double max = -DBL_MAX;
    int label = -1;
    for (int c=0; c<numCategories_; c++)
    {
        if (probs[c] > max)
        {
            max = probs[c];
            label = c;
        }
    }
    return labelName_[label];
}

std::string NaiveBayesModel::predict(std::string file)
{
    std::ifstream fin(file);
    std::string line;
    std::vector<std::string> v;
    while (getline(fin, line))
    {
        std::shared_ptr<std::vector<std::string> > splits = split(line, "\\s+");
        int sz = splits->size();
        for (int i=0; i<sz; i++)
        {
            v.push_back(splits->at(i));
        }
    }
    fin.close();
    return predict(v);
}
