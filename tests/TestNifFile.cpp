#include "TestNifFile.hpp"
#include "TestUtil.hpp"

#include <catch2/catch_test_macros.hpp>

#include <NifFile.hpp>
#include <NifUtil.hpp>
#include <Particles.hpp>

#include <fstream>
#include <sstream>

using namespace nifly;

const std::string nifSuffix = ".nif";
const std::string meshSuffix = ".mesh";
const std::string kfSuffix = ".kf";

const std::string folderInput = "input";
const std::string folderOutput = "output";
const std::string folderExpected = "expected";

std::tuple<std::string, std::string, std::string> GetFileTuple(const char* fileName, const std::string& suffix) {
	std::string fileInput = folderInput + "/" + fileName + suffix;
	std::string fileOutput = folderOutput + "/" + fileName + suffix;
	std::string fileExpected = folderExpected + "/" + fileName + suffix;
	return std::make_tuple(fileInput, fileOutput, fileExpected);
}

// Loads every external .mesh file referenced by a BSGeometry shape into its mesh slots.
static void LoadAllExternalMeshData(NifFile& nif, NiShape* shape) {
	auto meshPaths = nif.GetExternalGeometryPathRefs(shape);
	REQUIRE(!meshPaths.empty());

	uint8_t meshIndex = 0;
	for (auto meshPath : meshPaths) {
		std::string meshPathStr = meshPath.get();
		REQUIRE(!meshPathStr.empty());

		const auto meshFileInput = std::get<0>(GetFileTuple(meshPathStr.c_str(), meshSuffix));
		auto meshStream = GetBinaryInputFileStream(std::filesystem::u8path(meshFileInput));
		REQUIRE(meshStream);
		REQUIRE(nif.LoadExternalShapeData(shape, *meshStream, meshIndex));
		meshStream.reset();
		meshIndex++;
	}
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

TEST_CASE("BSGeometry bone weights are normalized (SF)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_SF";
	const auto fileInput = std::get<0>(GetFileTuple(fileName, nifSuffix));

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);

	auto shapes = nif.GetShapes();
	REQUIRE(!shapes.empty());

	size_t weightedVertices = 0;

	for (auto& s : shapes) {
		auto* bsGeom = dynamic_cast<BSGeometry*>(s);
		REQUIRE(bsGeom != nullptr);

		LoadAllExternalMeshData(nif, s);

		std::vector<std::string> bones;
		nif.GetShapeBoneList(s, bones);

		for (uint32_t boneIndex = 0; boneIndex < bones.size(); boneIndex++) {
			std::unordered_map<uint16_t, float> weights;
			nif.GetShapeBoneWeights(s, boneIndex, weights);

			for (const auto& [vid, weight] : weights) {
				REQUIRE(weight >= 0.0f);
				REQUIRE(weight <= 1.0f);
			}

			weightedVertices += weights.size();
		}
	}

	// The Starfield mesh fixture is skinned, so the BSGeometry path must
	// actually return per-vertex weights (regression for the new code path).
	REQUIRE(weightedVertices > 0);
}

TEST_CASE("Delete vertices for shape (SF)", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_SF";
	const auto fileInput = std::get<0>(GetFileTuple(fileName, nifSuffix));

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);

	auto shapes = nif.GetShapes();
	REQUIRE(!shapes.empty());

	for (auto& s : shapes) {
		auto* bsGeom = dynamic_cast<BSGeometry*>(s);
		REQUIRE(bsGeom != nullptr);

		LoadAllExternalMeshData(nif, s);

		auto* meshData = dynamic_cast<BSGeometryMeshData*>(bsGeom->GetGeomData());
		REQUIRE(meshData != nullptr);

		const uint32_t vertCountBefore = meshData->nVertices;
		REQUIRE(vertCountBefore > 4);

		const bool hadMeshlets = meshData->HasMeshlets();
		const bool hadWeights = !meshData->skinWeights.empty();
		const bool hadColors = !meshData->vColors.empty();
		const bool hadNormals = meshData->normals.size() == vertCountBefore;
		const bool hadTangents = meshData->tangents.size() == vertCountBefore;

		// Delete a few vertices spread across the mesh (sorted ascending)
		const std::vector<uint16_t> delIndices = {0,
												  static_cast<uint16_t>(vertCountBefore / 2),
												  static_cast<uint16_t>(vertCountBefore - 1)};

		// Not all vertices were deleted, so the shape must not be flagged for removal
		REQUIRE(!nif.DeleteVertsForShape(s, delIndices));

		const uint32_t vertCountAfter = vertCountBefore - static_cast<uint32_t>(delIndices.size());
		REQUIRE(meshData->nVertices == vertCountAfter);
		REQUIRE(meshData->vertices.size() == vertCountAfter);
		REQUIRE(s->GetNumVertices() == vertCountAfter);

		if (hadWeights)
			REQUIRE(meshData->skinWeights.size() == vertCountAfter);

		if (hadColors)
			REQUIRE(meshData->vColors.size() == vertCountAfter);

		if (hadNormals)
			REQUIRE(meshData->normals.size() == vertCountAfter);

		if (hadTangents) {
			REQUIRE(meshData->tangents.size() == vertCountAfter);
			REQUIRE(meshData->tangentWs.size() == vertCountAfter);
		}

		// All triangles (including LODs) must only reference remaining vertices
		REQUIRE(!meshData->tris.empty());

		for (const auto& t : meshData->tris) {
			REQUIRE(t.p1 < vertCountAfter);
			REQUIRE(t.p2 < vertCountAfter);
			REQUIRE(t.p3 < vertCountAfter);
		}

		for (const auto& lod : meshData->lods) {
			for (const auto& t : lod) {
				REQUIRE(t.p1 < vertCountAfter);
				REQUIRE(t.p2 < vertCountAfter);
				REQUIRE(t.p3 < vertCountAfter);
			}
		}

		// Meshlets and cull data were rebuilt if the mesh had them before
		REQUIRE(meshData->HasMeshlets() == hadMeshlets);
		REQUIRE(meshData->meshletList.size() == meshData->cullDataList.size());
	}
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
		LoadAllExternalMeshData(nif, s);

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
// Morrowind (file version 4.0.0.2) files use inline block types, a boolean size of four bytes,
// a linked list of extra data and a range of blocks that were removed in later versions.

