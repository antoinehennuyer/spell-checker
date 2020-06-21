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
    vector<struct TrieNode> childrens = {};

};

// shared_ptr<struct TrieNode> initializeFirstNode(shared_ptr<struct TrieNode> root, string first_val, int first_freq){
//     // PROCESS PREMIERE LIGNE
//     // CREATION PREMIER NODE
//     shared_ptr<struct TrieNode> old_node = root;
//     for (int i = 0; i<first_val.size(); i++){
//         // MAKE_SHARED
//         shared_ptr<struct TrieNode> node = make_shared<struct TrieNode>(); // TrieNode()
//         node.value = first_val[i];
//         cout << node.value << "\n";
//         node.freq = 0;
//         old_node.childrens.push_back(node);
//         old_node = node;
//     }
//     old_node.freq = first_freq;
//     std::cout << old_node.value << " " << old_node.freq;
//     return old_node;

// }

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

unsigned int write_node(struct TrieNode root, ofstream& outFile, unsigned int curr_offset){ // seekp(offset) | seekp(0,std::ios::end())
    outFile.seekp(curr_offset);
    int size = root.childrens.size();
    outFile.write((char*)&size, sizeof(int));

    outFile.write((char*)&root.freq, sizeof(int));

    outFile.write((char*)&root.value, sizeof(char));

    unsigned int offset_sons = curr_offset + sizeof(int) + sizeof(int) + sizeof(char);
    unsigned int next_offset = offset_sons + size * sizeof(unsigned int);

    std::vector<unsigned  int> offset_for_nodes(size);

    for (int i = 0; i < root.childrens.size(); i++){
        offset_for_nodes[i] = next_offset;
        next_offset = write_node(root.childrens.at(i), outFile, next_offset);
    }

    outFile.seekp(offset_sons);
    outFile.write((char *)offset_for_nodes.data(), offset_for_nodes.size() * sizeof(unsigned int));
    return next_offset;
}
struct TrieNode add_word(std::string line, int nb, struct TrieNode root, int ptr_line){
    // CHECK SON IF EXIST
    if (ptr_line == line.size()){
        root.freq = nb;
        return root;
    }
    auto it = std::find_if(root.childrens.begin(), root.childrens.end(), [&] (const TrieNode & item){ return item.value == line[ptr_line];});
    int found = -1;
    if (it != root.childrens.end())
    {
        found = distance(root.childrens.begin(), it);
    }
    // int found = -1;
    // for(int i = 0; i< root.childrens.size(); i++){ // find()
    //     if (root.childrens[i].value == line[ptr_line]){
    //         found = i;
    //         break; 
    //     }
    // }
    if (found != -1){
        root.childrens[found] = add_word(line, nb, root.childrens[found], ptr_line+1);
    }
    else{
        struct TrieNode son = TrieNode();
        son.freq = 0;
        son.value = line[ptr_line];
        root.childrens.push_back(add_word(line, nb, son, ptr_line+1));
    }
    return root;
    // add_word(line, nb, root.childrens[i])
}
int main()
{
    //sort("../words.txt");
    ifstream inFile;
    inFile.open("../words.txt");
    if (!inFile){
        cerr << "Unable to read the file out.txt";
        exit(1);
    }
    string line;
    int nb;
    inFile >> line >> nb;
    struct TrieNode root = TrieNode();
    root.value = '\0';
    root.freq = 0;
    int found = 1;
    int ptr = 0;
    //shared_ptr<struct TrieNode> old_node = initializeFirstNode(root, line, nb);
    struct TrieNode old_node;
    // CREATION TOUT LES AUTRES NODES

    while (inFile >> line >> nb){

        root = add_word(line, nb, root, 0);
        // GARDER ANCIEN MOT
        // COMPARE AVEC L'ANCIEN MOT
        // while (line.size() < previous_subval.size()){
        //     old_node = old_node.parent;
        //     previous_subval = previous_subval.substr(0,previous_subval.size()-1);
        //     // cout << "\n"<<line << " " << old_node.value << "\n";
        // }
        // if (line.compare(previous_subval) == 0){
        //     old_node.freq = nb;
        //     continue;
        // }
        // while (old_node.parent != nullptr && mismatch(previous_subval.begin(), previous_subval.end(), line.begin()).first != previous_subval.end()){
        //     old_node = old_node.parent;
        //     previous_subval = previous_subval.substr(0,previous_subval.size()-1);
        // }
        // CHECK IF EXIST ALREADY

        // found = 1;
        // old_node = root;
        // ptr = 0;
        // while(found == 1){
        //     found = 0;
        //     for(int k = 0; k < old_node.childrens.size(); k++){
        //         if ( old_node.childrens[k].value == line[ptr]){
        //             old_node = old_node.childrens.at(k);
        //             found = 1;
        //             ptr +=1;
        //             break;
        //         }

        //     }
        // }
        // CREER NOUVEAU FILS MAIS ATTENTION PAS ECRASER LES AUTRES
        // while (ptr < line.size())
        // {
        //     struct TrieNode son = TrieNode();
        //     son.value = line[ptr];
        //     son.freq = 0;
        //     old_node.childrens.push_back(son);
        //     old_node = son;
        //     ptr +=1;
        // }
        // old_node.freq = nb;

        // TANT QUE LES CARACTERES SONT DIFFERENT OU DE PLUS PETITE TAILLE REMONTE AU PARENT
        // QUAND MEME MOT AJOUTER FREQ
        // QUAND MEME LETTRE ALORS CREER FILS DROIT PUIS DESCENDRE
        // ATTENTION TESTER SI LE FILS EXISTE DEJA
        //cout << old_node.value << " "<< old_node.freq << "\n";
    }
    inFile.close();
    std::ofstream outFile;
    std::cout << root.childrens.at(0).childrens.at(0).value << " freq:" << root.childrens.at(0).childrens.at(0).freq;
    outFile.open("dict.bin", std::ofstream::binary);
    //outFile.write("Salut",4);
    //write_node(root, outFile, 0);
    outFile.close();
    // A ECRIRE BINAIRE
}
