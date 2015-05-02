#include "filecache.h"
#include <string>
#include <unordered_map>

#ifndef SERVER_H
#define SERVER_H
class Server
{
	public:
		Server();
		void start();
	private:
		int sockfd;
		FileCache fc;
		std::unordered_map<int, std::string> headers;

		int init_socket();
		void init_headers();
		std::string accept_request(int);
		void send_static_file(std::string, int);
};
#endif
