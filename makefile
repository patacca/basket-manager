CXX=g++
LIB=-lsqlite3

bm: main.cpp match.cpp
	$(CXX) -o bm main.cpp match.cpp $(LIB)
