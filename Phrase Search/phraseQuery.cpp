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
using convert_t = std::codecvt_utf8<wchar_t>;
std::wstring_convert<convert_t, wchar_t> strconverter;

class avlNode{
    public:
        int page;
        int wordIndex;
        int height;
        avlNode* left;
        avlNode* right;

    avlNode(int pageN, int wordIndexN){
        page = pageN;
        wordIndex = wordIndexN;
        height = 0;
        left = NULL;
        right = NULL;
    }
};

class avlTree{
    public:
        avlNode* root = NULL;

    avlNode* getRoot(){
        return root;
    }

    int height(avlNode* node){ 
        if(node == NULL) 
            return 0; 
		return node->height; 
    }

    int max(int a,int b) { 
        if(a>b)
			return a;
		return b;
    }
    
    void insert(int page, int wordIndex){
		avlNode* newNode = new avlNode(page, wordIndex); 
		if (root == NULL)
			root = newNode;
		else
			root = avlInsert(newNode, root);
	} 
	
	avlNode* rotatewithleft(avlNode* x){
		avlNode* leftChild;
		leftChild = x->left;
		x->left = leftChild->right;
		leftChild->right = x;
		x->height = max(height(x->left),height(x->right))+1;
		leftChild->height = max(height(leftChild->left),height(leftChild->right))+1;
		return leftChild;
	}

    avlNode* rotatewithright(avlNode* x){
		avlNode* rightChild;
		rightChild = x->right;
		x->right = rightChild->left;
		rightChild->left = x;
		x->height = max(height(x->left),height(x->right))+1;
		rightChild->height = max(height(rightChild->left),height(rightChild->right))+1;
		return rightChild;
	}

	avlNode* doublerotatewithleft(avlNode* x){
		avlNode* temp;
		x->left = rotatewithright(x->left);
		temp = rotatewithleft(x);
		return temp;
	}

	avlNode* doublerotatewithright(avlNode* x){
		avlNode* temp;
		x->right = rotatewithleft(x->right);
		temp = rotatewithright(x);
		return temp;
	}

    avlNode* avlInsert(avlNode* newNode, avlNode* root){
		avlNode* newRoot = root;  // root of subtree in which i'm inserting
		// bool isBalanced = true;
		if(newNode->wordIndex < root->wordIndex){//branch left
			if (root->left == NULL)
				newRoot->left = newNode;     
			else{
				newRoot->left = avlInsert(newNode, root->left);  
				if((height(newRoot->left)-height(newRoot->right)) == 2){
					if(newNode->wordIndex < newRoot->wordIndex)
						newRoot = rotatewithleft(newRoot);
					else
						newRoot = doublerotatewithleft(newRoot);
				}
			}
		}
		else if(newNode->wordIndex > root->wordIndex){//branch right
			if (root->right == NULL)
				newRoot->right = newNode;   
			else{
				newRoot->right = avlInsert(newNode,root->right);  
				if ((height(newRoot->right)-height(newRoot->left)) == 2){
					if(newNode->wordIndex > newRoot->right->wordIndex)
						newRoot = rotatewithright(newRoot);
					else 
						newRoot = doublerotatewithright(newRoot);
				}
			}
		}
		//this will be when the wordIndex is same but page is different
		//i'll put them according to pageName in that case
		else{
			if(newNode->page < root->page){//branch left
				if (root->left == NULL)
					newRoot->left = newNode;
				else{
					newRoot->left = avlInsert(newNode, root->left);  
					if((height(newRoot->left)-height(newRoot->right)) == 2){
						if(newNode->wordIndex < newRoot->left->wordIndex)
							newRoot = rotatewithleft(newRoot);
						else
							newRoot=doublerotatewithleft(newRoot);
					}
				}
			}
			else if(newNode->page > root->page){//branch right
				if (root->right == NULL)
					newRoot->right = newNode;   
				else{
					newRoot->right = avlInsert(newNode, root->right);  
					if ((height(newRoot->right)-height(newRoot->left)) == 2){
						if(newNode->wordIndex > newRoot->left->wordIndex)
							newRoot = rotatewithright(root);
						else 
							newRoot = doublerotatewithright(root);
					}
				}
			}
		}

		// Update height
		newRoot->height = max(height(newRoot->left),height(newRoot->right))+1;

		return newRoot; // return new root of this subtree
	}

    bool isPresent(int page, int wordIndex, avlNode* root){
		if(root == NULL)
			return false;
		if(root->wordIndex < wordIndex)
			return isPresent(page, wordIndex, root->right);
		if(root->wordIndex > wordIndex)
			return isPresent(page, wordIndex, root->left);
		if(root->wordIndex == wordIndex){
			if(root->page == page)
				return true;
			if(root->page > page)
				return isPresent(page, wordIndex, root->left);
			if(root->page < page)
				return isPresent(page, wordIndex, root->right);
		}
		return false;
	}
	
	void getWordIndex(avlNode* root, int page, vector<int> newList){
		if(root == NULL)
			return;
		getWordIndex(root->left, page, newList);
		if(root->page == page)
			newList.push_back(root->wordIndex);
		getWordIndex(root->right, page, newList);
	}
	
	vector<int> getWordIndex(int page){
		vector<int> newList;
		getWordIndex(root, page, newList);
		return newList;
	}
	
	void print(avlNode* root){
		if(root == NULL)
			return;
		print(root->left);
		cout<<root->wordIndex<<" "<<root->page<<" "<<root->height<<endl;
		print(root->right);
	}

};

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

string my_to_string(wstring wstr){
    return strconverter.to_bytes(wstr);
}

