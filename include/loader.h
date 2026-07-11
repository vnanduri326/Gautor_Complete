#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct WordEntry {
    std::string word;
    uint64_t freq;
};

struct LoadStats {
    size_t rowsRead      = 0;
    size_t accepted      = 0;
    size_t rejectedEmpty = 0;
    size_t rejectedChar  = 0;   // contained a character outside a-z
    size_t rejectedParse = 0;   // malformed row / bad count
    size_t lowercased    = 0;   // had uppercase, was folded
};

// Loads a `word,count` CSV (Kaggle unigram_freq.csv).
//
// GUARANTEE: every returned word is non-empty and contains ONLY [a-z].
// This is what keeps the Trie's `c - 'a'` indexing in bounds. Do not bypass it.
//
// limit = 0 means "load everything".
std::vector<WordEntry> loadWords(const std::string& path,
                                 LoadStats& stats,
                                 bool skipHeader = true,
                                 size_t limit = 0);

// True if s is non-empty and every char is in [a-z].
bool isCleanWord(const std::string& s);