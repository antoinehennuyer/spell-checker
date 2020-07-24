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
#include <thread>
#include "PatriciaTrie.hh"
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <cstring>

//using namespace rapidjson;

using namespace std;

struct Document {
    string word;
    int freq;
    int distance;
};

/**
 * retourne la taille du noeud
 *
 * @param buffer est le patricia tree
 *
 * @return retourne la taille du noeud
*/
int getSize(char* buffer) {
    return (int) *buffer;
}

/**
 * retourne la fréquence du noeud
 *
 * @param buffer est le patricia tree
 *
 * @return retourne la fréquence du noeud
*/
int *getFreq(char* buffer) {
    return (int*) (buffer + sizeof(int));
}

/**
 * retourne la valeur du noeud
 *
 * @param buffer est le patricia tree
 *
 * @param value  est la string parcouru
 *
 * @param size_value  la taille de la valeur
 *
 * @return retourne la valeur du noeud
*/
size_t getValue(char* buffer, string &value) {
    size_t i = 0;
    while ((char) *(buffer + 2 * sizeof(int) + i) != '\0') {
        char c = (char) *(buffer + 2 * sizeof(int) + i);
        value += c;
        i = i + 1;
    }
    return i;
}

/**
 * compare deux documents json de la liste des documents json afin de les trier en fonction des consignes
 *
 * @param buff_child est le fils du patricia tree
 *
 * @param child  est le numéro du fils
 *
 * @param size_value  la taille de la valeur
 *
 * @return retourne offset du fils child
*/
unsigned int getoffset(char *buff_child, int child, int size_value){
    return *(unsigned int *)(buff_child + 2 * sizeof(int) + (size_value + 1) * sizeof(char) + sizeof(unsigned int) * child);
}

/**
 * compare deux documents json de la liste des documents json afin de les trier en fonction des consignes
 *
 * @param d1 est un Document(Json)
 *
 * @param d1 est un Document(Json)
 *
 * @return retourne un boulean true si le document d1 doit être placé avant le document d2 sinon false
*/
bool compare(Document& d1, Document& d2) {
    if (d1.distance == d2.distance) {
        if (d1.freq == d2.freq) {
            string word1 = d1.word;
            string word2 = d2.word;
            int compare = word1.compare(word2);
            return compare < 0;
        } else {
            return (d1.freq > d2.freq);
        }
    } else {
        return (d1.distance < d2.distance);
    }
}

/**
 * print le document json sous la forme : "{word: _, freq: _, dist: _},"
 *
 * @param d est le Document(Json)
*/
void printJson(Document& d) {
    cout << ",{\"word\":\"" << d.word << "\",\"freq\":" << d.freq << ",\"distance\":" << d.distance << "}";
}

/**
 * crée et retourne un json {word: _, freq: _, dist: _} à partir du path, de la fréquence du noeud et de la distance
 *
 * @param path est la chaine de caractère formée de tous les caractères rencontrés lors de notre descente à partir du root
 *
 * @param buffer est le patricia tree
 *
 * @param nb_error nombre de "difference" rencontré (erreur) -> distance
 *
 * @return le document au format json {word: _, freq: _, dist: _}
*/
Document createJson(string path, char* buffer, int nb_error) {
    struct Document d = {path.c_str(), *getFreq(buffer), nb_error};

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
void browse(string path, string word, char* root, char* tree, size_t dist, list<Document>& words) {
    int size = getSize(tree);
    string value = "";
    size_t i = getValue(tree, value);//PAS BIEN

    if (word.length() + dist >= path.length()) { // si la taille du mot recherché + la distance est plus petit que la taille du mot trouvé => impossible
        for (unsigned j = 0; j < size; j++) { // on parcourt nos childrens
            string new_path = path;
            unsigned offset = getoffset(tree, j, i); // i = size of the parent's value
            char *child = root + offset;
            value = "";
            getValue(child, value); //RECUP LE I
            new_path.append(value); // on ajoute les caractères du noeud à notre chemin
            int freq = *getFreq(child);
            if (freq) { // si le mot trouvé existe (freq != 0) alors on calcule sa distance
                size_t nb_errors = getDistance(word, new_path);
                if (nb_errors <= dist) { // si la distance n'est pas supérieur à notre distance maximal alors on ajoute le mot à notre liste
                    words.push_back(createJson(new_path, child, nb_errors));
                    if (!dist) { // optimisation pour la distance 0, un seul mot possible, si on le trouve, on l'ajoute puis on sort de la fonction
                        return;
                    }
                }
            }
            browse(new_path, word, root, child, dist, words);
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

char* read_file(int fd, long unsigned size_check) {
    void *buffer_void = mmap(NULL, size_check, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    return (char *)buffer_void;
}

/**
 *  menu où l'on entre les instructions "approx"
 *
 * @param root Patricia Tree
 *
 * @return 0 si pas d'erreur 1 sinon
*/
int my_menu(char* root){ // menu où on entre les instructions "approx"
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
                    
                    browse("", word, root, root, dist, words);
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
                            cout << "{\"word\":\"" << item.word << "\",\"freq\":" << item.freq << ",\"distance\":" << item.distance << "}";
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
    int fd = open(argv[1], O_RDONLY);
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
    char* root = read_file(fd, size_check);
    my_menu(root);
    if (munmap(root, size_check) == -1) {
       cerr << "munmap failed with error:";
    }
}
