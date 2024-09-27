main : main.cc
	g++ -o $@ $^ -std=c++17 -I../../../common/ -I../../../odb/ -lfmt -lspdlog -lgflags -lcpr -lelasticlient /usr/lib/x86_64-linux-gnu/libjsoncpp.so.19

.PHONY:clean
clean:
	rm main