// Loads and saves a Morrowind file and compares the result against the expected output
static void CheckMorrowindFile(const char* fileName, const std::string& suffix = nifSuffix) {
	const auto [fileInput, fileOutput, fileExpected] = GetFileTuple(fileName, suffix);

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);
	REQUIRE(nif.GetHeader().GetVersion().IsMW());
	REQUIRE(nif.GetHeader().HasInlineBlockTypes());
	REQUIRE(!nif.HasUnknown());
	REQUIRE(nif.Save(fileOutput) == 0);

	REQUIRE(CompareBinaryFiles(fileOutput, fileExpected));
}

TEST_CASE("Load and save static file (MW)", "[NifFile]") {
	CheckMorrowindFile("TestNifFile_Static_MW");
}

TEST_CASE("Load and save billboard file (MW)", "[NifFile]") {
	CheckMorrowindFile("TestNifFile_Billboard_MW");
}

TEST_CASE("Load and save skinned file (MW)", "[NifFile]") {
	CheckMorrowindFile("TestNifFile_Skinned_MW");
}

TEST_CASE("Load and save particle file (MW)", "[NifFile]") {
	CheckMorrowindFile("TestNifFile_Particles_MW");
}

TEST_CASE("Load and save rotating particle file (MW)", "[NifFile]") {
	CheckMorrowindFile("TestNifFile_RotatingParticles_MW");
}

TEST_CASE("Load and save UV controller file (MW)", "[NifFile]") {
	CheckMorrowindFile("TestNifFile_UVController_MW");
}

TEST_CASE("Load and save texture effect file (MW)", "[NifFile]") {
	CheckMorrowindFile("TestNifFile_TextureEffect_MW");
}

TEST_CASE("Load and save morph file (MW)", "[NifFile]") {
	CheckMorrowindFile("TestNifFile_Morph_MW");
}

TEST_CASE("Load and save path controller file (MW)", "[NifFile]") {
	CheckMorrowindFile("TestNifFile_PathController_MW");
}

TEST_CASE("Load and save animation sequence file (MW)", "[NifFile]") {
	CheckMorrowindFile("TestNifFile_Sequence_MW", kfSuffix);
}

TEST_CASE("Save unmodified file without changes (MW)", "[NifFile]") {
	// Every file of the game has to be written back byte for byte when nothing is changed
	for (auto fileName : {"TestNifFile_Static_MW",
						  "TestNifFile_Billboard_MW",
						  "TestNifFile_Skinned_MW",
						  "TestNifFile_Particles_MW",
						  "TestNifFile_RotatingParticles_MW",
						  "TestNifFile_UVController_MW",
						  "TestNifFile_TextureEffect_MW",
						  "TestNifFile_Morph_MW",
						  "TestNifFile_PathController_MW"}) {
		INFO(fileName);

		const auto fileInput = std::get<0>(GetFileTuple(fileName, nifSuffix));

		std::ifstream in(fileInput, std::ios::in | std::ios::binary);
		REQUIRE(in);

		std::stringstream original;
		original << in.rdbuf();
		in.close();

		NifFile nif;
		std::stringstream loadFrom(original.str());
		REQUIRE(nif.Load(loadFrom) == 0);

		NifSaveOptions saveOptions;
		saveOptions.optimize = false;
		saveOptions.sortBlocks = false;

		std::stringstream saved;
		REQUIRE(nif.Save(saved, saveOptions) == 0);
		REQUIRE(saved.str() == original.str());
	}
}

