CC=g++ -std=c++11 -Wall -Llib -Iinclude -lstdc++ -lpthread
EXFLAGS=-Wl,-rpath=./lib
SOFLAGS=-shared -fPIC

all: bin/server

bin/:
	mkdir -p bin

bin/server: bin/ src/main.cpp lib/libserver.so
	$(CC) $(EXFLAGS) src/main.cpp -o bin/server -lserver

lib/:
	mkdir -p lib

lib/libserver.so: lib/ src/server.cpp lib/libfilecache.so
	$(CC) $(SOFLAGS) src/server.cpp -o lib/libserver.so -lfilecache

lib/libfilecache.so: lib/ src/filecache.cpp
	$(CC) $(SOFLAGS) src/filecache.cpp -o lib/libfilecache.so

clean:
	rm -f *.o lib/* bin/*
