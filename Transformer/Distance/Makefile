CC=g++

hello: distance.o 
	$(CC) -o distance distance.o 
hello.o: distance.cpp
	$(CC) -c distance.cpp
clean:
	- rm -f distance.o distance