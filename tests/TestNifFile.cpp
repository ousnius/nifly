#include "TestNifFile.hpp"
#include "TestUtil.hpp"

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

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

TEST_CASE("Clone named node preserves independent extra data", "[NifFile][CloneNamedNode]") {
	NifFile source;
	source.Create(NiVersion::getSF());
	auto sourceNode = source.AddNode("Node with extra data", MatTransform());
	REQUIRE(sourceNode);

	auto stringData = std::make_unique<NiStringExtraData>();
	stringData->name.get() = "Node metadata";
	stringData->stringData.get() = "Original value";
	auto stringId = source.AssignExtraData(sourceNode, std::move(stringData));
	auto integerData = std::make_unique<NiIntegerExtraData>();
	integerData->name.get() = "Node flags";
	integerData->integerData = 42;
	source.AssignExtraData(sourceNode, std::move(integerData));
	source.AddNode("Child node", MatTransform(), sourceNode);

	NifFile destination;
	destination.Create(NiVersion::getSF());
	NifFile* target = nullptr;
	SECTION("Between files with conflicting block indices") {
		destination.AddNode("Existing node", MatTransform());
		// A stale source index would point to a shader instead of extra data.
		auto shaderId = destination.GetHeader().AddBlock(std::make_unique<BSLightingShaderProperty>());
		REQUIRE(shaderId == stringId);
		target = &destination;
	}
	SECTION("Within the same file") {
		target = &source;
	}
	REQUIRE(target);

	auto clonedId = target->CloneNamedNode(sourceNode->name.get(), &source);
	auto clonedNode = target->GetHeader().GetBlock<NiNode>(clonedId);
	REQUIRE(clonedNode);
	clonedNode->name.get() = "Cloned node";
	target->GetRootNode()->childRefs.AddBlockRef(clonedId);
	REQUIRE(clonedNode->childRefs.GetSize() == 0);
	REQUIRE(sourceNode->childRefs.GetSize() == 1);
	REQUIRE(clonedNode->extraDataRefs.GetSize() == 2);

	auto clonedString = target->GetHeader().GetBlock<NiStringExtraData>(clonedNode->extraDataRefs.GetBlockRef(0));
	REQUIRE(clonedString);
	REQUIRE(clonedString->stringData.get() == "Original value");
	clonedString->stringData.get() = "Cloned value";
	REQUIRE(source.GetHeader().GetBlock<NiStringExtraData>(stringId)->stringData.get() == "Original value");

	std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);
	REQUIRE(target->Save(stream) == 0);
	NifFile reloaded;
	REQUIRE(reloaded.Load(stream) == 0);
	auto reloadedNode = reloaded.FindBlockByName<NiNode>("Cloned node");
	REQUIRE(reloadedNode);
	REQUIRE(reloadedNode->extraDataRefs.GetSize() == 2);
	auto reloadedString = reloaded.GetHeader().GetBlock<NiStringExtraData>(reloadedNode->extraDataRefs.GetBlockRef(0));
	auto reloadedInteger = reloaded.GetHeader().GetBlock<NiIntegerExtraData>(reloadedNode->extraDataRefs.GetBlockRef(1));
	REQUIRE(reloadedString);
	REQUIRE(reloadedInteger);
	REQUIRE(reloadedString->name.get() == "Node metadata");
	REQUIRE(reloadedString->stringData.get() == "Cloned value");
	REQUIRE(reloadedInteger->name.get() == "Node flags");
	REQUIRE(reloadedInteger->integerData == 42);
}

