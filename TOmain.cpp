#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

#include "Automaat_parser.h"


void replaceExtension(string& s, const string& newExt);


int main(int argc,  char const* argv[]) {

    cout<<argv[1]<<endl;
    Automaton::Automaat iets(argv[1]);
    string file = argv[1];

    replaceExtension(file, "1.dot");
//    cout<<file<<endl;
    iets.setOutputfilename(file);
    iets.To_Dotlanguage();

    stringstream ss;
    iets.DFAToRegex(ss);
    cout<<ss.str();

    replaceExtension(file, "1.dot");
    iets.setOutputfilename(file);
    iets.To_Dotlanguage();


    return 0;
}

void replaceExtension(string& s, const string& newExt) {
    /**
     * @brief this function will delete the exsiting extension and add the new extension (it will also delete the point so you need to add the point in front of the new extension)
     */
    string newStr;
    for(auto c:s){
        if(c == '.')
            //TODO: what if previous file has no extension? exeption please
            break;
        newStr.push_back(c);
    }
    newStr.append(newExt);
    s = newStr;
}