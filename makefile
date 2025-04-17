# Compiler and flags
CC := g++
CFLAGS := -g -fexceptions -Wall -Wextra -pedantic -std=c++11 
LDFLAGS := 

# Directories
SRC_DIR := .
OBJ_DIR := build/obj
BIN_DIR := build/bin

HEADERS := $(wildcard $(SRC_DIR)/*.h) $(wildcard $(SRC_DIR)/*.hpp)

all : $(BIN_DIR)/demo.exe $(BIN_DIR)/test.exe

#Demo
$(BIN_DIR)/demo.exe : $(OBJ_DIR)/demo.o $(OBJ_DIR)/bigNumber.o $(OBJ_DIR)/bigNumFn.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

#Test
$(BIN_DIR)/test.exe : $(OBJ_DIR)/test.o $(OBJ_DIR)/bigNumber.o $(OBJ_DIR)/bigNumFn.o
	$(CC)  $(CFLAGS) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

runDemo:
	$(BIN_DIR)/demo.exe

runTest:
	$(BIN_DIR)/test.exe

clean :
	rm -rf $(OBJ_DIR)/* $(BIN_DIR)/*

.phony : clean all