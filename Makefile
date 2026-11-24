# Build with:  make        (program)   |   make test   (self checks)   |   make clean
CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
SRC      = src/triad.cpp src/nullmodel.cpp src/chart.cpp

all: motifs

motifs: src/main.cpp $(SRC)
	$(CXX) $(CXXFLAGS) -o motifs src/main.cpp $(SRC)

test: tests/test_triad.cpp $(SRC)
	$(CXX) $(CXXFLAGS) -o test_triad tests/test_triad.cpp $(SRC)
	./test_triad

clean:
	rm -f motifs motifs.exe test_triad test_triad.exe
