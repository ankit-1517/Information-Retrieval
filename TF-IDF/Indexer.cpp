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

struct document{
    string id;
    long length;
    map<string, int> tf;
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

double rawTFIDF(long tf, int n, int df_t){
    double rel = log(1+(n*1.0)/df_t);
    rel *= tf;
    return rel;
}

double logNormTFIDF(long tf, int n, int df_t){
    double rel = log(1+(n*1.0)/df_t);
    rel *= (1+log(tf));
    return rel;
}

int main(){ 
    // first is filename, second is doc length
    vector<pair<string, long>> a;
	pair<string, int> tempPairDoc;
	pair<int, int> tempPairWord;
	tempPairDoc.second = 0;
    tempPairWord.first = 1;
    vector<pair<double,int>> stdVec;
    stdVec.push_back(tempPairWord);
    glob_t glob_result;
    string inputDir = "../testCases/*";
    glob(inputDir.c_str(),GLOB_TILDE,NULL,&glob_result);
    for(unsigned int i=0; i<glob_result.gl_pathc; ++i){
        tempPairDoc.first = glob_result.gl_pathv[i];
        a.push_back(tempPairDoc);
    }
    string outputFileName = "index";
    string stemmed;
    int m = a.size();
    // cout<<m<<endl;
    const char * fileName;
    // first is term frequency, second is doc id
    map<string, vector<pair<double,int>>> vocabulary;
    stemming::english_stem<> StemEnglish;
    std::map<string, vector<pair<double,int>>>::iterator it;
    
    for(int i=0;i<m;i++){
        fileName = a[i].first.c_str();
        ifstream file(fileName);
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
                        a[i].second++;
                        wstring temp = my_to_wstring(splitVec[z]);
                        StemEnglish(temp);
                        stemmed = my_to_string(temp);
                        it = vocabulary.find(stemmed);
                        if(it != vocabulary.end()){
                            int fileNo = it->second[it->second.size()-1].second;
                            if(fileNo == i){
                                it->second[it->second.size()-1].first++;
                            }
                            else{
                                it->second.push_back(tempPairWord);
                                it->second[it->second.size()-1].second = i;
                            }
                        }
                        else{
                            vocabulary.insert(make_pair(stemmed, stdVec));
                            it = vocabulary.find(stemmed);
                            it->second[0].second = i ;
                        }
                    }
                }
            }
            file.close();
        }
    }

    for(it = vocabulary.begin(); it != vocabulary.end(); it++){
        for(int i = 0; i<it->second.size(); i++){
            it->second[i].first = logNormTFIDF(it->second[i].first, m, it->second.size());
        }
    }

    ofstream myfile;
    myfile.open(outputFileName.c_str());
    for(int i=0;i<m;i++){
        myfile<<"@"<<a[i].first<<" ";
    }
    myfile<<endl;
    for (it = vocabulary.begin(); it != vocabulary.end(); it++) { 
        string s = it->first;
        trim(s);
        if(s=="")
            continue;
        myfile<<s;
        vector<pair<double, int>> tempVec = it->second;
        for(int i=0;i<tempVec.size();i++)
            myfile<<" "<<tempVec[i].first<<" "<<tempVec[i].second<<" ";
        myfile<<endl;
    } 
    cout<<"Invertex Index Created"<<endl;
    myfile.close();
    return 0;
}

