#include <time.h>
#include <stdlib.h>
#include <fstream>
#include <dirent.h> 
#include <unistd.h>
#include <sys/types.h>
#include <random>
#include <set>
#include <cmath>
#include <string.h>
#include <cfloat>

#include "utils.hpp"
#include "NaiveBayesGibbs.hpp"


inline double logsumexp(double x, double y, bool flg) {
  if (flg) return y;  // init mode
  double vmin = x;
  double vmax = y;
  if (x > y)
  {
      vmax = x;
      vmin = y;
  }
  if (vmax > vmin + 50) {
    return vmax;
  } else {
    return vmax + std::log(std::exp(vmin - vmax) + 1.0);
  }
}

NaiveBayesGibbs::NaiveBayesGibbs(std::string trainDir, std::string testDir, int parameter, int interval):
    DIRICHLET_HYPERPARAMETER(parameter), INTERVAL(interval)
{
    //get all train files
    std::cout << "loading all files..." << std::endl;
    numCategories_ = stasticsFiles(trainDir, true);
    for (int c=0; c<numCategories_; c++)
        categoryFiles_.push_back(0);
    stasticsFiles(testDir, false);

    std::cout << "load "<< trainFiles_.size() << " files for train..." << std::endl;
    std::cout << "load "<< testFiles_.size() << " files for test..." << std::endl;
    std::cout << "num categories : " << numCategories_ << std::endl;

    //init sampling labels;
    std::cout << "init sampling labels..." << std::endl;
    initLabels();

    //init sampling theta
    std::cout << "init sampling theta..." << std::endl;
    stasticsWords();
    updateTheta(0);

    const int numTrainFiles = trainFiles_.size();
    const double Z = (numTrainFiles + numCategories_*DIRICHLET_HYPERPARAMETER -1);
    logLabelNorm = log(Z);
}

void NaiveBayesGibbs::initLabels()
{
    int numTrainFiles = trainFiles_.size();
    for (int i=0; i<numTrainFiles; i++)
    {
        int label = trainAns_[i];
        labelHistory_[i] = labelVec_[i] = label;
        categoryFiles_[label]++;
    }
}

void NaiveBayesGibbs::stasticsWords()
{
    //stastics words
    std::cout << "stastics words... " << std::endl;
    std::set<std::string> uniqWords;
    for (std::vector<std::string>::iterator iter = trainFiles_.begin(); iter != trainFiles_.end(); iter++)
    {
        std::ifstream fin(*iter);
        std::string line;
        std::vector<std::string> v;
        while (getline(fin, line))
        {
            std::shared_ptr<std::vector<std::string> > splits = split(line, "\\s+");
            int sz = splits->size();
            for (int i=0; i<sz; i++)
            {
                uniqWords.insert(splits->at(i));
                v.push_back(splits->at(i));
            }
        }
        fin.close();
        trainWords_.push_back(v);
    }
    for (std::set<std::string>::iterator iter=uniqWords.begin(); iter!=uniqWords.end(); iter++)
    {
        thetaCur_[*iter] = std::vector<double>(numCategories_, 0.0);
        thetaHistory_[*iter] = std::vector<double>(numCategories_, 0);
        wordCount_[*iter] = std::vector<int>(numCategories_, 0);
    }
    std::cout << "train words number " << uniqWords.size() << std::endl;

    uniqWords.clear();
    for (std::vector<std::string>::iterator iter = testFiles_.begin(); iter != testFiles_.end(); iter++)
    {
        std::ifstream fin(*iter);
        std::string line;
        std::vector<std::string> v;
        while (getline(fin, line))
        {
            std::shared_ptr<std::vector<std::string> > splits = split(line, "\\s+");
            int sz = splits->size();
            for (int i=0; i<sz; i++)
            {
                uniqWords.insert(splits->at(i));
                v.push_back(splits->at(i));
            }
        }
        fin.close();
        testWords_.push_back(v);
    }
    std::cout << "test words number " << uniqWords.size() << std::endl;


    std::cout << "stastics the number of  given words in each category" << std::endl;
    for (int f=0; f<trainFiles_.size(); f++)
    {
        int label = labelVec_[f];
        std::vector<std::string> words = trainWords_[f];
        for (std::vector<std::string>::iterator iter=words.begin(); iter!=words.end(); iter++)
            wordCount_[*iter][label]++;
    }
}

NaiveBayesModel NaiveBayesGibbs::train(int numIter)
{
    double accuracy2 = checkMLE();
    std::cout << "start training..." << std::endl;
    for (int t=1; t<=numIter; t++)//number iteration
    {
        std::cout << Utils::getTime() << " update theta" << std::endl;
        updateTheta(t);
        double accuracy = check();
        std::cout << Utils::getTime() << " Iteratation num "  << t << " Accuracy " << accuracy << " maximum likelihood " << accuracy2 << std::endl;
        std::cout << "dirichlet hyperparameter : " << DIRICHLET_HYPERPARAMETER << std::endl;
    }
    return NaiveBayesModel(numCategories_, labelNames_, thetaHistory_);
}


