#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <bits/stdc++.h>  
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include "../stem/stemming/english_stem.h"
#include <cstdlib>

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

map<int, double> getDocs(vector<string> query, map<string, vector<pair<double,int>>> invIndex){
    std::map<string, vector<pair<double,int>>>::iterator it;
    map<string, int> queryTerms;
    std::map<string, int>::iterator itr;
    for(int i=0;i<query.size();i++){
        itr = queryTerms.find(query[i]);
        if(itr==queryTerms.end()){
            queryTerms.insert(make_pair(query[i], 1));
        }
    }
    map<int, double> relevance;
    std::map<int, double>::iterator itrDocs;
    string term;
    int tf;
    pair<double, int> tempPair;
    for(itr = queryTerms.begin(); itr!=queryTerms.end(); itr++){
        term = itr->first;
        tf = itr->second;
        it = invIndex.find(term);
        if(it==invIndex.end())
            continue;
        vector<pair<double, int>> termIndex = it->second;
        for(int i=0;i<termIndex.size();i++){
            tempPair = termIndex[i];
            itrDocs = relevance.find(tempPair.second);
            if(itrDocs!=relevance.end())
                itrDocs->second += tf*tempPair.first;
            else
                relevance.insert(make_pair(tempPair.second, tf*tempPair.first));
        }
    }
    return relevance;
} 

int main(){ 
    const char * fileName;
    stemming::english_stem<> StemEnglish;
    string line;
    //documents
    vector<string> a;
    map<string, vector<pair<double,int>>> invIndex;
    std::map<string, vector<pair<double,int>>>::iterator it;
    pair<double,int> tempPair;
    vector<string> splitVec;
    
    try { 
        ifstream file("index");
        if(file.is_open()){
            while(getline(file,line)){
                trim(line);
                split(splitVec, line, is_any_of("@"), token_compress_on);
                for(int i=1;i<splitVec.size();i++){
                    trim(splitVec[i]);
                    if(splitVec[i]=="")
                        continue;
                    a.push_back(splitVec[i]);
                }
                break;
            }
            // cout<<a.size()<<endl;
            while(getline(file, line)) {
                trim(line);
                split(splitVec, line, is_any_of(" "), token_compress_on);
                if(splitVec.size()==0)
                    continue;
                if(splitVec[0]=="")
                    continue;
                vector<pair<double,int>> tempVec;
                // cout<<splitVec.size()<<" "<<line<<endl;
                for(int i=1;i<splitVec.size();i+=2){
                    tempPair.first = stod(splitVec[i]);
                    tempPair.second = stod(splitVec[i+1]);
                    tempVec.push_back(tempPair);
                }
                invIndex.insert(make_pair(splitVec[0], tempVec));
            }
            file.close();
        }  
        else{
            throw 1;
        }   
    } 
    catch(...) { 
        cout<<"Index file not present.\n"; 
        exit(0);
    }

    // query processing
    string query, input, stemmed;
    do{
        cout<<"Type query: ";
        getline(cin, query);
        to_lower(query);
        trim(query);
        split(splitVec, query, is_any_of(" .,-:'\"?/_()*!#@;[]{}<>&"), token_compress_on);
        vector<string> splitQuery;
        for(int i=0;i<splitVec.size();i++){
            if(splitVec[i]=="")
                continue;
            if(!isStopWord(splitVec[i])){
                wstring temp = my_to_wstring(splitVec[i]);
                StemEnglish(temp);
                stemmed = my_to_string(temp);
                splitQuery.push_back(stemmed);
            }
        }
        map<int, double> result = getDocs(splitQuery, invIndex);
        priority_queue< pair<double, int>> ans;
        int ansSize = 0;
        std::map<int, double>::iterator itResult;
        if(result.size()==0){
            cout<<"No results found!\n";
            goto nextQuery;
        }
        for(itResult = result.begin(); itResult!=result.end(); itResult++){
            ans.push(make_pair(itResult->second, itResult->first));
            ansSize++;   
        }
        for(int i=0;i<min(50,ansSize);i++) { 
            tempPair = ans.top();
            cout<<a[tempPair.second]<<" "<<tempPair.first<<endl;
            ans.pop();
        }
        nextQuery: cout<<"Test more queries? If yes, type yes. If no, type anything else.\n";
        getline(cin, input);
    }while(input=="yes");

    return 0;
}

