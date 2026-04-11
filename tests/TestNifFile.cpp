#define CATCH_CONFIG_MAIN

#include "TestNifFile.hpp"
#include "TestUtil.hpp"

#include <catch2/catch.hpp>

#include <NifFile.hpp>
#include <NifUtil.hpp>

using namespace nifly;

const std::string nifSuffix = ".nif";
const std::string meshSuffix = ".mesh";

const std::string folderInput = "input";
const std::string folderOutput = "output";
const std::string folderExpected = "expected";

std::tuple<std::string, std::string, std::string> GetFileTuple(const char* fileName, const std::string& suffix) {
	std::string fileInput = folderInput + "/" + fileName + suffix;
	std::string fileOutput = folderOutput + "/" + fileName + suffix;
	std::string fileExpected = folderExpected + "/" + fileName + suffix;
	return std::make_tuple(fileInput, fileOutput, fileExpected);
}

TEST_CASE("Load not existing file", "[NifFile]") {
	constexpr auto fileName = "not_existing.nif";

	NifFile nif;
	REQUIRE(nif.Load(fileName) != 0);
}

TEST_CASE("Load and save static file (SE)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Static_SE";
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Trim texture paths", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Static_SE";
	std::string fileInput = folderInput + "/" + fileName + nifSuffix;

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);

	auto shapes = nif.GetShapes();
	REQUIRE(!shapes.empty());

	auto shader = nif.GetShader(shapes.front());
	REQUIRE(shader);
	REQUIRE(shader->HasTextureSet());

	auto textureSet = nif.GetHeader().GetBlock<BSShaderTextureSet>(shader->TextureSetRef());
	REQUIRE(textureSet);
	REQUIRE(textureSet->textures.size() == 9);

	textureSet->textures[0].get() = " \\Data\\\\Textures//white.dds\r\n  ";
	textureSet->textures[1].get() = " ";

	nif.TrimTexturePaths();
	REQUIRE(textureSet->textures[0] == "textures\\white.dds");
	REQUIRE(textureSet->textures[1] == "");
}