void NaiveBayesGibbs::updateTheta(int round)
{
    std::mt19937 eng(int(time(0)));
    //#pragma omp parallel for
    for (int c=0; c<numCategories_; c++)
    {
        std::vector<double> y;
        double sum = 0.0;

        HashMap::iterator iter = thetaCur_.begin();
        for (; iter!=thetaCur_.end(); iter++)
        {
            std::gamma_distribution<double> gamma(wordCount_[iter->first][c] + DIRICHLET_HYPERPARAMETER);
            double tmp = log(1.0 * gamma(eng));
            y.push_back(tmp);
            sum = logsumexp(sum, tmp, iter==thetaCur_.begin());
        }

        int num=0;
        for (iter=thetaCur_.begin(); iter!=thetaCur_.end(); iter++)
        {
            double value = y[num]-sum;
            iter->second[c] = value;
//            thetaHistory_[iter->first][c] += value;
            double tmp = thetaHistory_[iter->first][c];
            if (round % INTERVAL == 0)
                thetaHistory_[iter->first][c] = logsumexp(tmp, value, tmp==0);
            num++;
        }
    }
}

int NaiveBayesGibbs::stasticsFiles(const std::string& dir, bool isTrainDir)
{
    int label = 0;
    DIR * pdir;
    struct dirent * entry;

    pdir = opendir(dir.c_str());

    while ((entry = readdir(pdir)) != NULL)
    {   
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        if (entry->d_type == DT_DIR)
        {
            int ans = label;
            if (isTrainDir)
                labelNames_.push_back(entry->d_name);
            else
            {
                for (int i=labelNames_.size()-1; i>=0; i--)
                {
                    if (labelNames_[i] == entry->d_name)
                    {
                        ans = i;
                        break;
                    }
                }
            }
            
            std::string folder;
            if (dir[dir.length()-1] == '/')
                folder = dir + entry->d_name;
            else
                folder = dir + "/" + entry->d_name;

            DIR* subPdir;
            struct dirent * subEntry;
            subPdir = opendir(folder.c_str());
            while ((subEntry = readdir(subPdir)) != NULL)
            {
                if (strcmp(subEntry->d_name, ".") == 0 || strcmp(subEntry->d_name, "..") == 0)
                    continue;
                if (subEntry->d_type == DT_DIR)
                {
                    std::cout << "文件组织形式有误，这里不应该出现文件夹 : " << folder+"/"+subEntry->d_name << std::endl;
                }
                else
                {
                    if (isTrainDir)
                    {
                        trainFiles_.push_back(folder + "/" + subEntry->d_name);
                        labelHistory_.push_back(-1);
                        labelVec_.push_back(-1);
                        trainAns_.push_back(ans);
                    }
                    else
                    {
                        testFiles_.push_back(folder + "/" + subEntry->d_name);
                        testAns_.push_back(ans);
                    }
                }
            }
            label++;
        }
    }
    return label;
}

double NaiveBayesGibbs::check()
{
    int correct = 0;
    int numFiles = testFiles_.size();
    for (int i=0; i<numFiles; i++)
    {
        int label = predict(i);
        if (label == testAns_[i])
            correct++;
    }
    return 1.0*correct/numFiles;
}

double NaiveBayesGibbs::checkMLE()
{
    int correct = 0;
    int numFiles = testFiles_.size();
    for (int i=0; i<numFiles; i++)
    {
        int label = predictMLE(i);
        if (label == testAns_[i])
            correct++;
    }
    return 1.0*correct/numFiles;
}


int NaiveBayesGibbs::predict(int fid)
{
    std::vector<std::string> words = testWords_[fid];
    std::vector<double> probs(numCategories_, 0.0);
    for (int i=words.size()-1; i>=0; i--)
    {
        if (thetaHistory_.find(words[i]) == thetaHistory_.end())
            continue;

        std::vector<double> thetaList = thetaHistory_[words[i]];
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
    return label;
}

int NaiveBayesGibbs::predictMLE(int fid)
{
    std::vector<std::string> words = testWords_[fid];
    std::vector<double> probs(numCategories_, 0.0);
    for (int i=words.size()-1; i>=0; i--)
    {
        if (wordCount_.find(words[i]) == wordCount_.end())
            continue;

        std::vector<int> counter = wordCount_[words[i]];
        for (int c=0; c<numCategories_; c++)
            probs[c] += log(counter[c]+0.01);
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
    return label;
}

