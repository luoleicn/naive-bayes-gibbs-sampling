#ifndef _NAIVEBAYESGIBBS_H
#define _NAIVEBAYESGIBBS_H

#include <iostream>
#include <vector>
#include <hash_map>
#include <string>
#include <memory>

#include "NaiveBayesModel.hpp"
#include "utils.hpp"

class NaiveBayesGibbs
{
    typedef __gnu_cxx::hash_map<std::string, std::vector<double>, hash_str, eqstr> HashMap;

    public:
    NaiveBayesGibbs(std::string trainDir, std::string testDir, int param=70, int interval=7);

    NaiveBayesModel train(int iter=1000);

    private:
    int numCategories_;//共多少个类别
    HashMap thetaCur_;//词服从的分布
    HashMap thetaHistory_;//历史
    __gnu_cxx::hash_map<std::string, std::vector<int>, hash_str, eqstr> wordCount_;//字-》每个类别下当前字的个数
    std::vector<int> labelVec_;//保存训练过程中标签的结果
    std::vector<int> categoryFiles_;//每个类别下的文档数
    std::vector<int> labelHistory_;//保存训练过程中标签的结果
    std::vector<std::string> labelNames_;
    std::vector<int> trainAns_;//保存文档标签
    std::vector<int> testAns_;//保存文档标签
    std::vector<std::string> trainFiles_;//所有文件
    std::vector<std::string> testFiles_;//所有文件
    std::vector<std::vector<std::string> > trainWords_;//第一个维度是文档，第二个维度是文档中的词
    std::vector<std::vector<std::string> > testWords_;//第一个维度是文档，第二个维度是文档中的词
    double logLabelNorm;

    const int DIRICHLET_HYPERPARAMETER;
    const int INTERVAL;

    int stasticsFiles(const std::string&, bool);
    void updateTheta(int);
    void stasticsWords();
    void initLabels();
    double check();
    double checkMLE();
    int predict(int);
    int predictMLE(int);
};
#endif//_NAIVEBAYESGIBBS_H
