main: main.cc
	g++ -o $@ $^ -std=c++17 -lcurl -lcrypto /usr/lib/x86_64-linux-gnu/libjsoncpp.so.1.8.4

.PHONY:clean
clean:
	rm main