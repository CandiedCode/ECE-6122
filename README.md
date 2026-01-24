# ECE-6122
Advanced Programming

# Load modules on Pace

// Load latest version of gcc available on Pace
module load zstd/1.5.6-5ktxetg
module load gcc/14.2.0
module load cmake/3.26.3


# Build
cd build
cmake ..
make ./Problem1_test

ctest --verbose


clang-format -i Homework_0/*.cpp

g++ Homework_0/Problem1.cpp -o test


g++ -std=c++17 -Wall -Wextra -o program Problem1.cpp 

python3 -m pip install --user cpplint