TEST_CASE("Clone named node preserves legacy extra-data chains", "[NifFile][CloneNamedNode]") {
	NifFile source;
	source.Create(NiVersion::getMW());
	auto sourceNode = source.AddNode("Node with legacy extra data", MatTransform());
	REQUIRE(sourceNode);
	// AssignExtraData inserts at the head of the legacy linked list.
	for (const char* value : {"Tail", "Middle", "Head"}) {
		auto data = std::make_unique<NiStringExtraData>();
		data->stringData.get() = value;
		data->numBytes = static_cast<uint32_t>(sizeof(uint32_t) + data->stringData.get().size());
		source.AssignExtraData(sourceNode, std::move(data));
	}
	const uint32_t sourceHead = sourceNode->extraDataRef.index;

	NifFile destination;
	destination.Create(NiVersion::getMW());
	// Occupy every source data index with an incompatible block type.
	for (uint32_t i = 0; i < sourceHead; ++i)
		destination.AddNode("Existing node " + std::to_string(i), MatTransform());
	auto clonedId = destination.CloneNamedNode(sourceNode->name.get(), &source);
	auto clonedNode = destination.GetHeader().GetBlock<NiNode>(clonedId);
	REQUIRE(clonedNode);
	REQUIRE(destination.GetHeader().GetBlock<NiStringExtraData>(clonedNode->extraDataRef));
	destination.GetRootNode()->childRefs.AddBlockRef(clonedId);

	std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);
	REQUIRE(destination.Save(stream) == 0);
	NifFile reloaded;
	REQUIRE(reloaded.Load(stream) == 0);
	auto reloadedNode = reloaded.FindBlockByName<NiNode>(sourceNode->name.get());
	REQUIRE(reloadedNode);
	uint32_t next = reloadedNode->extraDataRef.index;
	for (const char* value : {"Head", "Middle", "Tail"}) {
		auto data = reloaded.GetHeader().GetBlock<NiStringExtraData>(next);
		REQUIRE(data);
		REQUIRE(data->stringData.get() == value);
		next = data->nextExtraDataRef.index;
	}
	REQUIRE(next == NIF_NPOS);
}

TEST_CASE("Clone named node preserves properties and their texture references", "[NifFile][CloneNamedNode]") {
	// Exercise both inline strings and the newer string table with legacy properties.
	const auto version = GENERATE(NiVersion::getMW(), NiVersion::getFO3());
	INFO(version.GetVersionInfo());

	NifFile source;
	source.Create(version);
	auto sourceNode = source.AddNode("Node with properties", MatTransform());
	REQUIRE(sourceNode);

	auto texture = std::make_unique<NiSourceTexture>();
	texture->name.get() = "Base texture";
	texture->fileName.get() = "textures/original.dds";
	const auto textureId = source.GetHeader().AddBlock(std::move(texture));
	auto texturing = std::make_unique<NiTexturingProperty>();
	texturing->name.get() = "Node texturing";
	texturing->hasBaseTex = true;
	texturing->baseTex.sourceRef.index = textureId;
	const auto texturingId = source.GetHeader().AddBlock(std::move(texturing));
	sourceNode->propertyRefs.AddBlockRef(texturingId);

	NifFile destination;
	destination.Create(version);
	for (uint32_t i = 0; i < texturingId; ++i)
		destination.AddNode("Existing node " + std::to_string(i), MatTransform());
	auto clonedId = destination.CloneNamedNode(sourceNode->name.get(), &source);
	auto clonedNode = destination.GetHeader().GetBlock<NiNode>(clonedId);
	REQUIRE(clonedNode);
	destination.GetRootNode()->childRefs.AddBlockRef(clonedId);

	auto checkProperties = [](NifFile& nif, NiNode* node) {
		REQUIRE(node->propertyRefs.GetSize() == 1);
		auto texturing = nif.GetHeader().GetBlock<NiTexturingProperty>(node->propertyRefs.GetBlockRef(0));
		REQUIRE(texturing);
		REQUIRE(texturing->name.get() == "Node texturing");
		auto texture = nif.GetHeader().GetBlock<NiSourceTexture>(texturing->baseTex.sourceRef);
		REQUIRE(texture);
		REQUIRE(texture->name.get() == "Base texture");
		REQUIRE(texture->fileName.get() == "textures/original.dds");
	};
	checkProperties(destination, clonedNode);

	std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);
	REQUIRE(destination.Save(stream) == 0);
	NifFile reloaded;
	REQUIRE(reloaded.Load(stream) == 0);
	auto reloadedNode = reloaded.FindBlockByName<NiNode>(sourceNode->name.get());
	REQUIRE(reloadedNode);
	checkProperties(reloaded, reloadedNode);
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
		uint32_t indexOffset = 0;
		for (const auto& meshlet : meshData->meshletList) {
			REQUIRE(meshlet.primOffset == indexOffset);
			indexOffset += (meshlet.primCount * 3 + 3) & ~3u;
		}
	}
}

