main : main.cc
	g++ -o $@ $^ -std=c++17 -lhiredis -lredis++ -lgflags -I../../../common/

.PHONY:clean
clean:
	rm main