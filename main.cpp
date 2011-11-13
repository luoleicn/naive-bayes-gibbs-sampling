#include <iostream>
#include <stdlib.h>
#include "NaiveBayesGibbs.hpp"

using namespace std;

int main(int args, char** argv)
{
    if (args < 2)
    {
        cout << "./naivebayes_gibbs.o numIter [dirichlet hyperparameter]" << endl;
        exit(-1);
    }
    int numIter = 1000;
    numIter = atoi(argv[1]);
    cout << argv[0] << " " << numIter << endl;

    NaiveBayesGibbs nbg("/home/luolei/gibbs/train", "/home/luolei/gibbs/test", atoi(argv[2]), atoi(argv[3]));
    NaiveBayesModel model = nbg.train(numIter);
    model.save("gibbs.model");
    model = NaiveBayesModel::load("gibbs.model");

    string test = "/home/luolei/gibbs/test/C000013/142.txt.seg";
    cout << "test file " << test << endl;
    cout << "label " << model.predict(test) << endl;
    return 0;
}
