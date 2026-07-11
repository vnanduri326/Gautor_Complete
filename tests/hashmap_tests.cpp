#include <catch2/catch_test_macros.hpp>
#include "../include/hashmap.h"

TEST_CASE("Hash Basic Operations", "[hashmap]") {
    HashMap hash(10);

    SECTION("Insertion and exact prefix match") {
        hash.insert("apple", 100);
        hash.insert("app", 500);
        hash.insert("apricot", 50);
        hash.insert("banana", 1000);

        std::vector<std::string> results = hash.getTopK("app", 2);
        
        REQUIRE(results.size() == 2);
        CHECK(results[0] == "app");
        CHECK(results[1] == "apple");
    }

    SECTION("Prefix not found") {
        hash.insert("apple", 100);
        
        std::vector<std::string> results = hash.getTopK("orange", 3);
        CHECK(results.empty());
    }

    SECTION("Requesting more results than available (k > total matching words)") {
        hash.insert("cat", 10);
        hash.insert("car", 20);

        std::vector<std::string> results = hash.getTopK("ca", 5);
        
        REQUIRE(results.size() == 2);
        CHECK(results[0] == "car"); 
        CHECK(results[1] == "cat"); 
    }

    SECTION("Frequency tie-breaking (Alphabetical sort)") {
        hash.insert("bat", 50);
        hash.insert("bad", 50);

        std::vector<std::string> results = hash.getTopK("ba", 2);
        REQUIRE(results.size() == 2);
        CHECK(results[0] == "bad");
        CHECK(results[1] == "bat");
    }

    SECTION("Empty prefix handling") {
        hash.insert("a", 10);
        hash.insert("b", 20);

        std::vector<std::string> results = hash.getTopK("", 2);
        REQUIRE(results.size() == 2);
        CHECK(results[0] == "b");
        CHECK(results[1] == "a");
    }

    SECTION("Prefix capacity reached insertion") {
        HashMap smallHash(3);

        smallHash.insert("car", 1);
        smallHash.insert("cat", 2);
        smallHash.insert("cab", 3);
        smallHash.insert("can", 4); // This should push "car" out of the list

        std::vector<std::string> results = smallHash.getTopK("ca", 5);

        REQUIRE(results.size() == 3);

        CHECK(results[0] == "can");
        CHECK(results[1] == "cab");
        CHECK(results[2] == "cat");
    }

    SECTION("Collision") {

        for (int i = 0; i < 1000; i++) {
            std::string randomWord = "word" + std::to_string(i);
            hash.insert(randomWord, i);
        }

        std::vector<std::string> results = hash.getTopK("word", 5);
        REQUIRE(results.size() == 5);
        CHECK(results[0] == "word999");
        CHECK(results[4] == "word995");
    }
}