CXX = g++
CXXFLAGS = -Wall -std=c++17 -Ishared

PREFIX ?= $(MINGW_PREFIX)
ifeq ($(PREFIX),)
    PREFIX := /mingw64
endif

CXXFLAGS += -I$(PREFIX)/include
LDFLAGS  += -L$(PREFIX)/lib -lgmpxx -lgmp -lssl -lcrypto -lws2_32

DEBUG ?= 0

ifeq ($(DEBUG),1)
    CXXFLAGS += -g -O0
else
    CXXFLAGS += -O2
endif


MAIN_EXE = crypto_project.exe

$(MAIN_EXE): main.cpp network.cpp user.cpp diffieHellmanHelper.h
	$(CXX) $(CXXFLAGS) main.cpp network.cpp user.cpp -o $(MAIN_EXE) $(LDFLAGS)


clean:
ifeq ($(OS),Windows_NT)
	del /Q $(MAIN_EXE)
else
	rm -f $(MAIN_EXE)
endif
