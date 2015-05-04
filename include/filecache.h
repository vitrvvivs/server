#include <string>
#include <unordered_map>

#ifndef FILECACHE_H
#define FILECACHE_H
struct File
{
	std::string name;
	int size;
	std::string* contents;

	File(){;
		name = "unitialized file";
		size = 32;
		contents = 0;}
	File(std::string a, int b, std::string* c):
		name{a}, size{b}, contents{c} {};
};

class FileCache
{
	public:
		FileCache(){
			root = "/srv/";}
		FileCache(std::string a):
			root{a} {};
		std::string* get(std::string);
	private:
		std::string root;
		std::string* loadfile(std::string*);
		std::unordered_map<std::string, int> freq;
		std::unordered_map<std::string, File> files;
		std::string naf_error = "not a file";
};
#endif
