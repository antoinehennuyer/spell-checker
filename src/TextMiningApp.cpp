#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <list>
#include <sstream>
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

void printJson(Document& d) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);
    cout << ',' << buffer.GetString();
}

Document createJson(string path, shared_ptr<struct TrieNode> tree, int nb_error) { //crée et retourne un json {word: _, freq: _, dist: _}
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

    return d;
}

vector<vector<size_t>> initErrorsMatrix(size_t length_word, size_t length_path) { // initialise et retourne la matrice des distances
    vector<vector<size_t>> nb_errors(length_word + 1, vector<size_t>(length_path + 1));
    size_t i = 0;
    if (length_word < length_path) {
        for (; i <= length_word; i++) {
            nb_errors[0][i] = i;
            nb_errors[i][0] = i;
        }
        for (; i <= length_path; i++) {
            nb_errors[0][i] = i;
        }
    } else {
        for (; i <= length_path; i++) {
            nb_errors[0][i] = i;
            nb_errors[i][0] = i;
        }
        for (; i <= length_word; i++) {
            nb_errors[i][0] = i;
        }
    }
    return nb_errors;
}
size_t getDistance(string word, string path) //calcule et retourne la distance entre le mot "word" et le mot "path"
{
    size_t length_word = word.length();
    size_t length_path = path.length();
    
    vector<vector<size_t>> nb_errors = initErrorsMatrix(length_word, length_path);
        
    size_t i;
    size_t j;
    for (i = 0; i < length_word; i++) {
        for (j = 0; j < length_path; j++) {
            size_t errors = 0;
            if (word[i] != path[j]) {
                errors = 1;
            }
            nb_errors[i + 1][j + 1] = min(nb_errors[i][j] + errors, min(nb_errors[i][j + 1], nb_errors[i + 1][j]) + 1);
            if (j > 0 && path[j - 1] == word[i] && i > 0 && word[i - 1] == path[j]
                && nb_errors[i - 1][j - 1] + errors < nb_errors[i + 1][j + 1]) { // swap
                nb_errors[i + 1][j + 1] = nb_errors[i - 1][j - 1] + errors;
            }
        }
    }
    return nb_errors[i][j];
}

void browse(string path, string word, shared_ptr<struct TrieNode> tree, size_t dist, list<Document>& words) { //parcours l'arbre en prefix et retourne la liste des mots d'une distance "dist" du mot "word"
    unsigned size = tree->childrens.size();
    if (word.length() + dist >= path.length()) { // si la taille du mot recherché + la distance est plus petit que la taille du mot trouvé => impossible
        for (unsigned j = 0; j < size; j++) { // on parcourt nos childrens
            string new_path = path;
            new_path.push_back(tree->childrens[j]->value); // on ajoute le caractère du noeud à notre chemin
            if (tree->childrens[j]->freq) { // si le mot trouvé existe (freq != 0) alors on calcule sa distance
                size_t nb_errors = getDistance(word, new_path);
                if (nb_errors <= dist) { // si la distance n'est pas supérieur à notre distance maximal alors on ajoute le mot à notre liste
                    words.push_back(createJson(new_path, tree->childrens[j], nb_errors));
                    if (!dist) { // optimisation pour la distance 0, un seul mot possible, si on le trouve, on l'ajoute puis on sort de la fonction
                        return;
                    }
                }
            }
            browse(new_path, word, tree->childrens[j], dist, words);
        }
    }
}

shared_ptr<struct TrieNode> read_from_file(ifstream& inFile) { //lit le fichier binaire et le retransforme en la structure tree TrieNode en prefix
    int size;
    shared_ptr<struct TrieNode> root = make_shared<struct TrieNode>();
    
    inFile.read(reinterpret_cast<char*>(&root->value), sizeof(char));
    inFile.read(reinterpret_cast<char *>(&root->freq), sizeof(int));
    inFile.read(reinterpret_cast<char *>(&size), sizeof(int));
    
    for (int i = 0; i < size; i++) {
        root->childrens.push_back(read_from_file(inFile));
    }
    return root;
}

int my_menu(shared_ptr<struct TrieNode> root){ // menu où on entre les instructions "approx"
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

            try {
                if (strcmp(approx, "approx") == 0) {
                    list<Document> words;

                    size_t dist;
                    std::stringstream sstream(distance);
                    sstream >> dist;
                    
                    browse("", word, root, dist, words);
                    cout << "[";
                    /*for (auto& i: words) {
                        printJson(i);
                        cout <<
                    }*/
                    list<Document>::iterator it;
                    auto begin = words.begin();
                    auto end = words.end();
                    for(it = begin; it!= end; ++it)
                    {
                        Document& item = *it;
                        if (it != begin) {
                            printJson(item);
                        } else {
                            StringBuffer buffer;
                            Writer<StringBuffer> writer(buffer);
                            item.Accept(writer);
                            cout << buffer.GetString();
                        }
                    }
                    cout << "]" << endl;
                }
            } catch (std::invalid_argument const &e) {
                continue;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: %s FILE\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    ifstream inFile;
    inFile.open(argv[1]);
    if (!inFile) {
        cerr << "Unable to read the file\n";
        exit(1);
    }
    shared_ptr<struct TrieNode> root = read_from_file(inFile);
    my_menu(root);
}
