all: main

main: main.cc
	g++ -o $@ $^ -std=c++17 -lbrpc -lgflags -lssl -lcrypto -lprotobuf -lleveldb -lspdlog -lfmt