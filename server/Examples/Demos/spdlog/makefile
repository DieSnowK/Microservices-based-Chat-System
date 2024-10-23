all:sync_stdout sync_file async

sync_stdout:sync_stdout.cc
	g++ -o $@ $^ -std=c++17 -lspdlog -lfmt

sync_file:sync_file.cc
	g++ -o $@ $^ -std=c++17 -lspdlog -lfmt

async:async.cc
	g++ -o $@ $^ -std=c++17 -lspdlog -lfmt

.PHONY:clean
clean:
	rm sync_stdout
	rm sync_file
