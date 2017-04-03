#include "server.h"
#include <thread>
#include <vector>
#include <iostream>
#include <unistd.h>

#define NUM_THREADS 4

int main(int argc, char *argv[])
{
	std::string server_root("./");
	if (argc > 1)
	{
		server_root.assign(argv[1]);
	}

	std::vector<std::thread> pool;
	Server server(server_root);
	for (int i = 0; i < NUM_THREADS; ++i)
	{
		pool.push_back(std::thread([&server, i](){
			server.start(i);
		}));
	}
	for (int i = 0; i < NUM_THREADS; ++i)
	{
		pool[i].join();
	}
    return 0;
}
