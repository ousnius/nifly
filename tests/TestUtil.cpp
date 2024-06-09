#include "TestUtil.hpp"
#include <fstream>
#include <catch2/catch.hpp>

bool CompareBinaryFiles(const std::filesystem::path& fileName1, const std::filesystem::path& fileName2) {
	// Open file streams at the end of each file
	std::ifstream fileStream1(fileName1, std::ifstream::ate | std::ifstream::binary);
	std::ifstream fileStream2(fileName2, std::ifstream::ate | std::ifstream::binary);

	// Compare file size to start with
	std::streampos fileSize1 = fileStream1.tellg();
	std::streampos fileSize2 = fileStream2.tellg();

	UNSCOPED_INFO("File size '" << fileName1.filename() << "': " << fileSize1);
	UNSCOPED_INFO("File size '" << fileName2.filename() << "': " << fileSize2);

	if (fileSize1 != fileSize2)
		return false;

	// Rewind to the beginning of each stream
	fileStream1.seekg(0);
	fileStream2.seekg(0);

	std::istreambuf_iterator<char> fileBegin1(fileStream1);
	std::istreambuf_iterator<char> fileBegin2(fileStream2);

	// Compare binary contents
	return std::equal(fileBegin1, std::istreambuf_iterator<char>(), fileBegin2);
}
