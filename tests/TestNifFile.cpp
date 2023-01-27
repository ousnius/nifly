#define CATCH_CONFIG_MAIN

#include "TestNifFile.hpp"
#include "TestUtil.hpp"

#include <catch2/catch.hpp>

#include <NifFile.hpp>

using namespace nifly;

const std::string nifSuffix = ".nif";
const std::string folderInput = "input";
const std::string folderOutput = "output";
const std::string folderExpected = "expected";

std::tuple<std::string, std::string, std::string> GetNifFileTuple(const char* fileName) {
	std::string fileInput = folderInput + "/" + fileName + nifSuffix;
	std::string fileOutput = folderOutput + "/" +  + fileName + nifSuffix;
	std::string fileExpected = folderExpected + "/" +  + fileName + nifSuffix;
	return std::make_tuple(fileInput, fileOutput, fileExpected);
}

TEST_CASE("Load not existing file", "[NifFile]") {
	constexpr auto fileName = "not_existing.nif";

	NifFile nif;
	REQUIRE(nif.Load(fileName) != 0);
}

TEST_CASE("Load and save static file (SE)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Static_SE";
	const auto [fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save static file (FO4)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Static_FO4";
	const auto [fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save static file (FO4, Version 132)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Static_FO4_132";
	const auto [fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save static file (FO4, Version 139)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Static_FO4_139";
	const auto [fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save skinned file (OB)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Skinned_OB";
	const auto [fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save skinned file (SE)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Skinned_SE";
	const auto [fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save skinned, dynamic file (SE)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Skinned_Dynamic_SE";
	const auto[fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save file without weights in NiSkinData", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Skinned_NoNiSkinDataWeights";
	const auto [fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save skinned file (FO4)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Skinned_FO4";
	const auto [fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save furniture file (SE)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Furniture_Col_SE";
	const auto [fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save file with loose blocks (SE)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_LooseBlocks_SE";
	const auto [fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save file with multi bound node (SE)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_MultiBound_SE";
	const auto[fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save animated file (LE)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Animated_LE";
	const auto [fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save file with deep scene graph (SE)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_DeepGraph_SE";
	const auto [fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load, optimize (LE to SE) and save file", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Optimize_LE_to_SE";
	const auto [fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	OptOptions options;
	options.targetVersion = NiVersion::getSSE();

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	nif.OptimizeFor(options);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load, optimize (LE to SE, dynamic) and save file", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Optimize_Dynamic_LE_to_SE";
	const auto[fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	OptOptions options;
	options.targetVersion = NiVersion::getSSE();
	options.headParts = true;

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	nif.OptimizeFor(options);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load, optimize (SE to LE) and save file", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Optimize_SE_to_LE";
	const auto [fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	OptOptions options;
	options.targetVersion = NiVersion::getSK();

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	nif.OptimizeFor(options);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save file with ordered node (SE)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_OrderedNode_SE";
	const auto [fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	OptOptions options;
	options.targetVersion = NiVersion::getSK();

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load, optimize (SE to LE, dynamic) and save file", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Optimize_Dynamic_SE_to_LE";
	const auto[fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	OptOptions options;
	options.targetVersion = NiVersion::getSK();
	options.headParts = true;

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	nif.OptimizeFor(options);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save file with non-zero index root node", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_RootNonZero";
	const auto [fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save file (FO76)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_FO76";
	const auto [fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("FixBSXFlags (remove external emittance)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_FixBSXFlags_RemoveExtEmit";
	const auto [fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	nif.FixBSXFlags();
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("FixBSXFlags (add external emittance)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_FixBSXFlags_AddExtEmit";
	const auto [fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	nif.FixBSXFlags();
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("FixShaderFlags (remove environment mapping)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_FixShaderFlags_RemoveEnvMap";
	const auto [fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	nif.FixShaderFlags();
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("FixShaderFlags (add environment mapping)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_FixShaderFlags_AddEnvMap";
	const auto [fileInput, fileOutput, fileExpected] = GetNifFileTuple(fileName);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	nif.FixShaderFlags();
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}
