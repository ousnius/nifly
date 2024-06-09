/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#pragma once

#include "Factory.hpp"
#include "Geometry.hpp"
#include "Nodes.hpp"

#if __has_include(<filesystem>)

#include <filesystem>

#elif __has_include(<experimental/optional>)

#include <experimental/filesystem>
namespace std::filesystem {
	using namespace std::experimental::filesystem;
}

#endif

namespace nifly {
// OptimizeFor function options
struct OptOptions {
	NiVersion targetVersion;	// NiVersion target for the optimization process
	bool headParts = false;		// Use mesh formats required for head parts (use ONLY for head parts!)
	bool removeParallax = true; // Remove parallax shader flags and texture paths
	bool calcBounds = true;		// Recalculate bounding spheres for unskinned meshes
	bool fixBSXFlags = true;	// Fix BSX flag values based on file contents
	bool fixShaderFlags = true;	// Fix shader flag values based on file contents
};

// OptimizeFor function result
struct OptResult {
	bool versionMismatch = false; // Indicates if versions are unsupported for the optimization process
	bool dupesRenamed = false;	  // Indicates if there were duplicate shape names that have been renamed
	std::vector<std::string> shapesVColorsRemoved;	 // Names of shapes that had their vertex colors removed
	std::vector<std::string> shapesNormalsRemoved;	 // Names of shapes that had their normals removed
	std::vector<std::string> shapesPartTriangulated; // Names of shapes that had their partitions triangulated
	std::vector<std::string> shapesTangentsAdded; // Names of shapes that received missing tangents/bitangents
	std::vector<std::string> shapesParallaxRemoved; // Names of shapes that had their parallax settings
};

// Sort function for bone weights with indices
struct BoneWeightsSort {
	bool operator()(const SkinWeight& lhs, const SkinWeight& rhs) { return rhs.weight < lhs.weight; }
};

// NifFile load options
struct NifLoadOptions {
	bool isTerrain = false; // Load as terrain file. Affects texture path cleanup and shape names.
};

// NifFile save options
struct NifSaveOptions {
	bool optimize = true;	// Update bounds and delete unreferenced blocks (see NifFile::Optimize)
	bool sortBlocks = true; // Sorts all blocks in a logical order (see NifFile::PrettySortBlocks)
};

class NifFile {
private:
	NiHeader hdr;
	std::vector<std::unique_ptr<NiObject>> blocks;
	bool isValid = false;
	bool hasUnknown = false;
	bool isTerrain = false;

public:
	NifFile() = default;

	NifFile(const std::filesystem::path& fileName, const NifLoadOptions& options = NifLoadOptions()) {
		Load(fileName, options);
	}

	NifFile(std::istream& file, const NifLoadOptions& options = NifLoadOptions()) { Load(file, options); }

	NifFile(const NifFile& other) { CopyFrom(other); }

	NifFile& operator=(const NifFile& other) {
		CopyFrom(other);
		return *this;
	}

	NiHeader& GetHeader() { return hdr; }
	const NiHeader& GetHeader() const { return hdr; }
	void CopyFrom(const NifFile& other);

	int Load(const std::filesystem::path& fileName, const NifLoadOptions& options = NifLoadOptions());
	int Load(std::istream& file, const NifLoadOptions& options = NifLoadOptions());
	int Save(const std::filesystem::path& fileName, const NifSaveOptions& options = NifSaveOptions());
	int Save(std::ostream& file, const NifSaveOptions& options = NifSaveOptions());

	// Update geometry bounds and delete unreferenced blocks
	void Optimize();

	// Optimizes/converts the file using OptOptions and returns OptResult.
	// For use with LE and SE files only.
	OptResult OptimizeFor(OptOptions& options);

	// Fills string refs, links NiGeometryData pointers, cleans up texture paths and removes invalid triangles.
	// For skinned BSTriShape blocks, copies mesh data from skin partitions to shape.
	// Already automatically called by NifFile::Load.
	void PrepareData();

