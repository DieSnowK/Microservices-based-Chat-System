main: main.cc
	g++ -o $@ $^ -std=c++17 -lfmt -lspdlog -lcurl -lcrypto /usr/local/lib/libjsoncpp.so.1.8.4 -g

.PHONY:clean
clean:
	rm main