TEST_CASE("Starfield vertex colors follow editor topology changes", "[NifFile][StarfieldColors]") {
	NifFile nif;
	nif.Create(NiVersion::getSF());
	auto block = std::make_unique<BSGeometry>();
	auto shape = block.get();
	shape->name.get() = "Colored shape";
	shape->SetInternalGeomData(true);
	auto& data = shape->AddMesh()->meshData;
	data.version = 2;
	data.scale = 1.0f;
	const std::vector<Vector3> originalVertices{{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}};
	data.Create(nif.GetHeader().GetVersion(), &originalVertices, nullptr, nullptr, nullptr);
	data.nVertices = 4;
	data.vColors = {{10, 20, 30, 40}, {50, 60, 70, 80}, {90, 100, 110, 120}, {130, 140, 150, 160}};
	nif.GetRootNode()->childRefs.AddBlockRef(nif.GetHeader().AddBlock(std::move(block)));
	const auto originalColors = data.vColors;

	// Outfit Studio edits copies of the attribute arrays, then writes them back.
	auto colorPtr = nif.GetColorsForShape(shape);
	REQUIRE(colorPtr);
	REQUIRE(colorPtr->size() == 4);
	const auto colors = *colorPtr;
	std::vector<Color4> copiedColors;
	REQUIRE(nif.GetColorsForShape(shape, copiedColors));
	REQUIRE(copiedColors.size() == colors.size());
	REQUIRE(copiedColors[2] == colors[2]);

	SECTION("Delete an interior vertex") {
		auto vertices = originalVertices;
		vertices.erase(vertices.begin() + 1);
		auto editedColors = colors;
		editedColors.erase(editedColors.begin() + 1);
		nif.SetVertsForShape(shape, vertices);
		nif.SetColorsForShape(shape, editedColors);
		REQUIRE(data.vColors.size() == 3);
		REQUIRE(data.vColors[1] == originalColors[2]);
	}
	SECTION("Undo restores the original colors") {
		nif.SetVertsForShape(shape, {originalVertices[0], originalVertices[2], originalVertices[3]});
		nif.SetColorsForShape(shape, {colors[0], colors[2], colors[3]});
		nif.SetVertsForShape(shape, originalVertices);
		nif.SetColorsForShape(shape, colors);
		REQUIRE(data.vColors.size() == originalColors.size());
		for (size_t i = 0; i < originalColors.size(); ++i)
			REQUIRE(data.vColors[i] == originalColors[i]);
	}

	const auto expectedColors = data.vColors;
	std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);
	REQUIRE(nif.Save(stream) == 0);
	NifFile reloaded;
	REQUIRE(reloaded.Load(stream) == 0);
	auto reloadedShape = reloaded.FindBlockByName<BSGeometry>("Colored shape");
	REQUIRE(reloadedShape);
	auto reloadedData = dynamic_cast<BSGeometryMeshData*>(reloadedShape->GetGeomData());
	REQUIRE(reloadedData);
	REQUIRE(reloadedData->vColors.size() == reloadedData->vertices.size());
	for (size_t i = 0; i < expectedColors.size(); ++i)
		REQUIRE(reloadedData->vColors[i] == expectedColors[i]);
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

// Appends an unsigned value as byteCount little-endian bytes
static void AppendLittleEndian(std::string& bytes, const uint32_t value, const int byteCount) {
	for (int i = 0; i < byteCount; i++)
		bytes.push_back(static_cast<char>((value >> (i * 8)) & 0xFF));
}