	// Calculates data sizes required for saving.
	// For skinned BSTriShape blocks, copies mesh data back from shape to skin partitions
	// Already automatically called by NifFile::Save.
	void FinalizeData();

	// Indicates that the file was fully loaded or otherwise initialized
	bool IsValid() const { return isValid; }

	// Indicates if there have been any unknown block types during load
	bool HasUnknown() const { return hasUnknown; }

	// Indicates if the file was loaded as terrain
	bool IsTerrain() const { return isTerrain; }

	// Check if all shapes are compatible with SSE (no strips in geometry or skin partition)
	bool IsSSECompatible() const;

	// Check if the shape is compatible with SSE (no strips in geometry or skin partition)
	bool IsSSECompatible(NiShape* shape) const;

	// Creates a new file with a root NiNode using the specified version.
	void Create(const NiVersion& version);

	// Deletes all blocks, header strings and resets the valid status.
	void Clear();

	// Link NiGeometryData pointer to NiGeometry.
	// Doesn't affect BSTriShape blocks.
	void LinkGeomData();

	// Removes triangles with vertex indices that don't exist
	void RemoveInvalidTris() const;

	// Returns vertex limit depending on the file version
	// All versions: 65535 (uint16_t)
	static size_t GetVertexLimit();

	// Returns triangle limit depending on the file version
	// All versions before FO4: 65535      (uint16_t)
	// FO4 and later:           4294967295 (uint32_t)
	size_t GetTriangleLimit() const;

	NiNode* AddNode(const std::string& nodeName, const MatTransform& xformToParent, NiNode* parent = nullptr);
	void DeleteNode(const std::string& nodeName);
	static bool CanDeleteNode(NiNode* node);
	bool CanDeleteNode(const std::string& nodeName) const;
	std::string GetNodeName(const uint32_t blockID) const;
	void SetNodeName(const uint32_t blockID, const std::string& newName);

	uint32_t AssignExtraData(NiAVObject* target, std::unique_ptr<NiExtraData> extraData);

	// Explicitly sets the order of shapes to a new one.
	void SetShapeOrder(const std::vector<std::string>& order);

	struct SortState {
		std::set<uint32_t> visitedIndices;
		std::vector<uint32_t> newIndices;
		uint32_t newIndex = 0;
		std::vector<uint32_t> rootShapeOrder;
	};

	void SetSortIndices(const NiRef& ref, SortState& sortState);
	void SetSortIndices(const NiRef* ref, SortState& sortState);
	void SetSortIndices(uint32_t refIndex, SortState& sortState);

	// Sorts NiObjectNET children
	void SortNiObjectNET(NiObjectNET* objnet, SortState& sortState);

	// Sorts NiAVObject children
	void SortAVObject(NiAVObject* avobj, SortState& sortState);

	// Sorts NiTimeController children
	void SortController(NiTimeController* controller, SortState& sortState);

	// Sorts NiCollisionObject children
	void SortCollision(NiObject* parent, uint32_t parentIndex, SortState& sortState);

	// Sorts NiShape children
	void SortShape(NiShape* shape, SortState& sortState);

	// Sorts a scene graph (starting at NiNode parent)
	void SortGraph(NiNode* root, SortState& sortState);

	// Sorts all blocks in a logical order.
	// Order is based on child references, block types and version.
	void PrettySortBlocks();

	// Fixes the flag values in "BSXFlags" blocks based on file contents.
	void FixBSXFlags();

	// Fixes the flag values in shader blocks based on file contents.
	void FixShaderFlags();

	// Deletes all unreferenced (loose) blocks of the given type.
	// Use default template type "NiObject" for all block types.
	// Does nothing when there are unknown block types to prevent data loss.
	// Returns the amount of deleted blocks (or 0).
	template<class T = NiObject>
	uint32_t DeleteUnreferencedBlocks() {
		if (hasUnknown)
			return 0;

		uint32_t deletionCount = 0;
		hdr.DeleteUnreferencedBlocks<T>(GetBlockID(GetRootNode()), &deletionCount);
		return deletionCount;
	}

