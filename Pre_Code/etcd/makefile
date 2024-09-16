all: registry discovery

registry: registry.cc
	g++ -o $@ $^ -std=c++17 -lspdlog -lfmt -lgflags -letcd-cpp-api -lcpprest

discovery: discovery.cc
	g++ -o $@ $^ -std=c++17 -lspdlog -lfmt -lgflags -letcd-cpp-api -lcpprest

.PHONT:clean
clean:
	rm registry discovery