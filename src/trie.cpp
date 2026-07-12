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
        if (index < 0 || index >= 26) { return; }   // ADD: reject non a-z
        if (!current->children[index]) { current->children[index] = new TrieNode(); }
        current = current->children[index];
    }
    current->isEndOfWord = true;
    current->frequency = frequency;
}

std::vector<std::string> Trie::getTopK(const std::string& prefix, int k) {
    TrieNode* current = root;

    std::string lowerPrefix;
    for (char c : prefix)
        lowerPrefix += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

    for (char c : lowerPrefix) {
        int index = c - 'a';
        if (index < 0 || index >= 26) { return {}; }
        if (!current->children[index]) { return {}; }
        current = current->children[index];
    }

    std::vector<std::pair<std::string, uint64_t>> candidates;
    dfsHelper(current, lowerPrefix, candidates);

    std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
        if (a.second == b.second) {
            return a.first < b.first;
        }
        return a.second > b.second;
    });

    std::vector<std::string> result;
    for (int i = 0; i < std::min(k, static_cast<int>(candidates.size())); ++i) {
        result.push_back(candidates[i].first);
    }

    return result;
}

size_t Trie::countNodes(TrieNode* node) const {
    if (!node) return 0;
    size_t total = 1;                               // this node
    for (int i = 0; i < 26; ++i) {
        total += countNodes(node->children[i]);     // plus everything below it
    }
    return total;
}

size_t Trie::nodeCount() const {
    return countNodes(root);
}

size_t Trie::memoryBytes() const {
    // Every node is identical in size: 26 pointers + a bool + a uint64 (padded).
    // The trie stores no strings, so nodeCount * nodeSize is the whole story.
    return nodeCount() * sizeof(TrieNode);
}