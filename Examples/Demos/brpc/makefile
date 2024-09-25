all: server client_sync client_async

server: server.cc main.pb.cc
	g++ -o $@ $^ -std=c++17 -lbrpc -lgflags -lssl -lcrypto -lprotobuf -lleveldb

client_sync: client_sync.cc main.pb.cc
	g++ -o $@ $^ -std=c++17 -lbrpc -lgflags -lssl -lcrypto -lprotobuf -lleveldb

client_async: client.async.cc main.pb.cc
	g++ -o $@ $^ -std=c++17 -lbrpc -lgflags -lssl -lcrypto -lprotobuf -lleveldb

.PHONY:clean
clean:
	rm server client_sync  client_async