#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "utils.hpp"

using namespace std;

int main(int args, char** argv)
{
    string s = "hello           world\n";
    shared_ptr<vector<string> > ptr = split(s, "\\s+");
    vector<string>::iterator iter = ptr->begin();
    for (; iter!=ptr->end(); iter++)
    {
        cout << "a "  << *iter << endl;
    }
    return 0;
}
