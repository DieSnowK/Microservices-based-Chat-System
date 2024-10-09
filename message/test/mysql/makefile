CFLAGS = -I/usr/include/mysql -I../../../odb/ -I../../../common -I./ 
main : main.cc message-odb.cxx
	g++ -o $@ $^ -std=c++17 $(CFLAGS) -lodb-mysql -lodb -lodb-boost -lfmt -lspdlog -lgflags

.PHONY:clean
clean:
	rm main