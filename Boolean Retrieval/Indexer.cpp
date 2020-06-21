#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <bits/stdc++.h> 
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <glob.h>
#include "../stem/stemming/english_stem.h"
#include <fstream>
#include <dirent.h>
#include <sys/types.h>

using namespace std;
using namespace boost;

string stopWords[] = {
    "ourselves", "hers", "between", "yourself", "but", "again", "there", "about", "once", "during", "out", "very", "having",
    "with", "they", "own", "an", "be", "some", "for", "do", "its", "yours", "such", "into", "of", "most", "itself", "other", "off", "is", "s",
    "am", "or", "who", "as", "from", "him", "each", "the", "themselves", "until", "below", "are", "we", "these", "your", "his", "through", "don",
    "nor", "me", "were", "her", "more", "himself", "this", "down", "should", "our", "their", "while", "above", "both", "up", "to", "ours", "had",
    "she", "all", "no", "when", "at", "any", "before", "them", "same", "and", "been", "have", "in", "will", "on", "does", "yourselves", "then", 
    "that", "because", "what", "over", "why", "so", "can", "did", "not", "now", "under", "he", "you", "herself", "has", "just", "where", "too",
    "only", "myself", "which", "those", "i", "after", "few", "whom", "t", "being", "if", "theirs", "my", "against", "a", "by", "doing", "it", 
    "how", "further", "was", "here", "than"
};

int sizeStop = sizeof(stopWords)/sizeof(stopWords[0]);

bool isStopWord(string s){
    for(int i=0;i<sizeStop;i++){
        if(s==stopWords[i])
            return true;
    }
    return false;
}

std::wstring my_to_wstring(const std::string& s){
    std::wstring temp(s.length(),L' ');
    std::copy(s.begin(), s.end(), temp.begin());
    return temp; 
}

std::string my_to_string(const std::wstring& s){
    std::string temp(s.length(), ' ');
    std::copy(s.begin(), s.end(), temp.begin());
    return temp; 
}

int main(){ 
    vector<string> a;
    vector<int> stdVec;
    glob_t glob_result;
    string inputDir = "../testCases/*";
    glob(inputDir.c_str(),GLOB_TILDE,NULL,&glob_result);
    for(unsigned int i=0; i<glob_result.gl_pathc; ++i){
        // cout << glob_result.gl_pathv[i] << endl;
        a.push_back(glob_result.gl_pathv[i]);
        stdVec.push_back(0);
    }
    string outputFileName = "index";
    string stemmed;
    int m = a.size();
    const char * fileName;
    map<string, vector<int>> vocabulary;
    stemming::english_stem<> StemEnglish;
    std::map<string, vector<int>>::iterator it;
    
    for(int i=0;i<m;i++){
        fileName = a[i].c_str();
        ifstream file(fileName);
        // Read file
        if(file.is_open()){
            string line;
            while(getline(file, line)){
                to_lower(line);
                trim(line);
                vector<string> splitVec;
                split(splitVec, line, is_any_of(" .,-:'\"?/_()*!#@;[]{}&|~"), token_compress_on);
                for(int z=0;z<splitVec.size();z++){
                    if(splitVec[z]=="")
                        continue;   
                    if(!isStopWord(splitVec[z])){
                        wstring temp = my_to_wstring(splitVec[z]);
                        StemEnglish(temp);
                        stemmed = my_to_string(temp);
                        it = vocabulary.find(stemmed);
                        if(it != vocabulary.end()){
                            it->second[i] = 1;
                        }
                        else{
                            vocabulary.insert(make_pair(stemmed, stdVec));
                            it = vocabulary.find(stemmed);
                            it->second[i] = 1;
                        }
                    }
                }
            }
            file.close();
        }
    }

    // Write to inverted index file
    ofstream myfile;
    myfile.open(outputFileName.c_str());
    for(int i=0;i<m;i++){
        myfile<<"@"<<a[i]<<" ";
    }
    myfile<<endl;
    for (it = vocabulary.begin(); it != vocabulary.end(); ++it) { 
        string s = it->first;
        trim(s);
        if(s=="")
            continue;
        myfile<<s<<" ";
        for(int i=0;i<m;i++)
            myfile<<it->second[i]<<" ";
        myfile<<endl;
    } 
    cout<<endl<<"Invertex Index Created"<<endl;
    myfile.close();
    return 0;
}

