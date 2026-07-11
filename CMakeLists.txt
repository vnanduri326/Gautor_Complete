cmake_minimum_required(VERSION 3.14)
project(Gautor_Complete VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(gautor_core 
    src/hashmap.cpp
    src/trie.cpp
)

target_include_directories(gautor_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

enable_testing()

find_package(Catch2 REQUIRED)

add_executable(trie_tests 
    tests/trie_tests.cpp
)

target_link_libraries(trie_tests PRIVATE gautor_core Catch2::Catch2)

# Make it discoverable by CTest
add_test(NAME TrieTests COMMAND trie_tests)