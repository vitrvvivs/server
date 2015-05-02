#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <cstring>
#include <csignal>
#include <unordered_map>
#include <vector>
#include <ctime>

#include <netdb.h>
#include <unistd.h>

#define PORT "3000"
#define BACKLOG 20
#define BUFSIZE 4096 // 4KB max request size
#define CACHESIZE 1048576 // 1MB max file size to be cached

void prevent(bool status, const char *message)
{
	if (status)
	{
		std::cerr << status << message << std::endl;
		exit(1);
	}
}

int setup_server()
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

std::unordered_map<int, std::string> headers;
void define_http_headers()
{
	headers[200] = "HTTP/1.1 200 OK\n\n";
	headers[404] = "HTTP/1.1 404 File not Found\n\n";
}

std::string read_request(int sockfd)
{
	int bufsize = BUFSIZE;

	int received = 0;
	std::vector<char> buffer(bufsize);
	std::string request;
	do
	{
		memset(buffer.data(), 0, bufsize);
		received = recv(sockfd, buffer.data(), bufsize, 0);
		if (received < 0) std::cerr << "ERROR read request" << std::endl;
		request.append(buffer.cbegin(), buffer.cend());
	} while (received == bufsize);
	return request;
}

struct file
{
	std::string name;
	int size;
	std::string* contents;

	file(){;
		name = "unitialized file";
		size = 32;
		contents = 0;}
	file(std::string a, int b, std::string* c):
		name{a}, size{b}, contents{c} {};
};

class filecache
{
	public:
		std::string* get(std::string filename);
	private:
		std::string* loadfile(std::string);
		std::unordered_map<std::string, int> freq;
		std::unordered_map<std::string, file> files;
		std::string naf_error = "not a file";
};
std::string* filecache::get(std::string filename)
{
	if (freq.find(filename) == freq.end())
		freq[filename] = 1;
	else
		freq[filename] += 1;

	// if cache is too large
	// delete large, usused files

	if (files.find(filename) == files.end())
	{
		std::cout << "filecache::get: cache miss: " << filename << std::endl;
		return loadfile(filename);
	}
	else
	{
		std::cout << "filecache::get: cache hit: " << filename << std::endl;
		return files[filename].contents;
	}
}
std::string* filecache::loadfile(std::string filename)
{
	std::string *contents = new std::string;
	int size;
	std::ifstream in(filename.c_str(), std::ios_base::in | std::ios::binary);
	if ((in.rdstate() & std::ifstream::failbit))
		std::cerr << in.rdstate() << std::endl;
		// return &naf_error;
	in.seekg(0, std::ios::end);
	size = in.tellg();
	contents->resize(size);
	in.seekg(0, std::ios::beg);
	in.read(&(*contents)[0], contents->size());
	in.close();

	if (size < CACHESIZE)
	{
		std::cout << "filecache::loadfile: caching: " << filename << std::endl;
		file f(filename, size, contents);
		files[filename] = f;
	}
	else
	{
		std::cout << "filecache::loadfile: too large to cache: "<< filename << std::endl;
	}

	return contents;
}
filecache fc;

void send_static_file(std::string filename, int sockfd)
{
	std::string* headbuffer = &headers[200];
	std::string* filebuffer = fc.get(filename);

	send(sockfd, headbuffer->data(), headbuffer->length(), 0);
	send(sockfd, filebuffer->data(), filebuffer->length(), 0);
}

int main(int argc, char *argv[])
{
	define_http_headers();
	int sockfd = setup_server();
	sockaddr_storage client_addr;
	socklen_t addr_size = sizeof(client_addr);
	int clientfd;
	std::clock_t start_time;

	while (true)
	{
		clientfd = accept(sockfd, (sockaddr *)&client_addr, &addr_size);
		start_time = std::clock();
		std::string request = read_request(clientfd);
		send_static_file("pripyat.jpg", clientfd);
		std::cout << "request took "
				<< (std::clock() - start_time) / (double)(CLOCKS_PER_SEC / 1000)
				<< " ms" << std::endl << std::endl;
		close(clientfd);
	}
    return 0;
}
