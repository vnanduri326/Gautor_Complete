#include "../include/hashmap.h"
#include <cctype>

HashNode::HashNode(std::string prefix) : prefix(prefix), nextPossiblePrefix(nullptr) {}

void HashNode::insertPredictedWord(const std::string& word, uint64_t frequency, int maxCap) {
    if (probableWords.size() == 0) {
        probableWords.push_back({word, frequency});
        return;
    }

    // Insert word at proper position based on freq
    bool inserted = false;
    for (auto it = probableWords.begin(); it != probableWords.end(); it++) {
        if (frequency > it->second || (frequency == it->second && word < it->first)) {
            probableWords.insert(it, {word, frequency});
            inserted = true;
            break;
        }
    }

    if (!inserted && probableWords.size() < maxCap) { probableWords.push_back({word, frequency}); }

    if (probableWords.size() > maxCap) {
        probableWords.pop_back();
    }
}


HashMap::HashMap(int wordCap) : PREDICTED_WORD_CAPACITY(wordCap) {
    table = new HashNode*[BUCKETS]; // Creates arr of HashNode ptrs of total size 100003
    for (size_t i = 0; i < BUCKETS; i++) { table[i] = nullptr; }
}

HashMap::~HashMap() {
    // Deletes each prefix at index i
    for (size_t i = 0; i < BUCKETS; i++) {
        HashNode* curr = table[i];
        while (curr != nullptr) {
            HashNode* next = curr->nextPossiblePrefix;
            delete curr;
            curr = next;
        }
    }
    delete[] table;
}

// Gets table index for prefix
uint64_t HashMap::hashFunction(const std::string& prefix) {
    uint64_t hashVal = 0;
    uint64_t power = 1;

    // ONLY LOWERCASE
    for (char ch : prefix) {
        hashVal = (hashVal + (int(ch) - 96) * power) % 1000000009;
        power = power * 31 % 1000000009;
    }
    return hashVal % BUCKETS;
}

void HashMap::insert(const std::string& word, uint64_t frequency) {
    std::string prefix = "";

    insertHelper(prefix, word, frequency); // Insert "" for parity with trie, to get words with all time highest frequency
    for (char ch : word) {
        prefix += ch;
        insertHelper(prefix, word, frequency);
    }
}

void HashMap::insertHelper(const std::string& prefix, const std::string& word, uint64_t frequency) {
    uint64_t index = hashFunction(prefix);

    HashNode* curr = table[index];
    HashNode* prev = nullptr;
    while (curr != nullptr && curr->prefix != prefix) {
        prev = curr;
        curr = curr->nextPossiblePrefix;
    }

    // Prefix not yet in table
    if (curr == nullptr) {
        curr = new HashNode(prefix);
        if (prev == nullptr) { table[index] = curr; }
        else { prev->nextPossiblePrefix = curr; }
    }

    curr->insertPredictedWord(word, frequency, PREDICTED_WORD_CAPACITY);
}

std::vector<std::string> HashMap::getTopK(const std::string& prefix, int k) {
    // Hashmap only has lowercase words, thus search must only be lowercase too
    std::string lowercasePrefix = "";
    for (char ch : prefix) { lowercasePrefix += std::tolower(ch); }

    uint64_t index = hashFunction(lowercasePrefix);
    std::vector<std::string> topK;

    // Get proper prefix in case of collision
    HashNode* curr = table[index];
    while (curr != nullptr && curr->prefix != lowercasePrefix) {
        curr = curr->nextPossiblePrefix;
    }

    if (curr == nullptr) return {};

    for (int i = 0; i < curr->probableWords.size(); i++) {
        if (topK.size() == k) break;
        topK.push_back(curr->probableWords[i].first);
    }
    return topK;
}