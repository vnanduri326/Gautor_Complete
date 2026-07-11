#include "../include/trie.h"
#include <algorithm>
#include <cctype>

TrieNode::TrieNode(){
    isEndOfWord = false;
    frequency = 0;
    for(int i = 0; i < 26; ++i){
        children[i] = nullptr;
    }
}
TrieNode::~TrieNode(){
    for(int i = 0; i < 26; ++i){
        delete children[i];
    }
}
Trie::Trie() {
    root = new TrieNode();
}
Trie::~Trie() {
    delete root;
}
void Trie::dfsHelper(TrieNode* current, std::string currentWord, std::vector<std::pair<std::string, uint64_t>>& candidates) {//helps find topK words
    if (current->isEndOfWord) {
        candidates.push_back({currentWord, current->frequency});
    }
    for (int i = 0; i < 26; ++i) {
        if (current->children[i]) {
            dfsHelper(current->children[i], currentWord + static_cast<char>('a' + i), candidates);
        }
    }
}
void Trie::insert(const std::string& word, uint64_t frequency) {
    TrieNode* current = root;
    for (char c : word) {
        int index = c - 'a';
        if (!current->children[index]) {
            current->children[index] = new TrieNode();
        }
        current = current->children[index];
    }
    current->isEndOfWord = true;
    current->frequency = frequency;
}
std::vector<std::string> Trie::getTopK(const std::string& prefix, int k) {
    TrieNode* current = root;
    for (char c : prefix){
        int index = std::tolower(c) - 'a';
        if (!current->children[index]) {
            return {}; //prefix not found
        }
        current = current->children[index];
    }
    std::vector<std::pair<std::string, uint64_t>> candidates;
    dfsHelper(current, prefix, candidates);
    std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
        if (a.second == b.second) {
            return a.first < b.first;
        }
        return a.second > b.second; //sort by frequency in descending order
    });
    std::vector<std::string> res;
    for (int i = 0; i < std::min(k, static_cast<int>(candidates.size())); ++i) {
        res.push_back(candidates[i].first);
    }
    return res;
}