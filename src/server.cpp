#include "server.h"
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

std::string Server::accept_request(int sockfd)
{
	int bufsize = BUFSIZE;

	int received = 0;
	std::string request;
	do
	{
		request.reserve(request.size() + bufsize);
		received = recv(sockfd, &request[request.size()], bufsize, 0);
		if (received < 0) std::cerr << "ERROR read request" << std::endl;
	} while (received == bufsize);
	request.shrink_to_fit();
	std::cout << request << std::endl;
	return request;
}

void Server::send_static_file(std::string filename, int sockfd)
{
	std::string* headbuffer = &headers[200];
	std::string* filebuffer = fc.get(filename);

	send(sockfd, headbuffer->data(), headbuffer->length(), 0);
	send(sockfd, filebuffer->data(), filebuffer->length(), 0);
}

Server::Server()
{
	init_headers();
	sockfd = init_socket();
}

void Server::start()
{
	std::clock_t start_time;
	sockaddr_storage client_addr;
	socklen_t addr_size = sizeof(client_addr);
	int clientfd;

	while (true)
	{
		clientfd = accept(sockfd, (sockaddr *)&client_addr, &addr_size);
		start_time = std::clock();
		std::string request = accept_request(clientfd);
		send_static_file("/home/gekkey/cpp/server/www/index.html", clientfd);
		std::cout << "request took "
				<< (std::clock() - start_time) / (double)(CLOCKS_PER_SEC / 1000)
				<< " ms" << std::endl << std::endl;
		close(clientfd);
	}
}
