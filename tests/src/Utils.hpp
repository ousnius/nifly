#include "ApprovalTests.v.10.8.0.hpp"
#include <catch2/catch.hpp>

#include <NifFile.hpp>

using namespace nifly;
namespace fs = std::filesystem;
using namespace ApprovalTests;

template<>
std::string ApprovalTests::StringMaker::toString(const NifFile& contents) {
	std::stringstream stream;
	auto copy = contents; // NifFile::Save is not const qualified
	copy.Save(stream);
	return stream.str();
}


template<>
std::string ApprovalTests::StringMaker::toString(const std::vector<std::string>& contents) {
	std::string res = "[";
	res.reserve(100 * contents.size());
	for (const auto& str : contents)
		res += str + "|";
	return res + "]";
}

template<>
std::string ApprovalTests::StringMaker::toString(const OptResult& contents) {
	std::string res;
	res.reserve(10000);

	res += "versionMismatch: " + toString(contents.versionMismatch) + '\n';
	res += "dupesRenamed: " + toString(contents.dupesRenamed) + '\n';
	res += "shapesVColorsRemoved: " + toString(contents.shapesVColorsRemoved) + '\n';
	res += "shapesNormalsRemoved: " + toString(contents.shapesNormalsRemoved) + '\n';
	res += "shapesPartTriangulated: " + toString(contents.shapesPartTriangulated) + '\n';
	res += "shapesTangentsAdded: " + toString(contents.shapesTangentsAdded) + '\n';
	res += "shapesParallaxRemoved: " + toString(contents.shapesParallaxRemoved) + '\n';

	return res;
}

static const auto directoryDisposer = Approvals::useApprovalsSubdirectory("../data/approval_tests");