wstring my_to_wstring(string str){
    return strconverter.from_bytes(str);
}

map<int, int> getDocs(vector<string> query, map<string,vector<pair<vector<int>,int>>> invIndex){
    vector<int> docs;
	vector<int> docsTemp;
	string term;
	map<int, int> relevance;
    bool found = false;
	vector<pair<vector<int>,int>> tempVec;
    map<string, vector<pair<vector<int>,int>>>::iterator it;
    
	for(int i=0;i<query.size();i++){
		it = invIndex.find(query[i]);
        if(it==invIndex.end()){
			return relevance;
		}
		if(found){
			tempVec = it->second;
			docsTemp.clear();
			for(int j=0;j<tempVec.size();j++){
				if(std::find(docs.begin(), docs.end(), tempVec[j].second) != docs.end()){
					docsTemp.push_back(tempVec[j].second);
				}
			}
			docs = docsTemp;
		}
		else{
			tempVec = it->second;
			for(int j=0;j<tempVec.size();j++){
				docs.push_back(tempVec[j].second);
			}
		}
		found = true;
	}
	if(docs.size()==0){
		return relevance;
	}

	vector<vector<int>> termPos;
	vector<vector<vector<int>>> pos;
	for(int i=0;i<docs.size();i++){
		termPos.clear();
		for(int j=0;j<query.size();j++){
			it = invIndex.find(query[j]);
			for(int k=0;k<it->second.size();k++){
				if(it->second[k].second==docs[i]){
					termPos.push_back(it->second[k].first);
					break;
				}
			}
		}
		pos.push_back(termPos);
	}
	
	int count;
	for(int i=0;i<pos.size();i++){
		count = 0;
		for(int j=0;j<pos[i][0].size();){
			for(int k=1;k<pos[i].size();k++){
				if(std::find(pos[i][k].begin(), pos[i][k].end(), pos[i][0][j]+k)==pos[i][k].end())
					goto nextPos;
			}
			count++;
			nextPos: j++;
		}
		if(count>0){
			relevance.insert(make_pair(docs[i], count));
		}
	}
	
	return relevance;
} 

int main(){ 
    vector<string> a;
    vector<string> fileMap;
    vector<int> stdVec;
    glob_t glob_result;
    string inputDir = "../testCases/*";
    glob(inputDir.c_str(),GLOB_TILDE,NULL,&glob_result);
    for(unsigned int i=0; i<glob_result.gl_pathc; ++i){
        a.push_back(glob_result.gl_pathv[i]);
        fileMap.push_back(glob_result.gl_pathv[i]);
    }
    string outputFileName = "index";
    string stemmed;
    int m = a.size();
    const char * fileName;
    map<string, vector<pair<vector<int>,int>>> invIndex;
    map<string, vector<pair<vector<int>,int>>>::iterator it;
    stemming::english_stem<> StemEnglish;
    vector<int> tempVec1;
	tempVec1.push_back(0);
	//first is occurences, second is file number
	pair<vector<int>,int> tempPair;
	tempPair.first = tempVec1;
	tempPair.second = 0;
	vector<pair<vector<int>,int>> tempVec2;
	tempVec2.push_back(tempPair);
	long count;
	vector<string> splitVec;
    
	cout<<"Creating inverted index...\n";
    for(int i=0;i<m;i++){
        fileName = a[i].c_str();
        ifstream file(fileName);
        // Read file
        if(file.is_open()){
			count = 0;
            string line;
            while(getline(file, line)){
                to_lower(line);
                trim(line);
                split(splitVec, line, is_any_of(" .,-:'\"?/_()*!#@;[]{}&|~"), token_compress_on);
                for(int z=0;z<splitVec.size();z++){
                    if(splitVec[z]=="")
                        continue;   
                    if(!isStopWord(splitVec[z])){
						wstring temp = my_to_wstring(splitVec[z]);
                        StemEnglish(temp);
                        stemmed = my_to_string(temp);
						if(stemmed==""||stemmed==" "||isStopWord(stemmed))
							continue;
                        count++;
                        it = invIndex.find(stemmed);
                        if(it != invIndex.end()){
                            if(it->second[it->second.size()-1].second==i){
								it->second[it->second.size()-1].first.push_back(count);
							}
							else{
								tempVec1[0] = count;
								tempPair.first = tempVec1;
								tempPair.second = i;
								it->second.push_back(tempPair);
							}
                        }
                        else{
							tempVec1[0] = count;
							tempPair.first = tempVec1;
							tempPair.second = i;
							tempVec2[0] = tempPair;
                            invIndex.insert(make_pair(stemmed, tempVec2));
                            it = invIndex.find(stemmed);
                        }
                    }
                }
            }
            file.close();
        }
    }
    
    // query processing
    string query, input;
    do{
        cout<<"Type query: ";
        getline(cin, query);
        to_lower(query);
        trim(query);
		pair<int,int> tempPair;
        split(splitVec, query, is_any_of(" .,-:'\"?/_()*!#@;[]{}<>&"), token_compress_on);
        vector<string> splitQuery;
        for(int i=0;i<splitVec.size();i++){
            if(splitVec[i]=="")
                continue;
			wstring temp = my_to_wstring(splitVec[i]);
			StemEnglish(temp);
			stemmed = my_to_string(temp);
            if(isStopWord(splitVec[i])||stemmed==""||stemmed==" ")
				continue;
			splitQuery.push_back(stemmed);
        }
        map<int, int> result = getDocs(splitQuery, invIndex);
        priority_queue< pair<int, int>> ans;
        int ansSize = 0;
        std::map<int, int>::iterator itResult;
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

}
