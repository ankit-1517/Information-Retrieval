#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <bits/stdc++.h> 
#include <locale>
#include <codecvt>
#include <boost/algorithm/string.hpp>
#include "../stem/stemming/english_stem.h"

using namespace std;
using namespace boost;
using convert_t = std::codecvt_utf8<wchar_t>;
std::wstring_convert<convert_t, wchar_t> strconverter;

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
bool queryProcessed = false;

bool isStopWord(string s){
    for(int i=0;i<sizeStop;i++){
        if(s==stopWords[i])
            return true;
    }
    return false;
}

string my_to_string(wstring wstr){
    return strconverter.to_bytes(wstr);
}

wstring my_to_wstring(string str){
    return strconverter.from_bytes(str);
}

void pageRank(){
    int n = 5, nIter = 50;               // n, nIter, a and d are the inputs
    float a[n][n] = {{0, 0, 0, 0, 1},
        {0.5, 0, 0, 0, 0},
        {0.5, 0, 0, 0, 0},
        {0, 1, 0.5, 0, 0},
        {0, 0, 0.5, 1, 0}};
    float d = 0.85, sum = 0;
    float b[n];
    float c[n];
    for(int i=0;i<n;i++){
        b[i] = ((float) rand()+1)/((float) RAND_MAX+1);
        sum += b[i];
    }
    for(int i=0;i<n;i++){
        b[i]/=sum;
    }
    while(nIter--){
        for(int i=0;i<n;i++){
            sum=0;
            for(int j=0;j<n;j++){
                sum+=a[i][j]*b[j];
            }
            c[i]=d*sum+(1-d)/n;
        }
        for(int i=0;i<n;i++)
            b[i]=c[i];
    }
    cout<<"Final scores: ";
    for(int i=0;i<n;i++){
        cout<<b[i]<<" ";
    }
    cout<<endl;
}

vector<int> andQuery(vector<vector<int>> vocab, int wordIndex, vector<int> currentResult){
    queryProcessed = true;
    for(int i=currentResult.size()-1;i>-1;i--){
        if(vocab[wordIndex][currentResult[i]]==0){
            currentResult.erase(currentResult.begin()+i);
        }
    }
    return currentResult;
}

vector<int> notQuery(vector<vector<int>> vocab, int wordIndex, vector<int> currentResult){
    queryProcessed = true;
    for(int i=currentResult.size();i>-1;i--){
        if(vocab[wordIndex][currentResult[i]]==1){
            currentResult.erase(currentResult.begin()+i);
        }
    }
    return currentResult;
}

vector<int> orQuery(vector<vector<int>> vocab, int wordIndex, vector<int> currentResult, int m){
    if(queryProcessed==false){
        currentResult.clear();
        for(int i=0;i<m;i++){
            if(vocab[wordIndex][currentResult[i]]==1){
                currentResult.push_back(i);
            }
        }
    }
    else{
        for(int i=0;i<m;i++){
            if(vocab[wordIndex][i]==1){
                currentResult.push_back(i);
            }
        }
        sort(currentResult.begin(), currentResult.end());
        for(int i=1;i<currentResult.size();i++){
            if(currentResult[i]==currentResult[i-1]){
                currentResult.erase(currentResult.begin()+i);
                i--;
            }
        }
    }   
    queryProcessed = true;
    return currentResult;
}

int main(){ 
    // pageRank();
    const char * fileName;
    stemming::english_stem<> StemEnglish;
    string line;
    vector<string> splitVec;
    vector<string> files;
    map<string, int> wordMap;
    map<string, int>::iterator it;
    vector<vector<int>> vocab;
    
    // reading index in memory
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
                    files.push_back(splitVec[i]);
                    // cout<<splitVec[i]<<endl;
                }
                break;
            }
            while(getline(file, line)) {
                trim(line);
                if(line=="")
                    continue;
                split(splitVec, line, is_any_of(" "), token_compress_on);
                if(splitVec.size()==0)
                    continue;
                wordMap.insert(make_pair(splitVec[0], wordMap.size()));
                vector<int> tempVec;
                for(int z=1;z<splitVec.size();z++){
                    tempVec.push_back(stoi(splitVec[z]));
                }
                vocab.push_back(tempVec);
            }
            file.close();
        }     
        else{
            throw 0;
        }
    } 
    catch(...) { 
        cout<<"Index file not present.\n"; 
        exit(0);
    }
    
    // query processing
    string query, input, stemmed, queryType;
    int wordIndex;
    cout<<"Syntax: [and/or/not] [queryTerm1] [and/or/not] [queryTerm2]..."<<endl;
    do{
        queryProcessed = false;
        vector<int> result;
        for(int i=0;i<files.size();i++){
            result.push_back(i);
        }
        cout<<"Type query: ";
        getline(cin, query);
        to_lower(query);
        trim(query);
        split(splitVec, query, is_any_of(".,-:'\"?/_()*!#@;[]{}<>&"), token_compress_on);
        if(splitVec.size()>1){
            cout<<"Syntax error in input query"<<endl<<"None of "<<".,-:'\"?/_()*!#@;[]{}<>&"<<" should be present in query."<<endl;
            goto nextQuery;
        }
        split(splitVec, query, is_any_of(" "), token_compress_on);
        if(splitVec.size()%2){
            cout<<"Syntax error in input query. Length not even."<<endl;
            goto nextQuery;
        }
        for(int i=0;i<splitVec.size();i++){
            queryType = splitVec[i];
            i++;
            if(!isStopWord(splitVec[i])){
                wstring temp = my_to_wstring(splitVec[i]);
                StemEnglish(temp);
                stemmed = my_to_string(temp);
            }
            else{
                continue;
            }
            it = wordMap.find(stemmed);
            if(it==wordMap.end()){
                if(queryType=="and"){
                    result.clear();
                    break;
                }
                else if(queryType=="or"||queryType=="not")
                    continue;
                else{
                    cout<<"Syntax error in input query. "+queryType+" present instead of and/or/not."<<endl;
                    goto nextQuery;
                }
            }
            else{
                wordIndex = it->second;
            }
            if(queryType=="and"){
                result = andQuery(vocab, wordIndex, result);
            }
            else if(queryType=="or"){
                result = orQuery(vocab, wordIndex, result, files.size());
            }
            else if(queryType=="not"){
                result = notQuery(vocab, wordIndex, result);
            }
            else{
                cout<<"Syntax error in input query. "+queryType+" present instead of and/or/not."<<endl;
                goto nextQuery;
            }
        }
        if(result.size()==0||queryProcessed==false){
            cout<<"No results found!"<<endl;
        }
        else{
            for(int i=0;i<result.size();i++){
                cout<<files[result[i]]<<endl;
            }
            cout<<endl;
        }
        nextQuery: cout<<"Test more queries? If yes, type yes. If no, type anything else.\n";
        getline(cin, input);
    }while(input=="yes");

    return 1;
}