	// Deletes all unreferenced (loose) NiNode blocks.
	// Does nothing when there are unknown block types to prevent data loss.
	// Counts the amount of deleted blocks in "deletionCount" if passed.
	bool DeleteUnreferencedNodes(int* deletionCount = nullptr);

	// Find a block of the given type by its name.
	// Block type needs a "name" member (like blocks based on NiObjectNET).
	// Returns block in the correct type or nullptr.
	template<class T = NiObject>
	T* FindBlockByName(const std::string& name) const {
		for (auto& block : blocks) {
			auto namedBlock = dynamic_cast<T*>(block.get());
			if (namedBlock && namedBlock->name == name)
				return namedBlock;
		}

		return nullptr;
	}

	// Returns index of a block in the blocks array or NIF_NPOS
	uint32_t GetBlockID(NiObject* block) const;

	// Returns first direct parent NiNode of a block (or nullptr)
	NiNode* GetParentNode(NiObject* block) const;

	// Moves block from its current parent NiNode to a new parent
	void SetParentNode(NiObject* block, NiNode* parent);

	// Returns all NiNode blocks
	std::vector<NiNode*> GetNodes() const;

	// Returns NiShader pointer of the shape (or nullptr).
	// The underlying shader block type can differ.
	NiShader* GetShader(NiShape* shape) const;

	// Returns NiMaterialProperty pointer of the shape (or nullptr).
	// Used by OB/FO3/NV.
	NiMaterialProperty* GetMaterialProperty(NiShape* shape) const;

	// Returns NiStencilProperty pointer of the shape (or nullptr)
	// Used by OB/FO3/NV.
	NiStencilProperty* GetStencilProperty(NiShape* shape) const;

	// Returns NiTexturingProperty pointer of the shape (or nullptr)
	// Used by OB.
	NiTexturingProperty* GetTexturingProperty(NiShape* shape) const;

	// Returns a mutable gometry data structure for manipulating geometry data. If
	// geometry data cannot be found, nullptr is returned
	NiGeometryData* GetGeometryData(NiShape* shape) const;

	// Returns a list of mesh names useful for locating external mesh data eg data/geometry/<meshname>
	std::vector<std::reference_wrapper<std::string>> GetExternalGeometryPathRefs(NiShape* shape) const;

	// Loads external shape data from the provided istream, storing data in the provided shape
	bool LoadExternalShapeData(NiShape* shape, std::istream& stream, uint8_t shapeIndex);
	// Saves external shape data from the provided shape, storing data in the provided ostream
	bool SaveExternalShapeData(NiShape* shape, std::ostream& outfile, uint8_t shapeIndex);

	// Returns references to all texture path strings of the shape
	std::vector<std::reference_wrapper<std::string>> GetTexturePathRefs(NiShape* shape) const;

	// Fills "outTexFile" with the texture path in the specified slot.
	// Returns:
	// 0 if the texture slot was not found
	// 1 if the texture is found in a BSShaderTextureSet block
	// 2 if the texture is found in a BSEffectShaderProperty block
	// 3 if the texture is found in a NiTexturingProperty block
	uint32_t GetTextureSlot(NiShape* shape, std::string& outTexFile, uint32_t texIndex = 0) const;

	// Sets texture path in the specified slot.
	// Will fill path in both BSShaderTextureSet, BSEffectShaderProperty or NiTexturingProperty blocks.
	void SetTextureSlot(NiShape* shape, std::string& inTexFile, uint32_t texIndex = 0);

	// Normalizes all texture paths in BSShaderTextureSet, BSEffectShaderProperty and NiTexturingProperty blocks
	void TrimTexturePaths();

	// Clones all referenced blocks in the specified block.
	// Source block can be located in a different file (see "srcNif" parameter).
	void CloneChildren(NiObject* block, NifFile* srcNif = nullptr);

	// Clones the specified shape with a destination name and returns it.
	// Source block can be located in a different file (see "srcNif" parameter).
	NiShape* CloneShape(NiShape* srcShape, const std::string& destShapeName, NifFile* srcNif = nullptr);

