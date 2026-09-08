Dependencies
GMP (GNU Multiple Precision Arithmetic Library) - For big integer operations
OpenSSL - For cryptographic functions (hashing and AES encryption/decryption)
C++ Compiler - Supporting C++11 or later (g++, clang++)
Make or CMake (optional, for build automation)


project/
├── main.cpp                 # Main program entry point
├── network.cpp              # Network communication implementation
├── user.cpp                 # User management implementation
├── diffieHellmanHelper.h    # Diffie-Hellman key exchange utilities
├── Makefile                 # Build configuration
└── README.md                # This file


Installation 
Windows
1. Install MSYS2 from https://www.msys2.org/
2. Open MSYS2 MinGW64 terminal and install dependencies: 
pacman -Syu
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-gmp mingw-w64-x86_64-openssl make
3. Verify Installation:
g++ --version
pkg-config --modversion gmp
pkg-config --modversion openssl

Building
From the project directory, run:
make
or
mingw32-make

This should compile and create crypto_project.exe

Run the program in cmd/powershell:
.\crypto_project.exe

The program will simulate communication between Alice and Bob over insecure network

To Clean:
make clean
or
minge32-make clean


Generating a new prime:
To use a different prime, change the value of SAFE_PRIME in network.h to 0 (comment the hex value and uncomment the code // "0", 16). 
To adjust the size of the new prime to be generated change the value of attribute n_bit of Network in network.h attribute. 
Recompile and the program should generate a new safe prime of specified size. 
After generation the program will output the hex of the newly generated prime, to reuse this number simply copy the number and paste it to the
SAFE_PRIME value