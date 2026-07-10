#define CATCH_CONFIG_MAIN  
#include <catch2/catch.hpp>
#include "../include/trie.h"

TEST_CASE("Trie Basic Operations", "[trie]") {
    Trie trie;

    SECTION("Insertion and exact prefix match") {
        trie.insert("apple", 100);
        trie.insert("app", 500);
        trie.insert("apricot", 50);
        trie.insert("banana", 1000);

        std::vector<std::string> results = trie.getTopK("app", 2);
        
        REQUIRE(results.size() == 2);
        CHECK(results[0] == "app");
        CHECK(results[1] == "apple");
    }

    SECTION("Prefix not found") {
        trie.insert("apple", 100);
        
        std::vector<std::string> results = trie.getTopK("orange", 3);
        CHECK(results.empty());
    }

    SECTION("Requesting more results than available (k > total matching words)") {
        trie.insert("cat", 10);
        trie.insert("car", 20);

        std::vector<std::string> results = trie.getTopK("ca", 5);
        
        REQUIRE(results.size() == 2);
        CHECK(results[0] == "car"); 
        CHECK(results[1] == "cat"); 
    }

    SECTION("Frequency tie-breaking (Alphabetical sort)") {
        trie.insert("bat", 50);
        trie.insert("bad", 50);

        std::vector<std::string> results = trie.getTopK("ba", 2);
        REQUIRE(results.size() == 2);
        CHECK(results[0] == "bad");
        CHECK(results[1] == "bat");
    }

    SECTION("Empty prefix handling") {
        trie.insert("a", 10);
        trie.insert("b", 20);

        std::vector<std::string> results = trie.getTopK("", 2);
        REQUIRE(results.size() == 2);
        CHECK(results[0] == "b");
        CHECK(results[1] == "a");
    }
}