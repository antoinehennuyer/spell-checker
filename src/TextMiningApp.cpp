/**
 * @file TextMiningApp.cpp
 */

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
#include <thread>
#include "PatriciaTrie.hh"
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>


using namespace rapidjson;

using namespace std;



/**
* compare deux documents json de la liste des documents json afin de les trier en fonction des consignes
*
* @param d1 est un Document(Json)
*
* @param d2 est un Document(Json)
*
* @return retourne un boulean true si le document d1 doit être placé avant le document d2 sinon false
*/
bool compare(Document& d1, Document& d2) {
    if (d1["distance"].GetInt() == d2["distance"].GetInt()) {
        if (d1["freq"].GetInt() == d2["freq"].GetInt()) {
            string word1 = d1["word"].GetString();
            string word2 = d2["word"].GetString();
            int compare = word1.compare(word2);
            return compare < 0;
        } else {
            return (d1["freq"].GetInt() > d2["freq"].GetInt());
        }
    } else {
        return (d1["distance"].GetInt() < d2["distance"].GetInt());
    }
}

/**
* print le document json sous la forme : "{word: _, freq: _, dist: _},"
*
* @param d est le Document(Json)
*/
void printJson(Document& d) { //print json document:{word: _, freq: _, dist: _},
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);
    cout << ',' << buffer.GetString();
}

/**
* crée et retourne un json {word: _, freq: _, dist: _} à partir du path, de la fréquence du noeud et de lsa distance
*
* @param path est la chaine de caractère formée de tous les caractères rencontrés lors de notre descente à partir du root
*
* @param tree est le patricia tree
*
* @param nb_error nombre de "difference" rencontré (erreur) -> distance
*
* @return le document au format json {word: _, freq: _, dist: _}
*/
Document createJson(string path, shared_ptr<struct TrieNode> tree, int nb_error) {
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

/**
* initialise et retourne la matrice des distances
*
* @param length_word est la taille du mot donné en entrée
*
* @param length_path est la taille de la chaine de caractère formée de tous les caractères rencontrés lors de notre descente à partir du root
*
* @return retourne la matrice des distances entre le mot word et le mot path
*/
vector<vector<size_t>> initErrorsMatrix(size_t length_word, size_t length_path) {
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

/**
* calcule et retourne la distance entre le mot "word" et le mot "path" à l'aide d'une matrice
*
* @param word est le mot donné en entrée
*
* @param path est la chaine de caractère formée de tous les caractères rencontrés lors de notre descente à partir du root
*
* @return retourne la distance entre le mot word et le mot path
*/
size_t getDistance(string word, string path) {
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

/**
* Parcourt l'arbre en prefix et ajoute à la liste words le document Json des mots d'une distance inférieure ou égale à "dist" du mot "word"
*
* @param path est la chaine de caractère formée de tous les caractères rencontrés lors de notre descente à partir du root
*
* @param word est le mot donné en entrée
*
* @param tree est le patricia tree
*
* @param dist est la distance donné en entrée
*
* @param words est la liste des documents(Json) ajouté au cours du parcourt de l'arbre
*/
void browse(string path, string word, shared_ptr<struct TrieNode> tree, size_t dist, list<Document>& words) {
    int size = tree->childrens.size();
    if (word.length() + dist >= path.length()) { // si la taille du mot recherché + la distance est plus petit que la taille du mot trouvé => impossible
        for (unsigned j = 0; j < size; j++) { // on parcourt nos childrens
            string new_path = path;
            new_path.append(tree->childrens[j]->value); // on ajoute les caractères du noeud à notre chemin
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

/**
* lit le fichier binaire et le retransforme en la structure tree TrieNode en prefix
*
* @param inFile FileStream est le fichier binaire
*
* @return root est le patricia tree
*/
shared_ptr<struct TrieNode> read_from_file(ifstream& inFile) {
    int size;
    int length_data;
    shared_ptr<struct TrieNode> root = make_shared<struct TrieNode>();
    
    inFile.read(reinterpret_cast<char *>(&length_data), sizeof(int));
    char value[length_data + 1];
    value[length_data] = '\0';
    inFile.read(reinterpret_cast<char*>(&value), length_data);
    inFile.read(reinterpret_cast<char *>(&root->freq), sizeof(int));
    inFile.read(reinterpret_cast<char *>(&size), sizeof(int));
    
    root->value = string(value);

    for (int i = 0; i < size; i++) {
        root->childrens.push_back(read_from_file(inFile));
    }
    return root;
}

int getSize(char* buffer) {
    return (int) *buffer;
}

int *getFreq(char* buffer) {
    return (int*) (buffer + sizeof(int));
}

size_t getValue(char* buffer, string &value) {
    size_t i = 0;
    while ((char) *(buffer + 2 * sizeof(int) + i) != '\0') {
        char c = (char) *(buffer + 2 * sizeof(int) + i);
        value += c;
        i = i + 1;
    }
    return i;
}

unsigned int getoffset(char *buff_child, int child, int size_value){
    return *(unsigned int *)(buff_child + 2 * sizeof(int) + size_value * sizeof(char) + 1 + sizeof(unsigned int) * child);

}
void read_file(char* path) {
    int fd = open(path, O_RDONLY);
    if (fd == -1)
        exit(EXIT_FAILURE);
    struct stat st;
    if (fstat(fd, &st) != 0)
    {
        close(fd);
        exit(EXIT_FAILURE);
    }
    long unsigned size_check = st.st_size;
    if (size_check < sizeof(shared_ptr<struct TrieNode>))
    {
        close(fd);
        exit(EXIT_FAILURE);
    }
    void *buffer_void = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    char *buffer = (char *)buffer_void;
    // ROOT NODE
    int size = getSize(buffer); //(int) *buffer;
    int freq = *getFreq(buffer); //(int) *(buffer + sizeof(int));
    string value = "";
    size_t i = getValue(buffer, value) ;
    printf("size : %d\n", size);
    printf("freq : %d\n", freq);
    printf("value : %s\n", value.c_str());
    // SON OF THE ROOT

    for (size_t j = 0; j < size; j++) {
        unsigned offset = getoffset(buffer, j, i); // i = size of the parent's value
        printf("OFFSET : %u\n", offset);
        char *buffer_child = buffer + offset;//(unsigned int) *(buffer + 2 * sizeof(int) + j);
        freq = *getFreq(buffer_child); //(int) *(buffer + sizeof(int));
        string value;
        getValue(buffer_child, value);
        printf("second size : %d\n", getSize(buffer_child));
        printf("second freq : %d\n", freq);
        printf("second value : %s\n", value.c_str());
    }
}

/**
*  menu où l'on entre les instructions "approx"
*
* @param root Patricia Tree
*
* @return 0 si pas d'erreur 1 sinon
*/
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
                    words.sort(compare);
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
    read_file(argv[1]);
    /*ifstream inFile;
    inFile.open(argv[1]);
    if (!inFile) {
        cerr << "Unable to read the file\n";
        exit(1);
    }
    shared_ptr<struct TrieNode> root = read_from_file(inFile);
    inFile.close();
    my_menu(root);*/
}
