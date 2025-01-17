all: publish consume
publish: publish.cc
	g++ -o $@ $^ -lamqpcpp -lev -lfmt -lspdlog -lgflags -std=c++17
consume: consume.cc
	g++ -o $@ $^ -lamqpcpp -lev -lfmt -lspdlog -lgflags -std=c++17

.PHONY:clean
clean:
	rm publish consume