// Returns the first block of the given type in the file (or nullptr)
template<typename T>
static T* FindFirstBlock(NifFile& nif) {
	auto& hdr = nif.GetHeader();

	for (uint32_t i = 0; i < hdr.GetNumBlocks(); i++) {
		auto* block = hdr.GetBlock<T>(i);
		if (block)
			return block;
	}

	return nullptr;
}

TEST_CASE("Read blocks of static file (MW)", "[NifFile]") {
	const auto fileInput = std::get<0>(GetFileTuple("TestNifFile_Static_MW", nifSuffix));

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);

	auto& hdr = nif.GetHeader();

	auto* root = nif.GetRootNode();
	REQUIRE(root);
	REQUIRE(root->name == "EditorMarker_box_02");
	REQUIRE(hdr.GetRootBlockIds() == std::vector<uint32_t>{0});

	// Extra data is a linked list up to file version 4.2.2.0
	auto* extraData = hdr.GetBlock<NiStringExtraData>(root->extraDataRef);
	REQUIRE(extraData);
	REQUIRE(extraData->stringData == "MRK");
	REQUIRE(extraData->nextExtraDataRef.IsEmpty());

	REQUIRE(FindFirstBlock<RootCollisionNode>(nif));

	auto shapes = nif.GetShapes();
	REQUIRE(shapes.size() == 2);

	auto* shape = shapes.front();
	REQUIRE(shape->name == "Tri EditorMarker_box_02");
	REQUIRE(shape->HasVertices());
	REQUIRE(shape->GetNumVertices() > 0);

	std::vector<Triangle> tris;
	REQUIRE(shape->GetTriangles(tris));
	REQUIRE(!tris.empty());

	// Morrowind stores the render state in properties instead of a shader property
	REQUIRE(shape->propertyRefs.GetSize() > 0);
	REQUIRE(hdr.GetBlock<NiMaterialProperty>(shape->propertyRefs.GetBlockRef(0)));
}

TEST_CASE("Read blocks of particle file (MW)", "[NifFile]") {
	const auto fileInput = std::get<0>(GetFileTuple("TestNifFile_Particles_MW", nifSuffix));

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);

	auto& hdr = nif.GetHeader();

	auto* particleNode = nif.FindBlockByName<NiBSParticleNode>("Blizzard01");
	REQUIRE(particleNode);

	auto* emitterNode = nif.FindBlockByName<NiBSAnimationNode>("Blizzard01 Emitter");
	REQUIRE(emitterNode);

	auto* particles = nif.FindBlockByName<NiAutoNormalParticles>("Blizzard");
	REQUIRE(particles);

	auto* controller = hdr.GetBlock<NiParticleSystemController>(particles->controllerRef);
	REQUIRE(controller);
	REQUIRE(controller->emitterRef.index == hdr.GetBlockID(emitterNode));
	REQUIRE(controller->particles.size() >= controller->numValid);

	// The particle modifiers form a linked list
	auto* gravity = hdr.GetBlock<NiGravity>(controller->particleModifierRef);
	REQUIRE(gravity);
	REQUIRE(gravity->controllerRef.index == hdr.GetBlockID(controller));

	auto* growFade = hdr.GetBlock<NiParticleGrowFade>(gravity->nextModifierRef);
	REQUIRE(growFade);
	REQUIRE(growFade->nextModifierRef.IsEmpty());

	auto* particleData = hdr.GetBlock<NiAutoNormalParticlesData>(particles->DataRef());
	REQUIRE(particleData);
	REQUIRE(particleData->numParticles == particleData->GetNumVertices());
}

TEST_CASE("Write multiple root references (MW)", "[NifFile]") {
	const auto fileInput = std::get<0>(GetFileTuple("TestNifFile_Static_MW", nifSuffix));

	NifFile nif;
	REQUIRE(nif.Load(fileInput) == 0);

	auto* collisionNode = FindFirstBlock<RootCollisionNode>(nif);
	REQUIRE(collisionNode);

	std::vector<uint32_t> rootIds{0, nif.GetBlockID(collisionNode)};
	nif.GetHeader().SetRootBlockIds(rootIds);

	NifSaveOptions saveOptions;
	saveOptions.optimize = false;
	saveOptions.sortBlocks = false;

	std::stringstream saved;
	REQUIRE(nif.Save(saved, saveOptions) == 0);

	NifFile loaded;
	std::stringstream loadFrom(saved.str());
	REQUIRE(loaded.Load(loadFrom) == 0);
	REQUIRE(loaded.GetHeader().GetRootBlockIds() == rootIds);
}

TEST_CASE("Load corrupted file", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Corrupted";
	std::string fileInput = folderInput + "/" + fileName + nifSuffix;

	NifFile nif;
	REQUIRE_THROWS(nif.Load(fileInput) == 0);
}
