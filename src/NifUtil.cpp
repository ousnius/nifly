/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#pragma once

#include "NifUtil.hpp"

#include <fstream>

namespace nifly {

void trim_whitespace(std::string& str) {
	if (str.empty())
		return;

	std::string::size_type i, j;
	i = 0;

	while (i < str.size() && isspace(str[i]))
		++i;

	if (i == str.size()) {
		str.clear();
		return;
	}

	j = str.size() - 1;

	while (isspace(str[j]))
		--j;

	str = str.substr(i, j - i + 1);
}

std::unique_ptr<std::istream> GetBinaryInputFileStream(const std::filesystem::path& path) {
	if (std::filesystem::exists(path)) {
		auto fileStream = std::make_unique<std::ifstream>(path, std::ios::in | std::ios::binary);
		if (fileStream && !fileStream->fail())
			return fileStream;
	}

	return nullptr;
}

std::unique_ptr<std::ostream> GetBinaryOutputFileStream(const std::filesystem::path& path) {
	auto fileStream = std::make_unique<std::ofstream>(path, std::ios::out | std::ios::binary);
	if (fileStream && !fileStream->fail())
		return fileStream;

	return nullptr;
}

} // namespace nifly