TEST_CASE("Load and save static file (FO4)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Static_FO4";
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save static file (FO4, Version 132)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Static_FO4_132";
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save static file (FO4, Version 139)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Static_FO4_139";
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save skinned file (OB)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Skinned_OB";
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save skinned file (SE)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Skinned_SE";
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save skinned, dynamic file (SE)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Skinned_Dynamic_SE";
	const auto[fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save file without weights in NiSkinData", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Skinned_NoNiSkinDataWeights";
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save skinned file (FO4)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Skinned_FO4";
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save furniture file (SE)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Furniture_Col_SE";
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save file with loose blocks (SE)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_LooseBlocks_SE";
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save file with multi bound node (SE)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_MultiBound_SE";
	const auto[fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save animated file (LE)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Animated_LE";
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save file with deep scene graph (SE)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_DeepGraph_SE";
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load, optimize (LE to SE) and save file", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Optimize_LE_to_SE";
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

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
	const auto[fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

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
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

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
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

	OptOptions options;
	options.targetVersion = NiVersion::getSK();

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load, optimize (SE to LE, dynamic) and save file", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Optimize_Dynamic_SE_to_LE";
	const auto[fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

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
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save file (FO76)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_FO76";
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save file (SF)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_SF";
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);

	auto shapes = nif.GetShapes();
	REQUIRE(!shapes.empty());

	for (auto& s : shapes) {
		auto meshPaths = nif.GetExternalGeometryPathRefs(s);
		REQUIRE(!meshPaths.empty());

		uint8_t meshIndex = 0;
		for (auto meshPath : meshPaths) {
			std::string meshPathStr = meshPath.get();
			REQUIRE(!meshPathStr.empty());

			const auto [meshFileInput, meshFileOutput, meshFileExpected] = GetFileTuple(meshPathStr.c_str(), meshSuffix);
			const std::filesystem::path meshInputPath = std::filesystem::u8path(meshFileInput);
			auto meshStream = GetBinaryInputFileStream(meshInputPath);
			REQUIRE(meshStream);

			// Load external mesh data into the shape (virtual BSGeometryMeshData block).
			REQUIRE(nif.LoadExternalShapeData(s, *meshStream, meshIndex));
			meshStream.reset();

			const std::filesystem::path meshOutputPath = std::filesystem::u8path(meshFileOutput);
			auto meshOutputStream = GetBinaryOutputFileStream(meshOutputPath);
			REQUIRE(meshOutputStream);

			// Save the virtual BSGeometryMeshData block of the shape to an external .mesh file.
			REQUIRE(nif.SaveExternalShapeData(s, *meshOutputStream, meshIndex));
			meshOutputStream.reset();

			const std::filesystem::path meshExpectedPath = std::filesystem::u8path(meshFileExpected);
			REQUIRE(CompareBinaryFiles(meshOutputPath, meshExpectedPath));

			meshIndex++;
		}

		REQUIRE(meshIndex == meshPaths.size());
	}

	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load external and save as internal mesh data (SF)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_ToInternalMesh_SF";
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

	// Load NIF with external mesh references
	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);

	auto shapes = nif.GetShapes();
	REQUIRE(!shapes.empty());

	for (auto& s : shapes) {
		auto meshPaths = nif.GetExternalGeometryPathRefs(s);
		REQUIRE(!meshPaths.empty());

		uint8_t meshIndex = 0;
		for (auto meshPath : meshPaths) {
			std::string meshPathStr = meshPath.get();
			REQUIRE(!meshPathStr.empty());

			const auto [meshFileInput, meshFileOutput, meshFileExpected] = GetFileTuple(meshPathStr.c_str(), meshSuffix);
			const std::filesystem::path meshInputPath = std::filesystem::u8path(meshFileInput);
			auto meshStream = GetBinaryInputFileStream(meshInputPath);
			REQUIRE(meshStream);

			REQUIRE(nif.LoadExternalShapeData(s, *meshStream, meshIndex));
			meshStream.reset();
			meshIndex++;
		}

		// Switch from external to internal mesh data
		auto bsgeo = dynamic_cast<nifly::BSGeometry*>(s);
		REQUIRE(bsgeo);
		bsgeo->SetInternalGeomData(true);
	}

	// Save NIF with embedded mesh data
	REQUIRE(nif.Save(fileOutput) == 0);

	// Reload and verify the internal data round-trips
	NifFile nif2;
	REQUIRE(nif2.Load(fileOutput) == 0);

	auto shapes2 = nif2.GetShapes();
	REQUIRE(shapes2.size() == shapes.size());

	for (auto& s2 : shapes2) {
		auto bsgeo2 = dynamic_cast<nifly::BSGeometry*>(s2);
		REQUIRE(bsgeo2);
		REQUIRE(bsgeo2->HasInternalGeomData());

		// External mesh paths should be empty for internal data
		auto meshPaths2 = nif2.GetExternalGeometryPathRefs(s2);
		REQUIRE(meshPaths2.empty());
	}

	// Save again and verify binary stability
	REQUIRE(nif2.Save(fileOutput) == 0);
	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("FixBSXFlags (remove external emittance)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_FixBSXFlags_RemoveExtEmit";
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	nif.FixBSXFlags();
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("FixBSXFlags (add external emittance)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_FixBSXFlags_AddExtEmit";
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	nif.FixBSXFlags();
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("FixShaderFlags (remove environment mapping)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_FixShaderFlags_RemoveEnvMap";
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	nif.FixShaderFlags();
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("FixShaderFlags (add environment mapping)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_FixShaderFlags_AddEnvMap";
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, nifSuffix);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	nif.FixShaderFlags();
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load corrupted file", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Corrupted";
	std::string fileInput = folderInput + "/" + fileName + nifSuffix;

	NifFile nif;
	REQUIRE_THROWS(nif.Load(fileInput) == 0);
}