	// Finds and clones the first NiNode with the specified name and returns its index (or NIF_NPOS).
	// Source block can be located in a different file (see "srcNif" parameter).
	uint32_t CloneNamedNode(const std::string& nodeName, NifFile* srcNif = nullptr);

	// Creates a new unskinned shape for the current file version with vertex/triangle data and returns it.
	// Adds default shader and texture set as well.
	// Parameters for texture coordinates (UVs) and normals are optional (pass nullptr).
	NiShape* CreateShapeFromData(const std::string& shapeName,
								 const std::vector<Vector3>* v,
								 const std::vector<Triangle>* t,
								 const std::vector<Vector2>* uv,
								 const std::vector<Vector3>* norms = nullptr);

	// Returns the names of all shape blocks in the file. Includes duplicates and unnamed shapes.
	std::vector<std::string> GetShapeNames() const;

	// Returns all shape blocks in the file.
	std::vector<NiShape*> GetShapes() const;

	// Renames a shape (same as setting the "name" member)
	static bool RenameShape(NiShape* shape, const std::string& newName);

	// Renames shapes with duplicate names by appending a suffix "_<count>"
	bool RenameDuplicateShapes();

	// Converts the shape from a NiTriStrips to a NiTriShape block
	void TriangulateShape(NiShape* shape);

	// Get direct children of a node of the given block type. Use template type "NiObject" for all block types.
	// Optionally, return extra data references as well.
	template<class T>
	std::vector<T*> GetChildren(NiNode* parent = nullptr, bool searchExtraData = false) const;

	// Returns the root NiNode (block at index 0).
	// If block index 0 is not a NiNode, find the first NiNode instead.
	NiNode* GetRootNode() const;

	// Returns a full block tree in a logical order (recursive function)
	void GetTree(std::vector<NiObject*>& result, NiObject* parent = nullptr) const;

	// Gets the transform (to parent) of the node with the specified name.
	// Returns false if no node matching the name was found.
	bool GetNodeTransformToParent(const std::string& nodeName, MatTransform& outTransform) const;

	// GetNodeTransform is deprecated. Use GetNodeTransformToParent instead.
	bool GetNodeTransform(const std::string& nodeName, MatTransform& outTransform) const {
		return GetNodeTransformToParent(nodeName, outTransform);
	}

	// Calculates the transform from the node's coordinate system to the global coordinate system
	// by composing transforms up the node tree to the root node.
	bool GetNodeTransformToGlobal(const std::string& nodeName, MatTransform& outTransform) const;

	// GetAbsoluteNodeTransform is deprecated. Use GetNodeTransformToGlobal instead.
	bool GetAbsoluteNodeTransform(const std::string& nodeName, MatTransform& outTransform) const {
		return GetNodeTransformToGlobal(nodeName, outTransform);
	}

	// Sets the transform (to parent) of the node with the specified name.
	// With "rootChildrenOnly" enabled, only set the transform of nodes that are direct root children.
	bool SetNodeTransformToParent(const std::string& nodeName,
								  const MatTransform& inTransform,
								  const bool rootChildrenOnly = false);

	// SetNodeTransform is deprecated. Use SetNodeTransformToParent instead.
	bool SetNodeTransform(const std::string& nodeName,
						  MatTransform& inTransform,
						  const bool rootChildrenOnly = false) {
		return SetNodeTransformToParent(nodeName, inTransform, rootChildrenOnly);
	}

	// Gets a list of all bone (node) names used by the shape and returns the count.
	uint32_t GetShapeBoneList(NiShape* shape, std::vector<std::string>& outList) const;

	// Gets a list of all bone (node) block indices used by the shape and returns the count.
	uint32_t GetShapeBoneIDList(NiShape* shape, std::vector<int>& outList) const;

	// Sets the bone index list of the shape's skin instance (and BSSkin::BoneData).
	// Resets bone transforms in BSSkin::BoneData if the bone count changed.
	void SetShapeBoneIDList(NiShape* shape, std::vector<int>& inList);

