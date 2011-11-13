CPPFLAGS = -g -Wno-deprecated
#CPPFLAGS = -O3 -Wno-deprecated
all:
	g++ NaiveBayesModel.hpp NaiveBayesModel.cpp NaiveBayesGibbs.hpp NaiveBayesGibbs.cpp\
		main.cpp utils.hpp utils.cpp -o naivebayes_gibbs.o -std=c++0x -lboost_regex \
		${CPPFLAGS}

clean:
	rm *.o
