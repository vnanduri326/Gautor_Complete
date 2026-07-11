#pragma once
#include <string>
#include <vector>

struct HashNode {
    std::string prefix;
    std::vector<std::pair<std::string, uint64_t>> probableWords;
    HashNode* nextPossiblePrefix; // For avoiding hash collisions

    HashNode(std::string prefix);
    void insertPredictedWord(const std::string& word, uint64_t frequency, int maxCap);
};

class HashMap {
    const size_t BUCKETS = 100003; // Good size for 1/3mil words
    const int PREDICTED_WORD_CAPACITY;
    HashNode** table;

    uint64_t hashFunction(const std::string& prefix);
    void insertHelper(const std::string& prefix, const std::string& word, uint64_t frequency);

public:
    HashMap(int wordCap);
    ~HashMap();

    void insert(const std::string& word, uint64_t frequency);
    std::vector<std::string> getTopK(const std::string& prefix, int k);
    size_t memoryBytes() const;      // heap bytes owned by this table
    size_t nodeCount() const;        // total HashNodes (= distinct prefixes stored)
    double avgChainLength() const;   // load factor: how well BUCKETS is sized
    size_t longestChain() const;     // worst-case lookup walk
};
};