	// Gets a map of vertex indices to bone weights for the specified shape and bone.
	// Data source is either BSTriShape (if existing) or otherwise NiSkinData.
	// Returns the amount of vertices with non-zero weights.
	uint32_t GetShapeBoneWeights(NiShape* shape,
								 const uint32_t boneIndex,
								 std::unordered_map<uint16_t, float>& outWeights) const;

	// Gets the shape's global-to-skin transform if it has one stored (same as GetShapeTransformGlobalToSkin).
	// Otherwise, try to calculate it using skin-to-bone and node-to-global transforms of existing bones.
	// Returns false if no transform was found or calculated.
	bool CalcShapeTransformGlobalToSkin(NiShape* shape, MatTransform& outTransforms) const;

	// Gets the shape's global-to-skin transform if it has one stored.
	// Returns false if no such transform exists in the file, in which case outTransform will not be changed.
	// Note that, even if this function returns false, you can not assume that the global-to-skin
	// transform is the identity; it almost never is.
	bool GetShapeTransformGlobalToSkin(NiShape* shape, MatTransform& outTransform) const;

	// Sets the shape's global-to-skin transform if it has one stored.
	// Does nothing if the shape has no such transform.
	void SetShapeTransformGlobalToSkin(NiShape* shape, const MatTransform& inTransform);

	// Gets the bone transform (skin-to-bone) of a bone with the specified name.
	// Returns false if bone was not found.
	bool GetShapeTransformSkinToBone(NiShape* shape,
									 const std::string& boneName,
									 MatTransform& outTransform) const;

	// Gets the bone transform (skin-to-bone) of a bone with the specified bone index.
	// Returns false if bone was not found.
	bool GetShapeTransformSkinToBone(NiShape* shape,
									 const uint32_t boneIndex,
									 MatTransform& outTransform) const;

	// Sets the bone transform (skin-to-bone) of a bone with the specified bone index.
	void SetShapeTransformSkinToBone(NiShape* shape,
									 const uint32_t boneIndex,
									 const MatTransform& inTransform);

	// GetShapeBoneTransform is deprecated. Use GetShapeTransformGlobalToSkin or GetShapeTransformSkinToBone instead.
	// Empty string for "boneName" returns the overall skin transform for the shape.
	bool GetShapeBoneTransform(NiShape* shape, const std::string& boneName, MatTransform& outTransform) const;

	// GetShapeBoneTransform is deprecated. Use GetShapeTransformGlobalToSkin or GetShapeTransformSkinToBone instead.
	// 0xFFFFFFFF on the bone index returns the overall skin transform for the shape.
	bool GetShapeBoneTransform(NiShape* shape, const uint32_t boneIndex, MatTransform& outTransform) const;

	// SetShapeBoneTransform is deprecated. Use SetShapeTransformGlobalToSkin or SetShapeTransfromSkinToBone instead.
	// 0xFFFFFFFF for the bone index sets the overall skin transform for the shape.
	bool SetShapeBoneTransform(NiShape* shape, const uint32_t boneIndex, MatTransform& inTransform);

	// Sets bounding sphere for the specified bone index on the shape with the name.
	// Returns false if shape or bone was not found.
	bool SetShapeBoneBounds(const std::string& shapeName, const uint32_t boneIndex, BoundingSphere& inBounds);

	// Gets bounding sphere for the specified bone index on the shape.
	// Returns false if shape or bone was not found.
	bool GetShapeBoneBounds(NiShape* shape, const uint32_t boneIndex, BoundingSphere& outBounds) const;

	// Changes a bone index (node reference) from an old to a new index.
	void UpdateShapeBoneID(const std::string& shapeName, const uint32_t oldID, const uint32_t newID);

	// Sets the bone weights on NiSkinData from the specified vertex weight map.
	// Not implemented for BSTriShape, use SetShapeVertWeights instead.
	void SetShapeBoneWeights(const std::string& shapeName,
							 const uint32_t boneIndex,
							 std::unordered_map<uint16_t, float>& inWeights);

