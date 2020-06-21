#include <iostream>
#include <glob.h>
#include <fstream>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;

int main(){
    glob_t glob_result;
    string inputDir = "../testCases/*";
    glob(inputDir.c_str(),GLOB_TILDE,NULL,&glob_result);
    for(unsigned int i=0; i<glob_result.gl_pathc; ++i){
        // cout<<i<<endl;
        string fileName = glob_result.gl_pathv[i];
        string fileName2 = "testCasesLarge/";
        std::ifstream  src(fileName.c_str(), std::ios::binary);
        vector<string> splitVec;
        split(splitVec, fileName, is_any_of("/"), token_compress_on);
        fileName2 += splitVec[2];
        string lines = "";
        ifstream file(fileName);
        if(file.is_open()){
            string line;
            while(getline(file, line))
                lines += line;
            file.close();
        }
        for(int j=1;j<=40;j++){
            string str = fileName2 + to_string(j);
            ofstream myfile;
            myfile.open(str.c_str());
            myfile<<lines;
            myfile.close();
        }
    }
    return 1;
}

