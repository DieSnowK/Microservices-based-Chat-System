main: main.cc
	g++ -o $@ $^ -std=c++17 -lcpr -lelasticlient -ljsoncpp -lspdlog -lfmt -lgflags

.PHONY:clean
clean:
	rm main