	// Sets the bone weights and bone indices for a single vertex on the shape
	// Not implemented for NiTriShape, use SetShapeBoneWeights instead.
	void SetShapeVertWeights(const std::string& shapeName,
							 const uint16_t vertIndex,
							 std::vector<uint8_t>& boneids,
							 std::vector<float>& weights) const;

	// Clears all bone weights and bone indices on the shape. Not implemented for NiTriShape.
	void ClearShapeVertWeights(const std::string& shapeName) const;

	// Gets the segmentation info and a list of the segments each triangle is assigned to.
	// A triangle can only be assigned to one segment at the same time.
	// A segment index of -1 in the list means the triangle is currently not assigned to any segment.
	static bool GetShapeSegments(NiShape* shape, NifSegmentationInfo& inf, std::vector<int>& triParts);

	// Sets the segmentation info and a list of the segments each triangle is assigned to.
	// A triangle can only be assigned to one segment at the same time.
	static void SetShapeSegments(NiShape* shape,
								 const NifSegmentationInfo& inf,
								 const std::vector<int>& triParts);

	// Gets the partition info and a list of the partitions each triangle is assigned to.
	// A triangle can only be assigned to one partition at the same time.
	// A partition index of -1 in the list means the triangle is currently not assigned to any partition.
	bool GetShapePartitions(NiShape* shape,
							NiVector<BSDismemberSkinInstance::PartitionInfo>& partitionInfo,
							std::vector<int>& triParts) const;

	// Sets the partition info and a list of the partitions each triangle is assigned to.
	// A triangle can only be assigned to one partition at the same time.
	// "convertSkinInstance" will convert a NiSkinInstance to a BSDismemberSkinInstance block.
	void SetShapePartitions(NiShape* shape,
							const NiVector<BSDismemberSkinInstance::PartitionInfo>& partitionInfo,
							const std::vector<int>& triParts,
							const bool convertSkinInstance = true);

	// Clears all partitions and assigns all triangles to a default partition slot.
	// Default slot 32 for Skyrim (body) and slot 0 for FO3/NV (torso).
	void SetDefaultPartition(NiShape* shape);

	// Delete partitions with the specified indices. partInds must be in sorted ascending order before calling!
	void DeletePartitions(NiShape* shape, std::vector<uint32_t>& partInds);

	// Reorder triangles of the shape to the order of triangle indices in the list
	static bool ReorderTriangles(NiShape* shape, const std::vector<uint32_t>& triangleIndices);

	// Gets pointer to vertex positions of the shape (can be nullptr or empty)
	const std::vector<Vector3>* GetVertsForShape(NiShape* shape);
	// Gets pointer to vertex normals of the shape (can be nullptr or empty)
	const std::vector<Vector3>* GetNormalsForShape(NiShape* shape);
	// Gets pointer to vertex texture coordinates (UVs) of the shape (can be nullptr or empty)
	const std::vector<Vector2>* GetUvsForShape(NiShape* shape);
	// Gets pointer to vertex colors of the shape (can be nullptr or empty)
	const std::vector<Color4>* GetColorsForShape(const std::string& shapeName);
	const std::vector<Color4>* GetColorsForShape(NiShape* shape);
	// Gets pointer to vertex tangents of the shape (can be nullptr or empty)
	const std::vector<Vector3>* GetTangentsForShape(NiShape* shape);
	// Gets pointer to vertex bitangents of the shape (can be nullptr or empty)
	const std::vector<Vector3>* GetBitangentsForShape(NiShape* shape);
	// Gets pointer to vertex eye data of the shape (can be nullptr or empty)
	const std::vector<float>* GetEyeDataForShape(NiShape* shape);

