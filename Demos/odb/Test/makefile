main : main.cc person-odb.cxx
	c++ -o $@ $^ -lodb-mysql -lodb -lodb-boost

.PHONY:clean
clean:
	rm main