// Builds the bytes of a sized string as stored in a file: a size prefix of szSize bytes
// followed by the raw payload
static std::string MakeSizedString(const std::string& payload, const int szSize) {
	std::string bytes;
	AppendLittleEndian(bytes, static_cast<uint32_t>(payload.size()), szSize);

	bytes += payload;
	return bytes;
}

static NiString ReadSizedString(const std::string& bytes, const int szSize) {
	std::istringstream in(bytes, std::ios::binary);
	NiIStream istream(&in, nullptr);

	NiString str;
	str.Read(istream, szSize);
	return str;
}

static std::string WriteSizedString(NiString& str, const int szSize) {
	std::ostringstream out(std::ios::binary);
	NiOStream ostream(&out, nullptr);

	str.Write(ostream, szSize);
	return out.str();
}

TEST_CASE("Read and write sized strings", "[NifFile]") {
	for (const int szSize : {1, 2, 4}) {
		INFO("size prefix: " << szSize);

		SECTION("plain string") {
			const auto bytes = MakeSizedString("Bip01 Pelvis", szSize);
			auto str = ReadSizedString(bytes, szSize);

			REQUIRE(str.get() == "Bip01 Pelvis");
			REQUIRE_FALSE(str.GetNullOutput());
			REQUIRE(str.byteLength() == 12);
			REQUIRE(WriteSizedString(str, szSize) == bytes);
		}

		SECTION("empty string") {
			const auto bytes = MakeSizedString("", szSize);
			auto str = ReadSizedString(bytes, szSize);

			REQUIRE(str.get().empty());
			REQUIRE_FALSE(str.GetNullOutput());
			REQUIRE(WriteSizedString(str, szSize) == bytes);
		}

		SECTION("null terminated string") {
			// The trailing null byte is counted in the size, but is not part of the string
			const auto bytes = MakeSizedString(std::string("Nifly", 5) + '\0', szSize);
			auto str = ReadSizedString(bytes, szSize);

			REQUIRE(str.get() == "Nifly");
			REQUIRE(str.GetNullOutput());
			REQUIRE(str.byteLength() == 6);
			REQUIRE(WriteSizedString(str, szSize) == bytes);
		}

		SECTION("null separated strings are not truncated") {
			// The payload of NiStringPalette is a buffer of null separated strings
			const std::string payload = std::string("Bip01\0NiTransformController\0Bip01 Pelvis\0", 41);
			const auto bytes = MakeSizedString(payload, szSize);
			auto str = ReadSizedString(bytes, szSize);

			REQUIRE(str.get().size() == 40);
			REQUIRE(str.GetNullOutput());
			REQUIRE(str.byteLength() == 41);
			REQUIRE(str.get().find("NiTransformController") == 6);
			REQUIRE(str.get().find("Bip01 Pelvis") == 28);
			REQUIRE(WriteSizedString(str, szSize) == bytes);
		}
	}
}

TEST_CASE("Read and write string palette", "[NifFile]") {
	// A palette as the format defines it, with the offsets its controlled blocks reference
	const std::string palette = std::string("Bip01\0NiTransformController\0Bip01 Pelvis\0Bip01 Spine\0", 53);
	const std::vector<std::pair<uint32_t, std::string>> entries{{0, "Bip01"},
																{6, "NiTransformController"},
																{28, "Bip01 Pelvis"},
																{41, "Bip01 Spine"}};

	// The palette size is repeated after the payload
	const auto length = static_cast<uint32_t>(palette.size());
	std::string bytes = MakeSizedString(palette, 4);
	AppendLittleEndian(bytes, length, 4);

	NiStringPalette stringPalette;
	{
		std::istringstream in(bytes, std::ios::binary);
		NiIStream istream(&in, nullptr);
		NiStreamReversible stream(&istream, nullptr, NiStreamReversible::Mode::Reading);
		stringPalette.Sync(stream);
	}

	REQUIRE(stringPalette.length == length);
	REQUIRE(stringPalette.palette.byteLength() == length);

	for (const auto& [offset, name] : entries) {
		INFO("offset: " << offset);
		REQUIRE(stringPalette.palette.get().compare(offset, name.size(), name) == 0);
	}

	// A load and save round trip has to write the palette back unchanged
	std::ostringstream out(std::ios::binary);
	{
		NiOStream ostream(&out, nullptr);
		NiStreamReversible stream(nullptr, &ostream, NiStreamReversible::Mode::Writing);
		stringPalette.Sync(stream);
	}

	REQUIRE(out.str() == bytes);
}

