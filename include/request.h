#include <string>
#include <unordered_map>

#ifndef REQUEST_H
#define REQUEST_H
struct Request
{
	public:
		Request(std::string*);
		std::string* get(std::string);
		bool valid;
	private:
		std::string method;
		std::string action;
		std::unordered_map<std::string, std::string> properties;
};
#endif
