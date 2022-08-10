#include "xTea.h"

#include <string>
#include <fstream>
#include <iterator>
#include <string>
#include <algorithm>
#include <cstdio>
#include <bitset>


bool compareFiles(const std::string &p1, const std::string &p2)
{
	std::ifstream f1(p1, std::ifstream::binary | std::ifstream::ate);
	std::ifstream f2(p2, std::ifstream::binary | std::ifstream::ate);

	if (f1.fail() || f2.fail())
	{
		return false; // file problem
	}

	if (f1.tellg() != f2.tellg())
	{
		return false; // size mismatch
	}

	// seek back to beginning and use std::equal to compare contents
	f1.seekg(0, std::ifstream::beg);
	f2.seekg(0, std::ifstream::beg);
	return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
					  std::istreambuf_iterator<char>(),
					  std::istreambuf_iterator<char>(f2.rdbuf()));
}

std::string hex_string(unsigned short length)
{
	static char hex_characters[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	std::string out;
	out.resize(length);
	for (unsigned short i = 0; i < length; i++)
		out[i] = hex_characters[rand() % 16];
	return out;
}

int main()
{
	dummy();

	std::fstream plain_file;
	plain_file.open("plain_file.txt", std::ios::out);
	if (!plain_file)
		return false;

	plain_file << hex_string(512);
	plain_file.close();

	const char *chiavestr = "incomprehensible"; // 128 bit
	const uint32_t *k = (uint32_t *)(&chiavestr[0]);

	xTea xtea{};
	printf("Encode\n");
	int setup = xtea.Setup("plain_file.txt", "encripted_file.bin", k);
	if (setup == 0)
	{
		if (xtea.Encode(false))
			printf("	OK\n");
		else
		{
			printf("	Error while encoding\n");
		}
	}
	else
	{
		printf("	Error while loading xtea %d\n", setup);
		return false;
	}

	printf("Decode\n");
	setup = xtea.Setup("encripted_file.bin", "replain_file.txt", k);
	if (setup == 0)
	{
		if (xtea.Decode(false))
			printf("	OK\n");
		else
		{
			printf("	Error while encoding\n");
		}
	}
	else
	{
		printf("	Error while loading xtea %d\n", setup);
		return false;
	}

	if (compareFiles("plain_file.txt", "replain_file.txt"))
	{
		printf("It works!\n");
	}
	else
	{
		std::cerr << "It does not work!\n";
		return false;
	}

	printf("removing temporary files\n");
	int x = (remove("plain_file.txt")!=0) + 2*(remove("replain_file.txt")!=0) + 4*(remove("encripted_file.bin")!=0);
	if (x == 0)
		printf("	OK\n");
	else
		std::cout << "	Something went wrong, but it's not important " << std::bitset<3>(x)  << std::endl;

	return 0;
}