TEST_CASE("Load corrupted file", "[NifFile]") {
	constexpr auto fileName = "TestNifFile_Corrupted";
	std::string fileInput = folderInput + "/" + fileName + nifSuffix;

	NifFile nif;
	REQUIRE_THROWS(nif.Load(fileInput) == 0);
}

TEST_CASE("BSGeometry skinning is created and resized like BSTriShape (SF)", "[NifFile][StarfieldSkinning]") {
	NifFile nif;
	nif.Create(NiVersion::getSF());
	auto block = std::make_unique<BSGeometry>();
	auto shape = block.get();
	shape->name.get() = "Skinned shape";
	shape->SetInternalGeomData(true);
	auto& data = shape->AddMesh()->meshData;
	data.version = 2;
	data.scale = 1.0f;
	const std::vector<Vector3> vertices{{0, 0, 0}, {1, 0, 0}, {0, 1, 0}};
	data.Create(nif.GetHeader().GetVersion(), &vertices, nullptr, nullptr, nullptr);
	data.nVertices = 3;
	nif.GetRootNode()->childRefs.AddBlockRef(nif.GetHeader().AddBlock(std::move(block)));

	auto findSkinAttach = [](NifFile& file, BSGeometry* geom) -> SkinAttach* {
		for (auto& extraDataRef : geom->extraDataRefs) {
			auto skinAttach = file.GetHeader().GetBlock<SkinAttach>(extraDataRef);
			if (skinAttach)
				return skinAttach;
		}
		return nullptr;
	};

	REQUIRE_FALSE(shape->IsSkinned());
	REQUIRE_FALSE(findSkinAttach(nif, shape));

	// An unskinned BSGeometry gets a BSSkinInstance, BSSkinBoneData and an empty SkinAttach block
	nif.CreateSkinning(shape);
	REQUIRE(shape->IsSkinned());

	auto& hdr = nif.GetHeader();
	auto skinInst = hdr.GetBlock<BSSkinInstance>(shape->SkinInstanceRef());
	REQUIRE(skinInst);
	REQUIRE(skinInst->targetRef.index == nif.GetBlockID(nif.GetRootNode()));

	auto boneData = hdr.GetBlock(skinInst->dataRef);
	REQUIRE(boneData);
	REQUIRE(boneData->nBones == 0);
	REQUIRE(boneData->boneXforms.empty());

	auto skinAttach = findSkinAttach(nif, shape);
	REQUIRE(skinAttach);
	REQUIRE(skinAttach->bones.empty());

	// Skinning an already skinned shape must not add any blocks
	const auto numBlocks = hdr.GetNumBlocks();
	nif.CreateSkinning(shape);
	REQUIRE(hdr.GetNumBlocks() == numBlocks);
	REQUIRE(hdr.GetBlock<BSSkinInstance>(shape->SkinInstanceRef()) == skinInst);

	// Starfield bone refs are all None (bones are bound by SkinAttach names),
	// but the bone data block still has to grow and shrink with the bone list
	std::vector<int> boneIDs(3, static_cast<int>(NIF_NPOS));
	nif.SetShapeBoneIDList(shape, boneIDs);
	REQUIRE(skinInst->boneRefs.GetSize() == 3);
	REQUIRE(boneData->nBones == 3);
	REQUIRE(boneData->boneXforms.size() == 3);

	std::vector<int> fewerBoneIDs(1, static_cast<int>(NIF_NPOS));
	nif.SetShapeBoneIDList(shape, fewerBoneIDs);
	REQUIRE(skinInst->boneRefs.GetSize() == 1);
	REQUIRE(boneData->nBones == 1);
	REQUIRE(boneData->boneXforms.size() == 1);

	nif.SetShapeBoneIDList(shape, boneIDs);
	REQUIRE(boneData->nBones == 3);
	REQUIRE(boneData->boneXforms.size() == 3);

	// With the bone data sized correctly, per-bone transforms can be stored
	MatTransform xform;
	xform.translation = Vector3(1.0f, 2.0f, 3.0f);
	nif.SetShapeTransformSkinToBone(shape, 2, xform);
	MatTransform readBack;
	REQUIRE(nif.GetShapeTransformSkinToBone(shape, 2, readBack));
	REQUIRE(readBack.translation == xform.translation);

	// Setting the bone list by name writes the names to SkinAttach, keeps the bone refs
	// as None and sizes the bone data to match, so GetShapeBoneList reads them back
	const std::vector<std::string> boneNames{"COM", "Pelvis", "Spine1"};
	nif.SetShapeBoneList(shape, boneNames);
	REQUIRE(skinAttach->bones.size() == 3);
	for (uint32_t i = 0; i < boneNames.size(); ++i)
		REQUIRE(skinAttach->bones[i].get() == boneNames[i]);
	REQUIRE(skinInst->boneRefs.GetSize() == 3);
	for (auto& boneRef : skinInst->boneRefs)
		REQUIRE(boneRef.IsEmpty());
	REQUIRE(boneData->nBones == 3);
	REQUIRE(boneData->boneXforms.size() == 3);
	// Same bone count: the transforms set before are preserved
	REQUIRE(boneData->boneXforms[2].boneTransform.translation == xform.translation);

	std::vector<std::string> readBones;
	REQUIRE(nif.GetShapeBoneList(shape, readBones) == 3);
	REQUIRE(readBones == boneNames);

	// Adding a bone grows SkinAttach and the bone data together
	const std::vector<std::string> moreBoneNames{"COM", "Pelvis", "Spine1", "Spine2"};
	nif.SetShapeBoneList(shape, moreBoneNames);
	REQUIRE(skinAttach->bones.size() == 4);
	REQUIRE(skinAttach->bones[3].get() == "Spine2");
	REQUIRE(skinInst->boneRefs.GetSize() == 4);
	REQUIRE(boneData->nBones == 4);
	REQUIRE(boneData->boneXforms.size() == 4);
	nif.SetShapeTransformSkinToBone(shape, 3, xform);

	std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);
	REQUIRE(nif.Save(stream) == 0);
	NifFile reloaded;
	REQUIRE(reloaded.Load(stream) == 0);

	auto reloadedShape = reloaded.FindBlockByName<BSGeometry>("Skinned shape");
	REQUIRE(reloadedShape);
	REQUIRE(reloadedShape->IsSkinned());

	auto reloadedSkinInst = reloaded.GetHeader().GetBlock<BSSkinInstance>(reloadedShape->SkinInstanceRef());
	REQUIRE(reloadedSkinInst);
	REQUIRE(reloadedSkinInst->boneRefs.GetSize() == 4);

	auto reloadedBoneData = reloaded.GetHeader().GetBlock(reloadedSkinInst->dataRef);
	REQUIRE(reloadedBoneData);
	REQUIRE(reloadedBoneData->nBones == 4);
	REQUIRE(reloadedBoneData->boneXforms.size() == 4);
	REQUIRE(reloadedBoneData->boneXforms[3].boneTransform.translation == xform.translation);

	auto reloadedSkinAttach = findSkinAttach(reloaded, reloadedShape);
	REQUIRE(reloadedSkinAttach);
	REQUIRE(reloadedSkinAttach->bones.size() == 4);
	std::vector<std::string> reloadedBones;
	REQUIRE(reloaded.GetShapeBoneList(reloadedShape, reloadedBones) == 4);
	REQUIRE(reloadedBones == moreBoneNames);
}

