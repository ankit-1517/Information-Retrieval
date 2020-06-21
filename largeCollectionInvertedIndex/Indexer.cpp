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
#include <cstdio>

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

void createUnsortedVocab(string outputFileName, vector<string> a, stemming::english_stem<> StemEnglish){
    ofstream myfile;
    myfile.open(outputFileName.c_str());
    int m = a.size();
    string stemmed;
    const char * fileName;
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
                    wstring temp = my_to_wstring(splitVec[z]);
                    StemEnglish(temp);
                    stemmed = my_to_string(temp);
                    trim(stemmed);
                    if(isStopWord(stemmed)||stemmed==""||stemmed==" ")
                        continue;
                    myfile<<stemmed<<" "<<i<<endl;
                }
            }
            file.close();
        }
        else{cout<<"File "<<a[i]<<" not opened"<<endl;}
    }
    myfile.close();
}

int createSortedRuns(string inputFileName, string sortedRunName, int nFilesSort){
    const char * fileName = inputFileName.c_str();
    ifstream file(fileName);
    // in pair, first is the file no, and second is the no of occurences of the term in that particular file
    map<string,vector<pair<int,int>>> vocab;   
    map<string,vector<pair<int,int>>>::iterator it;   
    pair<int, int> tempPair;
    tempPair.first = 0; tempPair.second = 1;
    vector<pair<int, int>> tempVec;
    tempVec.push_back(tempPair);
    
    // Read file
    int i;
    int j = 0;
    int fileNo;
    vector<string> splitVec;
    bool newFile = true;
    if(file.is_open()){
        string line;
        ofstream sortedRun;
        while(getline(file, line)){
            trim(line);
            split(splitVec, line, is_any_of(" "), token_compress_on);
            if(splitVec.size()<2)
                continue;
            i = stoi(splitVec[1]);
            if(i%nFilesSort==1 && newFile){
                if(sortedRun.is_open()){
                    for(it = vocab.begin(); it!=vocab.end(); it++){
                        sortedRun<<it->first;
                        for(int z = 0; z<it->second.size(); z++){
                            sortedRun<<" "<<it->second[z].first<<" "<<it->second[z].second;
                        }
                        sortedRun<<endl;
                    }
                    sortedRun.close();
                }
                j++;
                vocab.clear();
                sortedRun.open((sortedRunName+to_string(j)).c_str());
                newFile = false;
            }
            else if(i%nFilesSort!=1){newFile=true;}
            it = vocab.find(splitVec[0]);
            if(it != vocab.end()){
                fileNo = it->second[it->second.size()-1].first;
                if(fileNo == i){
                    it->second[it->second.size()-1].second++;
                }
                else{
                    tempPair.first = i;
                    it->second.push_back(tempPair);
                }
            }
            else{
                tempVec[0].first = i;
                vocab.insert(make_pair(splitVec[0], tempVec));
            }
        }
        file.close();
        if(sortedRun.is_open()){
            for(it = vocab.begin(); it!=vocab.end(); it++){
                sortedRun<<it->first;
                for(int z = 0; z<it->second.size(); z++){
                    sortedRun<<" "<<it->second[z].first<<" "<<it->second[z].second;
                }
                sortedRun<<endl;
            }
            sortedRun.close();
        }
    }
    remove(inputFileName.c_str());
    return j;
}

void mergeSortedRuns(string inputFileName, string mergedFileName, int totalSortedRuns){
    const char * fileName;
    vector<ifstream> files(totalSortedRuns);
    string line;
    vector<string> words;
    string word;
    vector<vector<string>> wordIndex;
    vector<string> tempWordIndex;
    for(int i = 1; i<=totalSortedRuns; i++){
        fileName = (inputFileName + to_string(i)).c_str();
        files[i-1].open(fileName);
        getline(files[i-1], line);
        trim(line);
        split(tempWordIndex, line, is_any_of(" "), token_compress_on);
        words.push_back(tempWordIndex[0]);
        tempWordIndex.erase(tempWordIndex.begin());
        wordIndex.push_back(tempWordIndex);
    }
    int minIndex, index;
    ofstream myfile;
    string outputLine;
    myfile.open(mergedFileName.c_str());
    while(files.size()>0){
        vector<int> merge;
        minIndex = 0;
        merge.push_back(0);
        for(int i=1;i<words.size();i++){
            if(words[i]<words[minIndex]){
                merge.clear();
                merge.push_back(i);
                minIndex = i;
            }
            else if(words[i]==words[minIndex]){
                merge.push_back(i);
            }
        }
        outputLine = words[minIndex];
        vector<int> toErase;
        for(int i=0;i<merge.size();i++){
            index = merge[i];
            for(int j=0;j<wordIndex[index].size();j++){
                outputLine += " "+wordIndex[index][j];
            }
            getline(files[index], line);
            trim(line);
            if(line==""){
                //remove
                toErase.push_back(index);
            }
            else{
                split(tempWordIndex, line, is_any_of(" "), token_compress_on);
                words[index] = tempWordIndex[0];
                tempWordIndex.erase(tempWordIndex.begin());
                wordIndex[index] = tempWordIndex;
            }
        }
        for(int i=toErase.size()-1;i>-1;i--){
            index = toErase[i];
            files.erase(files.begin()+index);
            words.erase(words.begin()+index);
            tempWordIndex.erase(tempWordIndex.begin()+index);
            wordIndex.erase(wordIndex.begin()+index);
        }
        myfile<<outputLine<<endl;
    }
    myfile.close();
    for(int i=1;i<=totalSortedRuns;i++){
        fileName = (inputFileName + to_string(i)).c_str();
        remove(fileName);
    }
}

int main(){ 
    time_t start, end; 
    time(&start); 
    vector<string> a;
    glob_t glob_result;
    string inputDir = "testCasesLarge/*";
    glob(inputDir.c_str(),GLOB_TILDE,NULL,&glob_result);
    for(unsigned int i=0; i<glob_result.gl_pathc; ++i){
        a.push_back(glob_result.gl_pathv[i]);
    }
    cout<<a.size()<<endl;
    string outputFileName = "unsorted";
    int m = a.size();
    stemming::english_stem<> StemEnglish;
    
    createUnsortedVocab(outputFileName, a, StemEnglish);
    cout<<"Unsorted Vocabulary created"<<endl;
    
    string sortedRunName = "sortedRun";
    int nFilesSort = 50;
    int totalSortedRuns = createSortedRuns(outputFileName, sortedRunName, nFilesSort);
    cout<<"Sorted Runs created"<<endl;

    string mergedFileName = "finalIndex";
    mergeSortedRuns(sortedRunName, mergedFileName, totalSortedRuns);
    
    cout<<"Megred Runs. Inverted Index created"<<endl;
    time(&end); 
    double time_taken = double(end - start); 
    cout << "Time taken by program is : " << fixed 
         << time_taken << setprecision(5); 
    cout << " sec " << endl; 
    return 0;
}