	// Gets copy of vertex positions of the shape. Returns false if none are found.
	bool GetVertsForShape(NiShape* shape, std::vector<Vector3>& outVerts) const;
	// Gets copy of vertex texture coordinates (UVs) of the shape. Returns false if none are found.
	bool GetUvsForShape(NiShape* shape, std::vector<Vector2>& outUvs) const;
	// Gets copy of vertex colors of the shape. Returns false if none are found.
	bool GetColorsForShape(NiShape* shape, std::vector<Color4>& outColors) const;
	// Gets copy of vertex tangents of the shape. Returns false if none are found.
	bool GetTangentsForShape(NiShape* shape, std::vector<Vector3>& outTang) const;
	// Gets copy of vertex bitangents of the shape. Returns false if none are found.
	bool GetBitangentsForShape(NiShape* shape, std::vector<Vector3>& outBitang) const;
	// Gets copy of vertex eye data of the shape. Returns false if none is found.
	static bool GetEyeDataForShape(NiShape* shape, std::vector<float>& outEyeData);

	// Sets vertex positions of the shape. Use this function to change vertex count.
	// If vertex count is changed, other vertex data (UVs, normals, ...) is dropped.
	void SetVertsForShape(NiShape* shape, const std::vector<Vector3>& verts);
	// Sets vertex texture coordinates (UVs) of the shape. Size needs to match the current vertex count.
	void SetUvsForShape(NiShape* shape, const std::vector<Vector2>& uvs);
	// Sets vertex colors of the shape. Size needs to match the current vertex count.
	void SetColorsForShape(NiShape* shape, const std::vector<Color4>& colors);
	// Sets vertex colors of the shape. Size needs to match the current vertex count.
	void SetColorsForShape(const std::string& shapeName, const std::vector<Color4>& colors);
	// Sets vertex tangents of the shape. Size needs to match the current vertex count.
	void SetTangentsForShape(NiShape* shape, const std::vector<Vector3>& tangents);
	// Sets vertex bitangents of the shape. Size needs to match the current vertex count.
	void SetBitangentsForShape(NiShape* shape, const std::vector<Vector3>& bitangents);
	// Sets vertex eye data of the shape. Size needs to match the current vertex count.
	static void SetEyeDataForShape(NiShape* shape, const std::vector<float>& eyeData);

	// Gets binary extra data that contains tangent and bitangent data (used in OB).
	// Returns nullptr if no matching extra data was found.
	NiBinaryExtraData* GetBinaryTangentData(NiShape* shape,
											std::vector<nifly::Vector3>* outTangents = nullptr,
											std::vector<nifly::Vector3>* outBitangents = nullptr) const;

	// Sets binary extra data that contains tangent and bitangent data (used in OB).
	void SetBinaryTangentData(NiShape* shape,
							  const std::vector<nifly::Vector3>* tangents,
							  const std::vector<nifly::Vector3>* bitangents);

	// Deletes binary extra data that contains tangent and bitangent data (used in OB).
	void DeleteBinaryTangentData(NiShape* shape);

	// Inverts all texture coordinates on the U- and/or V-axis
	void InvertUVsForShape(NiShape* shape, bool invertX, bool invertY);

	// Mirrors the shape on the X-, Y- and/or Z-axis.
	// Updates normals and tangents as well. Flips triangles if needed.
	void MirrorShape(NiShape* shape, bool mirrorX, bool mirrorY, bool mirrorZ);

	// Sets vertex normals of the shape. Size needs to match the current vertex count.
	void SetNormalsForShape(NiShape* shape, const std::vector<Vector3>& norms);

	// Recalculates (or adds) new normals for the shape.
	// "smooth" and "smoothThresh" affect normals smoothing on virtually welded mesh/UV seams.
	// "force" creates normals for Skyrim model space mapped meshes, which are usually not required.
	void CalcNormalsForShape(NiShape* shape,
							 const bool force = false,
							 const bool smooth = true,
							 const float smoothThresh = 60.0f);

	// Recalculates (or adds) new tangents and bitangents for the shape.
	// Requires normals and UVs to be set beforehand.
	void CalcTangentsForShape(NiShape* shape);

	// Apply normals from a different file to a shape with the same name and vertex count.
	int ApplyNormalsFromFile(NifFile& srcNif, const std::string& shapeName);

	// Gets the translation of the root node (or zero vector)
	void GetRootTranslation(Vector3& outVec) const;

