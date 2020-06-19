#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <vector>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

using namespace std;

struct TrieNode{
    char value;
    int freq;
    vector<std::shared_ptr<struct TrieNode>> childrens;
    shared_ptr<struct TrieNode> parent;
};

string browse(string path, string word, shared_ptr<struct TrieNode> tree, int dist, int nb_error, int i, string words) {
    if (nb_error > dist){
        return words;
    }
    if (word[i] == '\0') {
        Document d;
        Document::AllocatorType& alloc = d.GetAllocator();

        d.SetObject();

        Value textWord;
        textWord.SetString(path.c_str(), alloc);
        Value textFreq;
        textFreq.SetInt(tree->freq);
        Value textDist;
        textDist.SetInt(nb_error);
        
        d.AddMember("word", textWord, alloc);
        d.AddMember("freq", textFreq, alloc);
        d.AddMember("distance", textDist, alloc);
                
        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        d.Accept(writer);

        words.append(buffer.GetString());
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

shared_ptr<struct TrieNode> read_from_file(ifstream& inFile) {
    int size;
    char value;
    shared_ptr<struct TrieNode> root = make_shared<struct TrieNode>();
    
    inFile.read(reinterpret_cast<char*>(&root->value), sizeof(char));
    inFile.read(reinterpret_cast<char *>(&root->freq), sizeof(int));
    inFile.read(reinterpret_cast<char *>(&size), sizeof(int));
    
    for (int i = 0; i < size; i++) {
        root->childrens.push_back(read_from_file(inFile));
    }
    return root;
}

/*void test_dist(shared_ptr<struct TrieNode> root, string word){
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
}*/

int my_menu(shared_ptr<struct TrieNode> root){
    while (1) {
        char line[4096];
        if (isatty(0))
            putchar('>');
        char *message = fgets(line, 4095, stdin);
        if (message == NULL)
                   return 0;
        if (strcmp(line, "\n") != 0){
            size_t i = 0;
            for (; line[i] != '\n'; i++);
            line[i] = '\0';
            char *approx = strtok(line, " \t");
            char *distance = strtok(NULL, " \t");
            char *word = strtok(NULL, " \t");

            vector<string> words(1000); //PAS OUF
            try {
                if (strcmp(approx, "approx") == 0) {
                    string words = "[";
                    words = browse("", word, root, stoi(distance), 0, 0, words);
                    words.push_back(']');
                    std::cout << words << std::endl;
                }
            } catch (std::invalid_argument const &e) {
                continue;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2)
    {
        printf("usage: %s FILE\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    ifstream inFile;
    inFile.open(argv[1]);
    if (!inFile){
        cerr << "Unable to read the file";
        exit(1);
    }
    shared_ptr<struct TrieNode> root = read_from_file(inFile);
    printf("[Log] Read: %s entries.\n", "");
    my_menu(root);
}
