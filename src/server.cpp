#include "server.h"
#include "request.h"

#include <string>
#include <ctime>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <iostream>

#define PORT "3000"
#define BACKLOG 20
#define BUFSIZE 4096 // 4KB max request size

void prevent(bool status, const char *message)
{
	if (status)
	{
		std::cerr << status << message << std::endl;
		exit(1);
	}
}

Request::Request(std::string* req_string)
{
	std::string key;
	std::string val;

	unsigned int start = 0;
	unsigned int end = req_string->find(" ");
	method = req_string->substr(start, end - start);

	start = end + 1;
	end = req_string->find(" ", start);
	action = req_string->substr(start, end - start);

	valid = (end != std::string::npos);
	return;

	end = req_string->find("\n", end);
	while (end < req_string->size())
	{
		start = end + 1;
		end = req_string->find(":", start);
		key = req_string->substr(start, end - start);

		start = end + 2;
		end = req_string->find("\n", start);
		val = req_string->substr(start, end - start);

		properties[key] = val;
	}
}
std::string* Request::get(std::string key)
{
	if (key == "method")
		return &method;
	else if (key == "action")
		return &action;
	else
		return &(properties[key]);
}

int Server::init_socket()
{
	// getaddrinfo()
	// socket()
	// bind()
	// listen()

	int sockfd, yes=1;
	addrinfo hints, *serv_info;
	std::memset(&hints, 0, sizeof(hints)); // ensure hints empty
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	prevent(getaddrinfo(NULL, PORT, &hints, &serv_info),
			"ERROR getaddrinfo");
	prevent((sockfd = socket(
				serv_info->ai_family,
				serv_info->ai_socktype,
				serv_info->ai_protocol)) < 0,
			"ERROR create socket");
	prevent(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)),
			"ERROR setsockopt");
	prevent(bind(sockfd, serv_info->ai_addr, serv_info->ai_addrlen),
			"ERROR bind socket");
	prevent(listen(sockfd, BACKLOG),
			"ERROR listen");

	freeaddrinfo(serv_info);
	return sockfd;
}
void Server::init_headers()
{
	headers[200] = "HTTP/1.1 200 OK\n\n";
	headers[404] = "HTTP/1.1 404 File not Found\n\n";
}
Request Server::accept_request(int sockfd)
{
	int received = 0;
	std::string request;
	do
	{
		request.resize(request.size()+BUFSIZE);
		received = recv(sockfd, &request[request.size() - BUFSIZE], BUFSIZE, 0);
		if (received < 0)
		{
			std::cerr << "ERROR accept request" << std::endl;
			return Request(&request);
		}
	} while (received == BUFSIZE);
	request.resize(request.size() - BUFSIZE + received);
	return Request(&request);
}
void Server::send_static_file(std::string filename, int sockfd)
{
	std::string* filebuffer = fc.get(filename);
	std::string* headbuffer;
	if (!filebuffer)
	{
		headbuffer = &headers[404];
		filebuffer = fc.get("404.html");
	}
	else
	{
		headbuffer = &headers[200];
	}
	send(sockfd, headbuffer->data(), headbuffer->length(), 0);
	send(sockfd, filebuffer->data(), filebuffer->length(), 0);
}
Server::Server()
{
	init_headers();
	sockfd = init_socket();
	fc = FileCache("/home/gekkey/cpp/server/www");
}

void Server::start(int threadno)
{
	std::clock_t start_time;
	sockaddr_storage client_addr;
	socklen_t addr_size = sizeof(client_addr);
	int clientfd;

	std::cout << "Listener thread started\n";
	while (true)
	{
		clientfd = accept(sockfd, (sockaddr *)&client_addr, &addr_size);
		start_time = std::clock();
		Request request = accept_request(clientfd);
		std::cout << "Thread " << threadno << ": " << *request.get("method") << " " << *request.get("action") << std::endl;
		send_static_file(*request.get("action"), clientfd);
		std::cout << "response took "
				<< (std::clock() - start_time) / (double)(CLOCKS_PER_SEC / 1000)
				<< " ms" << std::endl << std::endl;
		close(clientfd);
	}
}
