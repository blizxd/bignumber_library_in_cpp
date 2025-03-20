CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic -O3

OBJ_DIR = build/obj
BIN_DIR = build/bin

SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRCS))
HEADERS = $(wildcard *.h)

EXECUTABLE = $(BIN_DIR)/main.exe

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CXX) -o $@ $^

$(OBJ_DIR)/%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)/* $(BIN_DIR)/*