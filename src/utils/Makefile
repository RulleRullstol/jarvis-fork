CXX = clang++
CXXFLAGS = -std=c++22 -Wall -Wextra -pedantic -O2
LDFLAGS = -lcurl

SOURCES = test.cpp webUtils.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXEC = output

all: $(EXEC)

$(EXEC): $(OBJECTS)
    $(CXX) $(OBJECTS) -o $(EXEC) $(LDFLAGS)

%.o: %.cpp
    $(CXX) -c $(CXXFLAGS) $< -o $@

clean:
    rm -f $(OBJECTS) $(EXEC)

.PHONY: all clean
