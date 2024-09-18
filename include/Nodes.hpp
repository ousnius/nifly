/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#pragma once

#include "BasicTypes.hpp"
#include "Objects.hpp"

namespace nifly {
class NiNode : public NiCloneableStreamable<NiNode, NiAVObject> {
public:
	NiBlockRefArray<NiAVObject> childRefs;
	NiBlockRefArray<NiDynamicEffect> effectRefs;

	static constexpr const char* BlockName = "NiNode";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class BSFadeNode : public NiCloneable<BSFadeNode, NiNode> {
public:
	static constexpr const char* BlockName = "BSFadeNode";
	const char* GetBlockName() override { return BlockName; }
};

enum BSValueNodeFlags : uint8_t {
	BSVN_NONE = 0x0,
	BSVN_BILLBOARD_WORLD_Z = 0x1,
	BSVN_USE_PLAYER_ADJUST = 0x2
};

class BSValueNode : public NiCloneableStreamable<BSValueNode, NiNode> {
public:
	int value = 0;
	BSValueNodeFlags valueFlags = BSVN_NONE;

	static constexpr const char* BlockName = "BSValueNode";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSLeafAnimNode : public NiCloneable<BSLeafAnimNode, NiNode> {
public:
	static constexpr const char* BlockName = "BSLeafAnimNode";
	const char* GetBlockName() override { return BlockName; }
};

class BSTreeNode : public NiCloneableStreamable<BSTreeNode, NiNode> {
public:
	NiBlockRefArray<NiNode> bones1;
	NiBlockRefArray<NiNode> bones2;

