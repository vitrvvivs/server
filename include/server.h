#include "filecache.h"
#include "request.h"
#include <string>
#include <unordered_map>

#ifndef SERVER_H
#define SERVER_H
class Server
{
	public:
		Server(std::string);
		void start(int);
	private:
		int sockfd;
		FileCache fc;
		std::unordered_map<int, std::string> headers;

		int init_socket();
		void init_headers();
		Request accept_request(int);
		void send_static_file(std::string, int);
};
#endif
