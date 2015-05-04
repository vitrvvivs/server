#include "server.h"
#include <thread>
#include <vector>
#include <iostream>
#include <unistd.h>

#define NUM_THREADS 2

int main(int argc, char *argv[])
{
	std::vector<std::thread> pool;
	Server server;
	for (int i = 0; i < NUM_THREADS; ++i)
	{
		pool.push_back(std::thread([&server](){
			server.start();
		}));
	}
	for (int i = 0; i < NUM_THREADS; ++i)
	{
		pool[i].join();
	}
    return 0;
}
