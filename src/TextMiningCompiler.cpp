#include <iostream>
#include <fstream>

struct TrieNode{
    std::string value;
    int freq;
    struct TrieNode *childrens[50];
    struct TrieNode *parent;
};

int main()
{
    std::ifstream inFile;
    inFile.open("../../out.txt");
    if (!inFile){
        std::cerr << "Unable to read the file words.txt";
        exit(1);
    }
    std::string line;
    int nb;
    // PROCESS PREMIERE LIGNE
    std::string first_val;
    int first_freq;
    inFile >> first_val >> first_freq;

    // CREATION PREMIER NODE
    struct TrieNode *old_node = new TrieNode;
    old_node->value = " ";
    old_node->freq = 0;
    old_node->parent = nullptr;
    for (int i = 0; i<first_val.size(); i++){
        struct TrieNode *node = new TrieNode;
        node->value = first_val.substr(0,i+1);
        std::cout << node->value << "\n";
        node->freq = 0;
        node->parent = old_node;
        int j = 0;
        while(old_node->childrens[j] != nullptr){
            j+=1;
        }
        old_node->childrens[j] = node;
        old_node = node;
    }
    old_node->freq = first_freq;
    std::cout << old_node->parent->value << " " << old_node->freq;

    // CREATION TOUT LES AUTRES NODES
    std::string previous_val;
    while (inFile >> line >> nb){
        // GARDER ANCIEN MOT
        // COMPARE AVEC L'ANCIEN MOT 
        // TANT QUE LES CARACTERES SONT DIFFERENT OU DE PLUS PETITE TAILLE REMONTE AU PARENT
        // QUAND MEME MOT AJOUTER FREQ
        // QUAND MEME LETTRE ALORS CREER FILS DROIT PUIS DESCENDRE
    }
    inFile.close();
}