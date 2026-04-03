#include "TestUtil.hpp"
#include <fstream>
#include <array>
#include <cstdint>
#include <iomanip>
#include <catch2/catch.hpp>

bool CompareBinaryFiles(const std::filesystem::path& fileName1, const std::filesystem::path& fileName2) {
	// Open file streams at the end of each file
	std::ifstream fileStream1(fileName1, std::ifstream::ate | std::ifstream::binary);
	std::ifstream fileStream2(fileName2, std::ifstream::ate | std::ifstream::binary);

	if (!fileStream1.is_open() || !fileStream2.is_open()) {
		UNSCOPED_INFO("Failed to open file(s): '" << fileName1 << "' or '" << fileName2 << "'.");
		return false;
	}

	// Compare file size to start with
	std::streampos fileSize1 = fileStream1.tellg();
	std::streampos fileSize2 = fileStream2.tellg();

	UNSCOPED_INFO("File size '" << fileName1.filename() << "': " << fileSize1);
	UNSCOPED_INFO("File size '" << fileName2.filename() << "': " << fileSize2);

	if (fileSize1 != fileSize2)
		return false;

	if (fileSize1 < 0 || fileSize2 < 0) {
		UNSCOPED_INFO("Failed to read file size from stream(s).\n");
		return false;
	}

	// Rewind to the beginning of each stream
	fileStream1.seekg(0);
	fileStream2.seekg(0);

	if (!fileStream1.good() || !fileStream2.good()) {
		UNSCOPED_INFO("Failed to seek stream(s) to the beginning.\n");
		return false;
	}

	// Compare binary contents in fixed-size blocks and report the first mismatch.
	std::array<unsigned char, 4096> buffer1{};
	std::array<unsigned char, 4096> buffer2{};
	std::uintmax_t compared = 0;

	while (fileStream1 && fileStream2) {
		fileStream1.read(reinterpret_cast<char*>(buffer1.data()), static_cast<std::streamsize>(buffer1.size()));
		fileStream2.read(reinterpret_cast<char*>(buffer2.data()), static_cast<std::streamsize>(buffer2.size()));

		const std::streamsize read1 = fileStream1.gcount();
		const std::streamsize read2 = fileStream2.gcount();

		if (read1 != read2) {
			UNSCOPED_INFO("Read size mismatch after " << compared << " bytes: " << read1 << " vs " << read2 << '.');
			return false;
		}

		for (std::streamsize i = 0; i < read1; ++i) {
			if (buffer1[static_cast<size_t>(i)] != buffer2[static_cast<size_t>(i)]) {
				UNSCOPED_INFO(
					"First byte mismatch at offset " << (compared + static_cast<std::uintmax_t>(i))
					<< ": 0x" << std::hex << std::setw(2) << std::setfill('0')
					<< static_cast<unsigned int>(buffer1[static_cast<size_t>(i)])
					<< " vs 0x" << std::setw(2)
					<< static_cast<unsigned int>(buffer2[static_cast<size_t>(i)]) << std::dec
				);
				return false;
			}
		}

		compared += static_cast<std::uintmax_t>(read1);

		if (read1 == 0) {
			break;
		}
	}

	return true;
}
