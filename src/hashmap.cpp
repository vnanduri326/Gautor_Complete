#include "include/hashmap.h"
#include <algorithm>

HashNode::HashNode(std::string prefix) : prefix(prefix), nextPossiblePrefix(nullptr) {}

HashMap::HashMap(int wordCap) : PREDICTED_WORD_CAPACITY(wordCap) {
    table = new HashNode*[BUCKETS]; // Creates arr of HashNode ptrs of total size 100003
    for (size_t = 0; i < BUCKETS; i++) { table[i] = nullptr; }
}

HashMap::~HashMap() {
    // Deletes each prefix at index i
    for (size_t = 0; i < BUCKETS; i++) {
        HashNode* curr = table[i];
        while (curr != nullptr) {
            HashNode* next = curr->nextPossiblePrefix;
            delete curr;
            curr = next;
        }
    }
    delete[] table;
}