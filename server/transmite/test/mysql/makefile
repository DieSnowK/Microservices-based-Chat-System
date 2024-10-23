main : main.cc chat_session_member-odb.cxx user-odb.cxx
	c++ -o $@ $^ -std=c++17 -I../../../odb/ -I../../../common/ -I./ -lodb-mysql -lodb -lodb-boost -lfmt -lspdlog -lgflags
%.cxx:
	odb -d mysql --std c++11 --generate-query --generate-schema --profile boost/date-time ../../../odb/*.hpp

.PHONY:clean
clean:
	rm main