TEST_CASE("SetShapeBoneList creates missing SkinAttach and bone data for BSGeometry (SF)", "[NifFile][StarfieldSkinning]") {
	// Mirrors a loaded file whose shape has a BSSkin::Instance but no bone data and no SkinAttach
	NifFile nif;
	nif.Create(NiVersion::getSF());
	auto block = std::make_unique<BSGeometry>();
	auto shape = block.get();
	shape->name.get() = "Skinned shape";
	nif.GetRootNode()->childRefs.AddBlockRef(nif.GetHeader().AddBlock(std::move(block)));

	auto& hdr = nif.GetHeader();
	auto [skinInstS, skinInst] = make_unique<BSSkinInstance>();
	shape->SkinInstanceRef()->index = hdr.AddBlock(std::move(skinInstS));
	shape->SetSkinned(true);
	REQUIRE_FALSE(hdr.GetBlock(skinInst->dataRef));

	const std::vector<std::string> boneNames{"COM", "Pelvis"};
	nif.SetShapeBoneList(shape, boneNames);

	auto boneData = hdr.GetBlock(skinInst->dataRef);
	REQUIRE(boneData);
	REQUIRE(boneData->nBones == 2);
	REQUIRE(boneData->boneXforms.size() == 2);
	REQUIRE(skinInst->boneRefs.GetSize() == 2);

	std::vector<std::string> readBones;
	REQUIRE(nif.GetShapeBoneList(shape, readBones) == 2);
	REQUIRE(readBones == boneNames);

	// Unskinned shapes are left alone
	auto unskinnedBlock = std::make_unique<BSGeometry>();
	auto unskinned = unskinnedBlock.get();
	unskinned->name.get() = "Unskinned shape";
	nif.GetRootNode()->childRefs.AddBlockRef(hdr.AddBlock(std::move(unskinnedBlock)));
	const auto numBlocks = hdr.GetNumBlocks();
	nif.SetShapeBoneList(unskinned, boneNames);
	REQUIRE(hdr.GetNumBlocks() == numBlocks);
	REQUIRE(unskinned->extraDataRefs.GetSize() == 0);
}

