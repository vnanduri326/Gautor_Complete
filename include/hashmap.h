#pragma once
#include <string>
#include <cstdint>
#include <vector>

struct HashNode {
    std::string prefix;
    std::vector<std::pair<std::string, uint64_t>> possibleWords;
    HashNode* nextPossiblePrefix; // For avoiding hash collisions

    HashNode(std::string prefix);
    void insertPredictedWord(const std::string&, int maxCap);
};

class HashMap {
    const size_t BUCKETS = 100003; // Good size for 1/3mil words
    const int PREDICTED_WORD_CAPACITY;
    HashNode** table;

    uint64_t hashFunction(const std::string& prefix);
    void updateBucket(const std::string& prefix, const std::string& word, uint64_t frequency);

public:
    HashMap(int wordCap);
    ~HashMap();

    void insert(const std::string& word, uint64_t frequency);
    std::vector<std::string> getTopK(const std::string& word, int k);
};