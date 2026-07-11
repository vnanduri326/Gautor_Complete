cmake_minimum_required(VERSION 3.14)
project(Gautor_Complete VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_library(gautor_core 
    src/hashmap.cpp
    src/trie.cpp
    src/loader.cpp
)

target_include_directories(gautor_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(benchmark src/benchmark.cpp)
target_link_libraries(benchmark PRIVATE gautor_core)

find_package(SFML 2.5 COMPONENTS graphics window system)
if(SFML_FOUND)
    add_executable(gui src/gui.cpp)
    target_link_libraries(gui PRIVATE gautor_core sfml-graphics sfml-window sfml-system)
else()
    message(WARNING "SFML not found - skipping gui target")
endif()

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
