main : main.cc student-odb.cxx
	c++ -o $@ $^ -lodb-mysql -lodb -lodb-boost -lgflags

.PHONY:clean
clean:
	rm main