// benchmark.cpp — Gautor Complete performance harness
//
// Measures build time, query latency, and memory for the Trie vs the HashMap,
// verifies both return identical results, and writes results/results.csv
// (open that in Google Sheets to chart it for the report).
//
// Run:    ./benchmark ../data/unigram_freq.csv [maxWords]
// Writes: results/results.csv
//
// NOTE: must be built with -O2. Timing an unoptimized build is meaningless.

#include "../include/loader.h"
#include "../include/trie.h"
#include "../include/hashmap.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <numeric>
#include <iomanip>

using Clock = std::chrono::steady_clock;

static double toMs(Clock::duration d) {
    return std::chrono::duration<double, std::milli>(d).count();
}

// ---------------------------------------------------------------- stats
struct Stats {
    double mean = 0, p50 = 0, p95 = 0, p99 = 0, maxv = 0;
};

static Stats summarize(std::vector<double> v) {
    Stats s;
    if (v.empty()) return s;
    std::sort(v.begin(), v.end());
    s.mean = std::accumulate(v.begin(), v.end(), 0.0) / v.size();
    auto at = [&](double q) { return v[std::min(v.size() - 1, (size_t)(q * v.size()))]; };
    s.p50 = at(0.50);
    s.p95 = at(0.95);
    s.p99 = at(0.99);
    s.maxv = v.back();
    return s;
}

// ---------------------------------------------------------------- query set
// Sample real prefixes from the dataset, bucketed by length, so we never
// benchmark prefixes that match nothing. Fixed seed => reproducible runs.
static std::vector<std::string> makeQueries(const std::vector<WordEntry>& data,
                                            int len, size_t n, std::mt19937& rng) {
    std::vector<std::string> pool;
    for (const auto& e : data)
        if ((int)e.word.size() >= len) pool.push_back(e.word.substr(0, len));

    if (pool.empty()) return {};
    std::shuffle(pool.begin(), pool.end(), rng);
    if (pool.size() > n) pool.resize(n);
    return pool;
}

