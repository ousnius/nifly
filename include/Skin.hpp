/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#pragma once

#include "BasicTypes.hpp"
#include "VertexData.hpp"

namespace nifly {
#pragma pack(push, 1)
struct SkinWeight {
	uint16_t index;
	float weight;

	SkinWeight(const uint16_t index_ = 0, const float weight_ = 0.0f)
		: index(index_)
		, weight(weight_) {}
};
#pragma pack(pop)

struct VertexWeight {
	float w1 = 0.0f;
	float w2 = 0.0f;
	float w3 = 0.0f;
	float w4 = 0.0f;
};

struct BoneIndices {
	uint8_t i1 = 0;
	uint8_t i2 = 0;
	uint8_t i3 = 0;
	uint8_t i4 = 0;
};

class NiSkinData : public NiCloneableStreamable<NiSkinData, NiObject> {
public:
	struct BoneData {
		// boneTransform transforms from skin CS to bone CS.
		// Recommend renaming boneTransform to transformSkinToBone.
		MatTransform boneTransform;
		BoundingSphere bounds;
		uint16_t numVertices = 0;
		std::vector<SkinWeight> vertexWeights;
	};

	// skinTransform transforms from the global CS to the skin CS.
	// Recommend renaming to "transformGlobalToSkin".
	MatTransform skinTransform;
	uint32_t numBones = 0;
	uint8_t hasVertWeights = 1;
	std::vector<BoneData> bones;

	static constexpr const char* BlockName = "NiSkinData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) override;
};

class NiSkinPartition : public NiCloneableStreamable<NiSkinPartition, NiObject> {
public:
	struct PartitionBlock {
		uint16_t numVertices = 0;
		uint16_t numTriangles = 0;
		uint16_t numBones = 0;
		uint16_t numStrips = 0;
		uint16_t numWeightsPerVertex = 0;
		std::vector<uint16_t> bones;
		bool hasVertexMap = false;
		std::vector<uint16_t> vertexMap;
		bool hasVertexWeights = false;
		std::vector<VertexWeight> vertexWeights;
		std::vector<uint16_t> stripLengths;
		bool hasFaces = false;
		std::vector<std::vector<uint16_t>> strips;
		std::vector<Triangle> triangles;
		bool hasBoneIndices = false;
		std::vector<BoneIndices> boneIndices;

		uint8_t lodLevel = 0;  // User Version >= 12
		bool globalVB = false; // User Version >= 12
		VertexDesc vertexDesc; // User Version >= 12, User Version 2 == 100
		// When trueTriangles is changed so it's no longer in sync with
		// triParts, triParts should be cleared.
		std::vector<Triangle> trueTriangles; // User Version >= 12, User Version 2 == 100

		bool ConvertStripsToTriangles();
		void GenerateTrueTrianglesFromMappedTriangles();
		void GenerateMappedTrianglesFromTrueTrianglesAndVertexMap();
		void GenerateVertexMapFromTrueTriangles();
	};

	uint32_t numPartitions = 0;
	uint32_t dataSize = 0;	 // User Version >= 12, User Version 2 == 100
	uint32_t vertexSize = 0; // User Version >= 12, User Version 2 == 100
	VertexDesc vertexDesc;	 // User Version >= 12, User Version 2 == 100

	uint32_t numVertices = 0;			// Not in file
	std::vector<BSVertexData> vertData; // User Version >= 12, User Version 2 == 100
	std::vector<PartitionBlock> partitions;

	// bMappedIndices is not in the file; it is calculated from
	// the file version.  If true, the vertex indices in triangles
	// and strips are indices into vertexMap, not the shape's vertices.
	// trueTriangles always uses indices into the shape's vertex list.
	bool bMappedIndices = true;

	// triParts is not in the file; it is generated as needed.  If
	// not empty, its size should match the shape's triangle list.
	// It gives the partition index (into "partitions") of each
	// triangle.  Whenever triParts is changed so it's not in sync
	// with trueTriangles, GenerateTrueTrianglesFromTriParts should
	// be called to get them back in sync.
	std::vector<int> triParts;

	bool HasVertices() const { return vertexDesc.HasFlag(VF_VERTEX); }
	bool HasUVs() const { return vertexDesc.HasFlag(VF_UV); }
	bool HasNormals() const { return vertexDesc.HasFlag(VF_NORMAL); }
	bool HasTangents() const { return vertexDesc.HasFlag(VF_TANGENT); }
	bool HasVertexColors() const { return vertexDesc.HasFlag(VF_COLORS); }
	bool IsSkinned() const { return vertexDesc.HasFlag(VF_SKINNED); }
	bool HasEyeData() const { return vertexDesc.HasFlag(VF_EYEDATA); }
	bool IsFullPrecision() const { return true; }

