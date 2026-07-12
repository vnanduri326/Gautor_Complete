# Gautor Complete

A basic autocomplete engine and gui to compare the speeds between tries and hashmaps in autocomplete.

The project uses SFML 2.6.1 for the gui and Catch2 for unit testing. All dependencies are handled automatically.

## Prerequisites
* A C++14 (or newer) compiler
* CMake (3.14+)
* Git


## How to Build and Run in Terminal
```bash
# 1. Clone the repository
git clone [https://github.com/vnanduri326/Gautor_Complete.git](https://github.com/vnanduri326/Gautor_Complete.git)
cd Gautor_Complete

# 2. Create a build directory and configure CMake
mkdir build
cd build
cmake ..

# 3. Build the project
cmake --build .

# 4. Run the executables from inside the build folder
./gui
./benchmark