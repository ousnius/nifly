/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#pragma once

#include "NifUtil.hpp"

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

} // namespace nifly