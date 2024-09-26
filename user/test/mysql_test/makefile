main : main.cc user-odb.cxx
	g++ -o $@ $^ -std=c++17 -I../../../odb/ -I../../../common/ -I./ -lodb-mysql -lodb -lodb-boost -lfmt -lspdlog -lgflags

.PHONY:clean
clean:
	rm main