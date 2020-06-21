#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;
#include <thread>


struct TrieNode{
    char value;
    int freq;
    vector<std::shared_ptr<struct TrieNode>> childrens;
    shared_ptr<struct TrieNode> parent;
};

shared_ptr<struct TrieNode> initializeFirstNode(shared_ptr<struct TrieNode> root, string first_val, int first_freq){
    // PROCESS PREMIERE LIGNE
    // CREATION PREMIER NODE
    shared_ptr<struct TrieNode> old_node = root;
    for (int i = 0; i<first_val.size(); i++){
        shared_ptr<struct TrieNode> node = make_shared<struct TrieNode>();
        node->value = first_val[i];
        //cout << node->value << "\n";
        node->freq = 0;
        node->parent = old_node;
        old_node->childrens.push_back(node);
        old_node = node;
    }
    old_node->freq = first_freq;
    //std::cout << old_node->value << " " << old_node->freq;
    return old_node;
}

void sort(string path) {
    ifstream inFile(path);
    if (!inFile){
        cerr << "Unable to read the file words.txt";
        exit(1);
    }
    vector<string> words;
    string word;
    while (getline(inFile, word)) {
        words.push_back(word);
    }
    sort(words.begin(), words.end());
    
    ofstream outFile("out.txt");
    for (size_t i = 0; i < words.size(); i++) {
        outFile << words[i] << '\n';
    }
    words.clear();
    inFile.close();
    outFile.close();
}

void write_to_file(shared_ptr<struct TrieNode> root, ofstream& outFile){
    int size = root->childrens.size();
    outFile.write(reinterpret_cast<char*>(&root->value), sizeof(char));
    outFile.write(reinterpret_cast<char *>(&root->freq), sizeof(int));
    outFile.write(reinterpret_cast<char *>(&size), sizeof(int));
    for(int i = 0; i < size; i++){
        write_to_file(root->childrens.at(i), outFile);
    }
}

void create_all_nodes(ifstream& inFile,shared_ptr<struct TrieNode> old_node, std::string previous_val){

        // CREATION TOUT LES AUTRES NODES
    int length_word = 0;
    std::string line;
    int nb;
    while (inFile >> line >> nb){
        while (line.size() < previous_val.size()){
            old_node = old_node->parent;
            previous_val = previous_val.substr(0,previous_val.size()-1);
            // cout << "\n"<<line << " " << old_node->value << "\n";
        }
        while (old_node->parent != nullptr && mismatch(previous_val.begin(), previous_val.end(), line.begin()).first != previous_val.end()){
            old_node = old_node->parent;
            previous_val = previous_val.substr(0,previous_val.size()-1);
        }
        // CHECK IF EXIST ALREADY
        // int found = 1;
        // while(found == 1){
        //     found = 0;
        //     for(int k = 0; k < old_node->childrens.size(); k++){
        //         if (mismatch((previous_val + old_node->childrens.at(k)->value).begin(), (previous_val + old_node->childrens.at(k)->value).end(), line.begin()).first == (previous_val + old_node->childrens.at(k)->value).end()){
        //             old_node = old_node->childrens.at(k);
        //             previous_val = previous_val + old_node->value;
        //             found = 1;
        //         }

        //     }
        // }
        // if (line.compare(previous_val) == 0){
        //     old_node-> freq = nb;
        //     continue;
        // }
        length_word = previous_val.size();
        while (length_word < line.size())
        {
            shared_ptr<struct TrieNode> son = make_shared<struct TrieNode>();
            son->parent = old_node;
            son->value = line[length_word];
            son->freq = 0;
            old_node->childrens.push_back(son);
            old_node = son;
            length_word +=1;
        }
        old_node->freq = nb;
        previous_val = line;
    }
}

int main()
{
    sort("words.txt");
    ifstream inFile;
    inFile.open("out.txt");
    if (!inFile){
        cerr << "Unable to read the file out.txt";
        exit(1);
    }
    string line;
    int nb;

    inFile >> line >> nb;
    shared_ptr<struct TrieNode> root = make_shared<struct TrieNode>();
    root->value = '\0';
    root->freq = 0;
    root->parent = nullptr;
    std::string previous_val = line;

    shared_ptr<struct TrieNode> old_node = initializeFirstNode(root, line, nb);
    create_all_nodes(inFile, old_node, previous_val);
    //std::cout << root->childrens.at(0)->value << " freq:" << root->childrens.at(0)->freq;
    inFile.close();
    std::ofstream outFile;
    outFile.open("dict.bin");
    write_to_file(root, outFile);
    outFile.close();
}
