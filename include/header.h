#include <string>
#include <unordered_map>

#ifndef HEADER_H
#define HEADER_H
struct Header
{
	public:
		Header();
		Header(int);
		std::string* get(std::string);
		std::string str();

		int response_code;
		int content_length;
		int size();
	private:
		std::string _str;
		std::unordered_map<int, std::string> resp_codes = 
		{
			{200, "200 OK"},
			{400, "400 Bad Request"},
			{404, "404 Not Found"},
		};
};
#endif
