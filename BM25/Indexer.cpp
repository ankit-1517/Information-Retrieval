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

double termWeight(double k_1, double k_3, double b, long tf, long dl, double dl_avg, int n, int df_t){
    double rel = log(1+(n*1.0)/df_t);
    rel *= tf*(1+k_1);
    rel /= ( k_1*((1-b) + b*dl/dl_avg) + tf );
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
                        if(stemmed==""){
                            // cout<<"YES"<<endl;
                            continue;
                        }
                        a[i].second++;
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
    
    double avgLength = 0;
    for(int i=0;i<m;i++){
        avgLength += a[i].second;
    }
    avgLength /= m;

    //calculate weight
    double k_1 = 1.25; 
    double k_3 = 1.25;
    double b = 0.75;
    for(it = vocabulary.begin(); it != vocabulary.end(); it++){
        for(int i = 0; i<it->second.size(); i++){
            long docLength = a[it->second[i].second].second;
            it->second[i].first = termWeight(k_1, k_3, b, it->second[i].first, docLength, avgLength, m, it->second.size());
            // cout<<it->second[i].first;
        }
    }

    // Write to inverted index file
    ofstream myfile;
    myfile.open(outputFileName.c_str());
    for(int i=0;i<m;i++){
        myfile<<"@"<<a[i].first<<" ";
    }
    myfile<<endl;
    for (it = vocabulary.begin(); it != vocabulary.end(); it++) { 
        myfile<<it->first;
        vector<pair<double, int>> tempVec = it->second;
        for(int i=0;i<tempVec.size();i++)
            myfile<<" "<<tempVec[i].first<<" "<<tempVec[i].second<<" ";
        myfile<<endl;
    } 
    cout<<"Invertex Index Created"<<endl;
    myfile.close();
    return 0;
}

