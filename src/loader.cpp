#include "../include/loader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>

bool isCleanWord(const std::string& s) {
    if (s.empty()) return false;
    for (unsigned char c : s) {
        if (c < 'a' || c > 'z') return false;
    }
    return true;
}

std::vector<WordEntry> loadWords(const std::string& path,
                                 LoadStats& stats,
                                 bool skipHeader,
                                 size_t limit) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("could not open dataset: " + path);

    std::vector<WordEntry> out;
    out.reserve(limit ? limit : 350000);

    std::string line;
    if (skipHeader) std::getline(in, line);   // discard "word,count"

    while (std::getline(in, line)) {
        // Kaggle's file ships with CRLF endings; strip the stray '\r'
        // or the count will fail to parse on the last column.
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;

        stats.rowsRead++;

        size_t comma = line.find(',');
        if (comma == std::string::npos) { stats.rejectedParse++; continue; }

        std::string word = line.substr(0, comma);
        std::string countStr = line.substr(comma + 1);

        if (word.empty()) { stats.rejectedEmpty++; continue; }

        // Fold to lowercase so the Trie and HashMap see identical keys.
        bool hadUpper = false;
        for (char& c : word) {
            if (c >= 'A' && c <= 'Z') { c = c - 'A' + 'a'; hadUpper = true; }
        }
        if (hadUpper) stats.lowercased++;

        // Reject anything the Trie's 26-slot array cannot index
        // (apostrophes, hyphens, digits, accented chars -> negative index).
        if (!isCleanWord(word)) { stats.rejectedChar++; continue; }

        uint64_t freq = 0;
        try {
            freq = std::stoull(countStr);   // counts reach ~2.3e10 -> must be 64-bit
        } catch (...) {
            stats.rejectedParse++;
            continue;
        }

        out.push_back({word, freq});
        stats.accepted++;

        if (limit && out.size() >= limit) break;
    }
    return out;
}