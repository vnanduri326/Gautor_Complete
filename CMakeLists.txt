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

include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)

add_executable(trie_tests 
    tests/trie_tests.cpp
)

add_executable(hashmap_tests
        tests/hashmap_tests.cpp
)

target_link_libraries(trie_tests PRIVATE gautor_core Catch2::Catch2WithMain)
target_link_libraries(hashmap_tests PRIVATE gautor_core Catch2::Catch2WithMain)

add_test(NAME TrieTests COMMAND trie_tests)
add_test(NAME HashTests COMMAND hashmap_tests)