	// Moves a single vertex to the specified position
	void MoveVertex(NiShape* shape, const Vector3& pos, const int id);

	// Moves the entire shape by the specified offset. Respects the specified masking map.
	void OffsetShape(NiShape* shape,
					 const Vector3& offset,
					 std::unordered_map<uint16_t, float>* mask = nullptr);

	// Scales the entire shape from the scene root by the specified factors. Respects the specified masking map.
	void ScaleShape(NiShape* shape, const Vector3& scale, std::unordered_map<uint16_t, float>* mask = nullptr);

	// Rotates the entire shape from the scene root by the specified angles in degrees. Respects the specified masking map.
	void RotateShape(NiShape* shape,
					 const Vector3& angle,
					 std::unordered_map<uint16_t, float>* mask = nullptr);

	// Returns alpha property of the shape (or nullptr)
	NiAlphaProperty* GetAlphaProperty(NiShape* shape) const;

	// Assigns a new alpha property block to the shape/shader.
	// Removes any existing ones. Pointer is moved to the file.
	uint32_t AssignAlphaProperty(NiShape* shape, std::unique_ptr<NiAlphaProperty> alphaProp);

	// Removes any existing alpha properties for the shape/shader.
	void RemoveAlphaProperty(NiShape* shape);

	// Deletes a shape and its child blocks
	void DeleteShape(NiShape* shape);

	// Deletes the shader of a shape and its child blocks
	void DeleteShader(NiShape* shape);

	// Deletes all skinning blocks of a shape and disables skinning
	void DeleteSkinning(NiShape* shape);

	// Removes any partitions without triangles assigned
	void RemoveEmptyPartitions(NiShape* shape);

	// Deletes the specified vertex indices from the shape and notifies all blocks.
	// Skinning and partitions/segments are corrected accordingly.
	bool DeleteVertsForShape(NiShape* shape, const std::vector<uint16_t>& indices);

	// Calculates the difference between the shape's vertex positions and the specified target data (with a scale).
	// Vertices that match up are not returned in the diff data map.
	int CalcShapeDiff(NiShape* shape,
					  const std::vector<Vector3>* targetData,
					  std::unordered_map<uint16_t, Vector3>& outDiffData,
					  float scale = 1.0f);

	// Calculates the difference between the shape's texture coordinates and the specified target data (with a scale).
	// Texture coordinates that match up are not returned in the diff data map.
	int CalcUVDiff(NiShape* shape,
				   const std::vector<Vector2>* targetData,
				   std::unordered_map<uint16_t, Vector3>& outDiffData,
				   float scale = 1.0f);

	// Create all blocks and flags required for skinning, if they don't already exist
	// Blocks: BSDismemberSkinInstance, NiSkinData, NiSkinPartition, BSSkin::Instance, BSSkin::BoneData
	void CreateSkinning(NiShape* shape);

	// Makes NiTriShapeData dynamic by setting the consistency flag to mutable.
	void SetShapeDynamic(const std::string& shapeName);

	// Maintains the number of and makeup of skin partitions where possible,
	// but updates the weighting values and vertex/triangle maps.
	// If required by limits, inserts additional partitions with matching slots.
	void UpdateSkinPartitions(NiShape* shape);

	// Update bone set partition flags. Called automatically in some functions that edit partitions.
	void UpdatePartitionFlags(NiShape* shape);
};

template<class T>
std::vector<T*> NifFile::GetChildren(NiNode* parent, bool searchExtraData) const {
	std::vector<T*> result;
	T* n;

	if (parent == nullptr) {
		parent = GetRootNode();
		if (parent == nullptr)
			return result;
	}

	for (auto& child : parent->childRefs) {
		n = hdr.GetBlock<T>(child);
		if (n)
			result.push_back(n);
	}

	if (searchExtraData) {
		for (auto& extraData : parent->extraDataRefs) {
			n = hdr.GetBlock<T>(extraData);
			if (n)
				result.push_back(n);
		}
	}

	return result;
}
} // namespace nifly
