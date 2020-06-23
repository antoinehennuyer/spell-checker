/**
 * @file PatriciaTrie.hh
 */
#include <algorithm>
#include <string>
#include <thread>
#include <vector>
/**
 * @brief Structure of the Patricia Trie
 */
struct TrieNode{
    std::string value; /**< value containing the characters of a node.*/
    int freq; /**< the frequency of a word inside the tree.*/
    std::vector<std::shared_ptr<struct TrieNode>> childrens; /**< vector of all the childrens of the node.*/
    std::shared_ptr<struct TrieNode> parent; /**< pointer to the parent of the node.*/
};