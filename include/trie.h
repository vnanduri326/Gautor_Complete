#pragma once
#include <string>
#include <vector>
#include <cstdint> 

struct TrieNode {
    TrieNode* children[26];
    bool isEndOfWord;
    uint64_t frequency;

    TrieNode();
    ~TrieNode();
};

class Trie {
private:
    TrieNode* root;
    void dfsHelper(TrieNode* current, std::string currentWord, std::vector<std::pair<std::string, uint64_t>>& candidates);
    size_t countNodes(TrieNode* node) const;

public:
    Trie();
    ~Trie();

    void insert(const std::string& word, uint64_t frequency);
    std::vector<std::string> getTopK(const std::string& prefix, int k);
    size_t memoryBytes() const;   // heap bytes owned by this trie
    size_t nodeCount() const;     // total TrieNodes allocated
};