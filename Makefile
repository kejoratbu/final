# Simple Makefile for INVENTORY-MANAGER (Standalone)

CXX := "C:\Program Files (x86)\Embarcadero\Dev-Cpp\TDM-GCC-64\bin\g++.exe"
CXXFLAGS := -std=c++17 -Wall -Wextra

SRCS := main.cpp
BIN := inventory.exe
TEST_SRC := tests/unit_tests.cpp
TEST_BIN := tests/runner.exe

.PHONY: all build run test clean

all: build

build: $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(BIN)

run: build
	./$(BIN)

test:
	$(CXX) $(CXXFLAGS) $(TEST_SRC) -o $(TEST_BIN)
	./$(TEST_BIN)

clean:
	del $(BIN) $(TEST_BIN) 2>NUL
