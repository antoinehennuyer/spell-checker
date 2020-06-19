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
        // MAKE_SHARED
        shared_ptr<struct TrieNode> node = make_shared<struct TrieNode>();
        node->value = first_val[i];
        cout << node->value << "\n";
        node->freq = 0;
        node->parent = old_node;
        old_node->childrens.push_back(node);
        old_node = node;
    }
    old_node->freq = first_freq;
    //std::cout << old_node->value << " " << old_node->freq;
    return old_node;
}

vector<string> browse(string path, string word, shared_ptr<struct TrieNode> tree, int dist, int nb_error, int i, vector<string> words) {
    if (nb_error > dist){
        vector<string> empty(0);
        return empty;
    }
    if (word[i] == '\0') {
        words.push_back(path + '\0');
        printf("Word : %s \n", path.c_str());
        return words;
    }
    else {
        for (unsigned j = 0; j < tree->childrens.size(); j++) {
            string new_path = path;
            new_path.push_back(tree->childrens[j]->value);
            if (tree->childrens[j]->value == word[i]) {
                words = browse(new_path, word, tree->childrens[j], dist, nb_error, i + 1, words);
            } else {
                words = browse(new_path, word, tree->childrens[j], dist, nb_error + 1, i + 1, words);
            }
        }
        return words;
    }
}

void test_dist(shared_ptr<struct TrieNode> root, string word){
    printf("\n%s\n", "début");
    
    printf("\n%s\n", "dist 0 :");
    vector<string> words(1000);
    browse("", word, root, 0, 0, 0, words);
    printf("\n%s\n", "dist 1 :");
    vector<string> words2(1000);
    browse("", word, root, 1, 0, 0, words2);
    printf("\n%s\n", "dist 2 :");
    vector<string> words3(1000);
    browse("", word, root, 2, 0, 0, words3);
    
    printf("\n%s\n", "fin");
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
unsigned int write_node(shared_ptr<struct TrieNode> root, ofstream& outFile, unsigned int curr_offset){ // seekp(offset) | seekp(0,std::ios::end())
    outFile.seekp(curr_offset);
    int size = root->childrens.size();
    outFile.write((char*)&size, sizeof(int));

    outFile.write((char*)&root->freq, sizeof(int));

    outFile.write((char*)&root->value, sizeof(char));

    unsigned int offset_sons = curr_offset + sizeof(int) + sizeof(int) + sizeof(char);
    unsigned int next_offset = offset_sons + size * sizeof(unsigned int);

    std::vector<unsigned  int> offset_for_nodes(size);

    for (int i = 0; i < root->childrens.size(); i++){
        offset_for_nodes[i] = next_offset;
        next_offset = write_node(root->childrens.at(i), outFile, next_offset);
    }

    outFile.seekp(offset_sons);
    outFile.write((char *)offset_for_nodes.data(), offset_for_nodes.size() * sizeof(unsigned int));
    return next_offset;
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

    // CREATION TOUT LES AUTRES NODES
    int length_word = 0;
    while (inFile >> line >> nb){
        // GARDER ANCIEN MOT
        // COMPARE AVEC L'ANCIEN MOT
        while (line.size() < previous_val.size()){
            old_node = old_node->parent;
            previous_val = previous_val.substr(0,previous_val.size()-1);
            // cout << "\n"<<line << " " << old_node->value << "\n";
        }
        // if (line.compare(previous_val) == 0){
        //     old_node->freq = nb;
        //     continue;
        // }
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
        // CREER NOUVEAU FILS MAIS ATTENTION PAS ECRASER LES AUTRES
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

        // TANT QUE LES CARACTERES SONT DIFFERENT OU DE PLUS PETITE TAILLE REMONTE AU PARENT
        // QUAND MEME MOT AJOUTER FREQ
        // QUAND MEME LETTRE ALORS CREER FILS DROIT PUIS DESCENDRE
        // ATTENTION TESTER SI LE FILS EXISTE DEJA
        //cout << old_node->value << " "<< old_node->freq << "\n";
    }
    inFile.close();
    std::ofstream outFile;
    outFile.open("dict.bin");
    //test_dist(root, "n936");
    //write_node(root, outFile, 0);
    write_to_file(root, outFile);
    outFile.close();
    // A ECRIRE BINAIRE
}