TEST_CASE("SetShapeBoneList resolves bone names to node refs for BSTriShape", "[NifFile]") {
	NifFile nif;
	nif.Create(NiVersion::getFO4());
	auto block = std::make_unique<BSTriShape>();
	auto shape = block.get();
	shape->name.get() = "Skinned shape";
	nif.GetRootNode()->childRefs.AddBlockRef(nif.GetHeader().AddBlock(std::move(block)));

	MatTransform xform;
	auto com = nif.AddNode("COM", xform);
	auto pelvis = nif.AddNode("Pelvis", xform, com);
	REQUIRE(com);
	REQUIRE(pelvis);

	nif.CreateSkinning(shape);
	REQUIRE(shape->IsSkinned());

	nif.SetShapeBoneList(shape, {"COM", "Pelvis", "MissingBone"});

	auto& hdr = nif.GetHeader();
	auto skinInst = hdr.GetBlock<BSSkinInstance>(shape->SkinInstanceRef());
	REQUIRE(skinInst);
	REQUIRE(skinInst->boneRefs.GetSize() == 3);
	REQUIRE(skinInst->boneRefs.GetBlockRef(0) == nif.GetBlockID(com));
	REQUIRE(skinInst->boneRefs.GetBlockRef(1) == nif.GetBlockID(pelvis));
	REQUIRE(skinInst->boneRefs.GetBlockRef(2) == NIF_NPOS);

	auto boneData = hdr.GetBlock(skinInst->dataRef);
	REQUIRE(boneData);
	REQUIRE(boneData->nBones == 3);

	std::vector<std::string> readBones;
	REQUIRE(nif.GetShapeBoneList(shape, readBones) == 2);
	REQUIRE(readBones == std::vector<std::string>{"COM", "Pelvis"});
}
