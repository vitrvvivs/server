#include "filecache.h"
#include <string>
#include <fstream>
#include <iostream>

#define CACHESIZE 1048576 // 1MB max file size to be cached
std::string* FileCache::get(std::string filename)
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
		return loadfile(&filename);
	}
	else
	{
		std::cout << "filecache::get: cache hit: " << filename << std::endl;
		return files[filename].contents;
	}
}
std::string* FileCache::loadfile(std::string* filename)
{
	std::string* contents = new std::string;
	int size;

	std::string abs_filename = root;
	abs_filename.append(filename->begin(), filename->end());

	std::ifstream in(abs_filename.c_str(), std::ios_base::in | std::ios::binary);
	if ((in.rdstate() & std::ifstream::failbit))
	{
		std::cerr << "filecache::loadfile failbit" << std::endl;
		return 0;
	}
	in.seekg(0, std::ios::end);
	size = in.tellg();
	if (size < 0)
	{
		std::cerr << "filecache::loadfile tellg < 0: " << std::endl;
		return 0;
	}
	contents->resize(size);
	in.seekg(0, std::ios::beg);
	in.read(&(*contents)[0], contents->size());
	in.close();

	if (size < CACHESIZE)
	{
		std::cout << "filecache::loadfile: caching: " << *filename << std::endl;
		File f(*filename, size, contents);
		files[*filename] = f;
	}
	else
	{
		std::cout << "filecache::loadfile: too large to cache: "
			<< *filename << std::endl;
	}

	return contents;
}