	static constexpr const char* BlockName = "NiSkinPartition";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) override;
	// DeletePartitions: partInds must be in sorted ascending order
	void DeletePartitions(const std::vector<uint32_t>& partInds);
	uint32_t RemoveEmptyPartitions(std::vector<uint32_t>& outDeletedIndices);
	// ConvertStripsToTriangles returns true if any conversions were
	// actually performed.  After calling this function, all of the
	// strips will be empty.
	bool ConvertStripsToTriangles();
	// PrepareTrueTriangles: ensures each partition's trueTriangles has
	// valid data, if necessary by generating it from "triangles" or "strips".
	void PrepareTrueTriangles();
	// PrepareVertexMapsAndTriangles: ensures "vertexMap" and "triangles"
	// have valid data for every partition, if necessary by generating them
	// from trueTriangles.
	void PrepareVertexMapsAndTriangles();
	// GenerateTriPartsFromTrueTriangles: generates triParts from
	// the partitions' trueTriangles by looking them up in shapeTris.
	// The new triParts will have the same size as shapeTris.  Though
	// typically triParts[i] will be between 0 and partitions.size()-1,
	// it is theoretically possible for some triParts[i] to be -1
	// (like because of garbage data in the file).
	void GenerateTriPartsFromTrueTriangles(const std::vector<Triangle>& shapeTris);
	// GenerateTrueTrianglesFromTriParts: generates the partitions'
	// trueTriangles from triParts and shapeTris.  If triParts[i] is
	// out of range, the corresponding triangle will not be copied
	// into a partition.
	void GenerateTrueTrianglesFromTriParts(const std::vector<Triangle>& shapeTris);
	// PrepareTriParts: ensures triParts has data, generating it
	// if necessary from trueTriangles and shapeTris.
	void PrepareTriParts(const std::vector<Triangle>& shapeTris);
};

class NiNode;

class NiBoneContainer : public NiCloneable<NiBoneContainer, NiObject> {
public:
	NiBlockPtrArray<NiNode> boneRefs;
};

class NiSkinInstance : public NiCloneableStreamable<NiSkinInstance, NiBoneContainer> {
public:
	NiBlockRef<NiSkinData> dataRef;
	NiBlockRef<NiSkinPartition> skinPartitionRef;
	NiBlockPtr<NiNode> targetRef;

	static constexpr const char* BlockName = "NiSkinInstance";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
	void GetPtrs(std::set<NiRef*>& ptrs) override;
};


enum PartitionFlags : uint16_t { PF_NONE = 0, PF_EDITOR_VISIBLE = 1 << 0, PF_START_NET_BONESET = 1 << 8 };

class BSDismemberSkinInstance : public NiCloneableStreamable<BSDismemberSkinInstance, NiSkinInstance> {
public:
	struct PartitionInfo {
		PartitionFlags flags = PF_NONE;
		uint16_t partID = 0;
	};

	NiVector<PartitionInfo> partitions;

	static constexpr const char* BlockName = "BSDismemberSkinInstance";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	// DeletePartitions: partInds must be in sorted ascending order.
	void DeletePartitions(const std::vector<uint32_t>& partInds);
};

class BSSkinBoneData : public NiCloneableStreamable<BSSkinBoneData, NiObject> {
public:
	uint32_t nBones = 0;

	struct BoneData {
		BoundingSphere bounds;
		// boneTransform transforms from skin CS (which is usually not
		// the same as global CS for skins with BSSkinBoneData) to bone
		// CS.  Recommend renaming boneTransform to transformSkinToBone.
		MatTransform boneTransform;
	};
	// Note that, unlike for NiSkinData, the global-to-skin transform
	// "skinTransform" is not given explicitly but implied by the other
	// transforms.

	std::vector<BoneData> boneXforms;

	static constexpr const char* BlockName = "BSSkin::BoneData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiAVObject;

class BSSkinInstance : public NiCloneableStreamable<BSSkinInstance, NiBoneContainer> {
public:
	NiBlockPtr<NiAVObject> targetRef;
	NiBlockRef<BSSkinBoneData> dataRef;
	NiVector<Vector3> scales;

	static constexpr const char* BlockName = "BSSkin::Instance";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
	void GetPtrs(std::set<NiRef*>& ptrs) override;
};
} // namespace nifly
