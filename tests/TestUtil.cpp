#include "TestUtil.hpp"
#include <fstream>

bool CompareBinaryFiles(const std::filesystem::path& fileName1, const std::filesystem::path& fileName2) {
	// Open file streams at the end of each file
	std::ifstream fileStream1(fileName1, std::ifstream::ate | std::ifstream::binary);
	std::ifstream fileStream2(fileName2, std::ifstream::ate | std::ifstream::binary);

	// Compare file size to start with
	if (fileStream1.tellg() != fileStream2.tellg())
		return false;

	// Rewind to the beginning of each stream
	fileStream1.seekg(0);
	fileStream2.seekg(0);

	std::istreambuf_iterator<char> fileBegin1(fileStream1);
	std::istreambuf_iterator<char> fileBegin2(fileStream2);

	// Compare binary contents
	return std::equal(fileBegin1, std::istreambuf_iterator<char>(), fileBegin2);
}
