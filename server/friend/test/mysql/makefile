CFLAGS = -I/usr/include/mysql -I../../../odb/ -I../../../common -I./

main : main.cc friend_apply-odb.cxx  relation-odb.cxx chat_session_member-odb.cxx chat_session-odb.cxx
	g++ -o $@ $^ -std=c++17 $(CFLAGS) -lodb-mysql  -lodb -lodb-boost -lfmt -lspdlog -lgflags

.PHONY:clean
clean:
	rm main