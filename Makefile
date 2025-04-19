CXX=g++
CXXFLAGS=-std=c++17 -Iinclude -Wall -g

all: writer reader

writer: writer.cpp
	$(CXX) $(CXXFLAGS) writer.cpp -o writer

reader: reader.cpp
	$(CXX) $(CXXFLAGS) reader.cpp -o reader

clean:
	rm -f writer reader
