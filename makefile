CC=g++ -std=c++11 -Wall -L.

all: server

server: server.cpp
	$(CC) server.cpp -Wl,-rpath=. -o server -lstdc++

clean:
	rm -f *.o *.so server
