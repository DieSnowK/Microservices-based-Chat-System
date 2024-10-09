main : main.cc
	c++ -o $@ $^ -I../../../odb/ -I../../../common/ -I./ -lfmt -lspdlog -lgflags -lcpr -lelasticlient /usr/lib/x86_64-linux-gnu/libjsoncpp.so.19

.PHONY:clean
clean:
	rm main