int main(int argc, char** argv) {
    const std::string path = (argc > 1) ? argv[1] : "../data/unigram_freq.csv";
    const size_t limit     = (argc > 2) ? std::stoul(argv[2]) : 0;

    const int K = 5;                 // top-k returned to the user
    const int HASH_CAP = 10;         // words cached per prefix in the hashmap
    const size_t QUERIES_PER_LEN = 500;
    const std::vector<int> LENGTHS = {1, 2, 3, 4, 5, 6, 7, 8};

    // ------------------------------------------------ load
    LoadStats ls;
    auto data = loadWords(path, ls, true, limit);

    std::cout << "=== DATASET ===\n";
    std::cout << "rows read       : " << ls.rowsRead << "\n";
    std::cout << "accepted        : " << ls.accepted << "\n";
    std::cout << "rejected (char) : " << ls.rejectedChar << "\n";
    std::cout << "rejected (parse): " << ls.rejectedParse << "\n";
    std::cout << "lowercased      : " << ls.lowercased << "\n\n";

    if (data.size() < 100000)
        std::cout << "WARNING: under the 100,000-row project requirement.\n\n";

    // ------------------------------------------------ build TRIE
    Trie* trie = new Trie();
    auto t0 = Clock::now();
    for (const auto& e : data) trie->insert(e.word, e.freq);
    auto t1 = Clock::now();
    double trieBuildMs = toMs(t1 - t0);
    size_t trieBytes = trie->memoryBytes();

    // ------------------------------------------------ build HASHMAP
    HashMap* hash = new HashMap(HASH_CAP);
    auto t2 = Clock::now();
    for (const auto& e : data) hash->insert(e.word, e.freq);
    auto t3 = Clock::now();
    double hashBuildMs = toMs(t3 - t2);
    size_t hashBytes = hash->memoryBytes();

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "=== BUILD ===\n";
    std::cout << "Trie    : " << trieBuildMs << " ms, " << trieBytes / 1048576.0 << " MB, "
              << trie->nodeCount() << " nodes\n";
    std::cout << "HashMap : " << hashBuildMs << " ms, " << hashBytes / 1048576.0 << " MB, "
              << hash->nodeCount() << " prefix nodes, avg chain "
              << hash->avgChainLength() << ", longest chain " << hash->longestChain() << "\n\n";

    // ------------------------------------------------ correctness parity
    // Both structures must return the SAME top-k. This is a project
    // requirement, and it catches bugs before the demo.
    std::mt19937 rng(42);
    size_t mismatches = 0, checked = 0;
    for (int len : LENGTHS) {
        for (const auto& q : makeQueries(data, len, 200, rng)) {
            auto a = trie->getTopK(q, K);
            auto b = hash->getTopK(q, K);
            checked++;
            if (a != b) {
                if (mismatches < 3) {
                    std::cout << "  PARITY MISMATCH on \"" << q << "\"\n    trie:";
                    for (auto& s : a) std::cout << " " << s;
                    std::cout << "\n    hash:";
                    for (auto& s : b) std::cout << " " << s;
                    std::cout << "\n";
                }
                mismatches++;
            }
        }
    }
    std::cout << "=== PARITY ===\n" << mismatches << " mismatches / " << checked
              << " prefixes checked" << (mismatches ? "   <-- FIX BEFORE DEMO" : "  OK") << "\n\n";

    // ------------------------------------------------ query benchmark
    std::ofstream csv("results/results.csv");
    csv << "structure,prefix_len,mean_ms,p50_ms,p95_ms,p99_ms,max_ms,"
           "build_ms,memory_bytes,n_words,n_queries\n";

    std::cout << "=== QUERY LATENCY (ms) ===\n";
    std::cout << std::left << std::setw(6) << "len"
              << std::setw(26) << "Trie  mean / p95"
              << "HashMap  mean / p95\n";

    for (int len : LENGTHS) {
        auto qs = makeQueries(data, len, QUERIES_PER_LEN, rng);
        if (qs.empty()) continue;

        // warm-up: the first pass pays the cache misses, so don't time it
        for (const auto& q : qs) { auto r = trie->getTopK(q, K); (void)r; }
        for (const auto& q : qs) { auto r = hash->getTopK(q, K); (void)r; }

        std::vector<double> trieT, hashT;
        trieT.reserve(qs.size());
        hashT.reserve(qs.size());
        volatile size_t sink = 0;

        for (const auto& q : qs) {
            auto a = Clock::now();
            auto r = trie->getTopK(q, K);
            auto b = Clock::now();
            trieT.push_back(toMs(b - a));
            sink += r.size();          // stops the optimizer deleting the call
        }
        for (const auto& q : qs) {
            auto a = Clock::now();
            auto r = hash->getTopK(q, K);
            auto b = Clock::now();
            hashT.push_back(toMs(b - a));
            sink += r.size();
        }

        Stats st = summarize(trieT), sh = summarize(hashT);

        std::cout << std::left << std::setw(6) << len
                  << std::setw(26) << (std::to_string(st.mean).substr(0, 8) + " / " + std::to_string(st.p95).substr(0, 8))
                  << (std::to_string(sh.mean).substr(0, 8) + " / " + std::to_string(sh.p95).substr(0, 8)) << "\n";

        csv << "Trie," << len << "," << st.mean << "," << st.p50 << "," << st.p95 << ","
            << st.p99 << "," << st.maxv << "," << trieBuildMs << "," << trieBytes << ","
            << data.size() << "," << qs.size() << "\n";
        csv << "HashMap," << len << "," << sh.mean << "," << sh.p50 << "," << sh.p95 << ","
            << sh.p99 << "," << sh.maxv << "," << hashBuildMs << "," << hashBytes << ","
            << data.size() << "," << qs.size() << "\n";
    }
    csv.close();

    std::cout << "\nwrote results/results.csv\n";
    delete trie;
    delete hash;
    return 0;
}