	static constexpr const char* BlockName = "BSTreeNode";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class BSOrderedNode : public NiCloneableStreamable<BSOrderedNode, NiNode> {
public:
	Vector4 alphaSortBound;
	bool isStaticBound = false;

	static constexpr const char* BlockName = "BSOrderedNode";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSMultiBoundData : public NiCloneable<BSMultiBoundData, NiObject> {};

class BSMultiBoundOBB : public NiCloneableStreamable<BSMultiBoundOBB, BSMultiBoundData> {
public:
	Vector3 center;
	Vector3 size;
	Matrix3 rotation;

	static constexpr const char* BlockName = "BSMultiBoundOBB";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSMultiBoundAABB : public NiCloneableStreamable<BSMultiBoundAABB, BSMultiBoundData> {
public:
	Vector3 center;
	Vector3 halfExtent;

	static constexpr const char* BlockName = "BSMultiBoundAABB";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSMultiBoundSphere : public NiCloneableStreamable<BSMultiBoundSphere, BSMultiBoundData> {
public:
	Vector3 center;
	float radius = 0.0f;

	static constexpr const char* BlockName = "BSMultiBoundSphere";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSMultiBound : public NiCloneableStreamable<BSMultiBound, NiObject> {
public:
	NiBlockRef<BSMultiBoundData> dataRef;

	static constexpr const char* BlockName = "BSMultiBound";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

enum BSCPCullingType : uint32_t {
	BSCP_CULL_NORMAL,
	BSCP_CULL_ALLPASS,
	BSCP_CULL_ALLFAIL,
	BSCP_CULL_IGNOREMULTIBOUNDS,
	BSCP_CULL_FORCEMULTIBOUNDSNOUPDATE
};

class BSMultiBoundNode : public NiCloneableStreamable<BSMultiBoundNode, NiNode> {
public:
	NiBlockRef<BSMultiBound> multiBoundRef;
	BSCPCullingType cullingMode = BSCP_CULL_NORMAL;

	static constexpr const char* BlockName = "BSMultiBoundNode";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

struct BSResourceID {
    uint32_t fileHash = 0;
    char extension[4];
    uint32_t dirHash = 0;
};

#pragma pack(push, 1)
struct BSDistantObjectUnknown {
	uint64_t unknown1 = 0;
	uint32_t unknown2 = 0;
};
#pragma pack(pop)

struct BSDistantObjectInstance {
	BSResourceID resourceID;
	NiVector<BSDistantObjectUnknown> unknownData;
	NiVector<Matrix4> transforms;

	void Sync(NiStreamReversible& stream) {
		stream.Sync(resourceID);
		unknownData.Sync(stream);
		transforms.Sync(stream);
	}
};

struct BSShaderTextureArray {
	uint8_t unknownByte = 1;
	NiSyncVector<BSTextureArray> textureArrays;

	void Sync(NiStreamReversible& stream) {
		stream.Sync(unknownByte);
		textureArrays.Sync(stream);
	}
};

class BSDistantObjectInstancedNode : public NiCloneableStreamable<BSDistantObjectInstancedNode, BSMultiBoundNode> {
public:
	NiSyncVector<BSDistantObjectInstance> instances;
	BSShaderTextureArray textureArrays[3]{};

	static constexpr const char* BlockName = "BSDistantObjectInstancedNode";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSRangeNode : public NiCloneableStreamable<BSRangeNode, NiNode> {
public:
	uint8_t min = 0;
	uint8_t max = 0;
	uint8_t current = 0;

	static constexpr const char* BlockName = "BSRangeNode";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSDebrisNode : public NiCloneable<BSDebrisNode, BSRangeNode> {
public:
	static constexpr const char* BlockName = "BSDebrisNode";
	const char* GetBlockName() override { return BlockName; }
};

class BSBlastNode : public NiCloneable<BSBlastNode, BSRangeNode> {
public:
	static constexpr const char* BlockName = "BSBlastNode";
	const char* GetBlockName() override { return BlockName; }
};

class BSDamageStage : public NiCloneable<BSDamageStage, BSBlastNode> {
public:
	static constexpr const char* BlockName = "BSDamageStage";
	const char* GetBlockName() override { return BlockName; }
};

struct UnkMaterialStruct {
	uint32_t biomeFormID = 0;
	uint32_t dirHash = 0;
	uint32_t fileHash = 0;
	std::string mat; // mat\0

	void Sync(NiStreamReversible& stream);
};

struct BSWaterReferenceStruct {
    Matrix4 transform;
    BSResourceID resourceID;
    uint32_t unkInt1 = 0;
    NiString material;

	void Sync(NiStreamReversible& stream);
};

struct BSWeakReference {
	uint32_t formID = 0;
	BSResourceID resourceID;

	uint32_t numTransforms = 0;
	std::vector<Matrix4> transforms;

	uint32_t numMaterials;
	std::vector<UnkMaterialStruct> unkMaterials;

	void Sync(NiStreamReversible& stream);
};

class BSWeakReferenceNode : public NiCloneableStreamable<BSWeakReferenceNode, NiNode> {
public:
	uint32_t numWeakRefs = 0;
	std::vector<BSWeakReference> weakRefs;

	uint32_t unkInt1 = 0;
	uint32_t numWaterRefs = 0;
	std::vector<BSWaterReferenceStruct> waterRefs;

	static constexpr const char* BlockName = "BSWeakReferenceNode";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSFaceGenNiNode : public NiCloneableStreamable<BSFaceGenNiNode, NiNode> {
public:
	uint16_t unkShort = 0;

	static constexpr const char* BlockName = "BSFaceGenNiNode";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

enum BillboardMode : uint16_t {
	ALWAYS_FACE_CAMERA,
	ROTATE_ABOUT_UP,
	RIGID_FACE_CAMERA,
	ALWAYS_FACE_CENTER,
	RIGID_FACE_CENTER,
	BSROTATE_ABOUT_UP,
	ROTATE_ABOUT_UP2 = 9
};

class NiBillboardNode : public NiCloneableStreamable<NiBillboardNode, NiNode> {
public:
	BillboardMode billboardMode = ALWAYS_FACE_CAMERA;

	static constexpr const char* BlockName = "NiBillboardNode";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

enum NiSwitchFlags : uint16_t { UPDATE_ONLY_ACTIVE_CHILD, UPDATE_CONTROLLERS };

class NiSwitchNode : public NiCloneableStreamable<NiSwitchNode, NiNode> {
public:
	NiSwitchFlags flags = UPDATE_ONLY_ACTIVE_CHILD;
	uint32_t index = 0;

	static constexpr const char* BlockName = "NiSwitchNode";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

struct LODRange {
	float nearExtent = 0.0f;
	float farExtent = 0.0f;
};

class NiLODData : public NiCloneable<NiLODData, NiObject> {};

class NiRangeLODData : public NiCloneableStreamable<NiRangeLODData, NiLODData> {
public:
	Vector3 lodCenter;
	NiVector<LODRange> lodLevels;

	static constexpr const char* BlockName = "NiRangeLODData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiScreenLODData : public NiCloneableStreamable<NiScreenLODData, NiLODData> {
public:
	Vector3 boundCenter;
	float boundRadius = 0.0f;
	Vector3 worldCenter;
	float worldRadius = 0.0f;
	NiVector<float> proportionLevels;

	static constexpr const char* BlockName = "NiScreenLODData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiLODNode : public NiCloneableStreamable<NiLODNode, NiSwitchNode> {
public:
	NiBlockRef<NiLODData> lodLevelData;

	static constexpr const char* BlockName = "NiLODNode";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class NiBone : public NiCloneable<NiBone, NiNode> {
public:
	static constexpr const char* BlockName = "NiBone";
	const char* GetBlockName() override { return BlockName; }
};

enum SortingMode { SORTING_INHERIT, SORTING_OFF };

class NiSortAdjustNode : public NiCloneableStreamable<NiSortAdjustNode, NiNode> {
public:
	SortingMode sortingMode = SORTING_INHERIT;

	static constexpr const char* BlockName = "NiSortAdjustNode";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};
} // namespace nifly
