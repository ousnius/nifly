/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#pragma once

#include "BasicTypes.hpp"
#include "Objects.hpp"

namespace nifly {
class NiNode : public NiObjectCRTP<NiNode, NiAVObject, true> {
private:
	BlockRefArray<NiAVObject> childRefs;
	BlockRefArray<NiDynamicEffect> effectRefs;

public:
	static constexpr const char* BlockName = "NiNode";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	BlockRefArray<NiAVObject>& GetChildren();
	BlockRefArray<NiDynamicEffect>& GetEffects();
};

class BSFadeNode : public NiObjectCRTP<BSFadeNode, NiNode> {
public:
	static constexpr const char* BlockName = "BSFadeNode";
	const char* GetBlockName() override { return BlockName; }
};

enum BSValueNodeFlags : uint8_t {
	BSVN_NONE = 0x0,
	BSVN_BILLBOARD_WORLD_Z = 0x1,
	BSVN_USE_PLAYER_ADJUST = 0x2
};

class BSValueNode : public NiObjectCRTP<BSValueNode, NiNode, true> {
private:
	int value = 0;
	BSValueNodeFlags valueFlags = BSVN_NONE;

public:
	static constexpr const char* BlockName = "BSValueNode";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSLeafAnimNode : public NiObjectCRTP<BSLeafAnimNode, NiNode> {
public:
	static constexpr const char* BlockName = "BSLeafAnimNode";
	const char* GetBlockName() override { return BlockName; }
};

class BSTreeNode : public NiObjectCRTP<BSTreeNode, NiNode, true> {
private:
	BlockRefArray<NiNode> bones1;
	BlockRefArray<NiNode> bones2;

public:
	static constexpr const char* BlockName = "BSTreeNode";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	BlockRefArray<NiNode>& GetBones1();
	BlockRefArray<NiNode>& GetBones2();
};

class BSOrderedNode : public NiObjectCRTP<BSOrderedNode, NiNode, true> {
private:
	Vector4 alphaSortBound;
	bool isStaticBound = false;

public:
	static constexpr const char* BlockName = "BSOrderedNode";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSMultiBoundData : public NiObjectCRTP<BSMultiBoundData, NiObject> {};

class BSMultiBoundOBB : public NiObjectCRTP<BSMultiBoundOBB, BSMultiBoundData, true> {
private:
	Vector3 center;
	Vector3 size;
	Matrix3 rotation;

public:
	static constexpr const char* BlockName = "BSMultiBoundOBB";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSMultiBoundAABB : public NiObjectCRTP<BSMultiBoundAABB, BSMultiBoundData, true> {
private:
	Vector3 center;
	Vector3 halfExtent;

public:
	static constexpr const char* BlockName = "BSMultiBoundAABB";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSMultiBoundSphere : public NiObjectCRTP<BSMultiBoundSphere, BSMultiBoundData, true> {
private:
	Vector3 center;
	float radius = 0.0f;

public:
	static constexpr const char* BlockName = "BSMultiBoundSphere";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSMultiBound : public NiObjectCRTP<BSMultiBound, NiObject, true> {
private:
	BlockRef<BSMultiBoundData> dataRef;

public:
	static constexpr const char* BlockName = "BSMultiBound";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	int GetDataRef();
	void SetDataRef(int datRef);
};

enum BSCPCullingType : uint32_t {
	BSCP_CULL_NORMAL,
	BSCP_CULL_ALLPASS,
	BSCP_CULL_ALLFAIL,
	BSCP_CULL_IGNOREMULTIBOUNDS,
	BSCP_CULL_FORCEMULTIBOUNDSNOUPDATE
};

class BSMultiBoundNode : public NiObjectCRTP<BSMultiBoundNode, NiNode, true> {
private:
	BlockRef<BSMultiBound> multiBoundRef;
	BSCPCullingType cullingMode = BSCP_CULL_NORMAL;

public:
	static constexpr const char* BlockName = "BSMultiBoundNode";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	int GetMultiBoundRef();
	void SetMultiBoundRef(int multBoundRef);
};

class BSRangeNode : public NiObjectCRTP<BSRangeNode, NiNode, true> {
private:
	uint8_t min = 0;
	uint8_t max = 0;
	uint8_t current = 0;

public:
	static constexpr const char* BlockName = "BSRangeNode";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSDebrisNode : public NiObjectCRTP<BSDebrisNode, BSRangeNode> {
public:
	static constexpr const char* BlockName = "BSDebrisNode";
	const char* GetBlockName() override { return BlockName; }
};

class BSBlastNode : public NiObjectCRTP<BSBlastNode, BSRangeNode> {
public:
	static constexpr const char* BlockName = "BSBlastNode";
	const char* GetBlockName() override { return BlockName; }
};

class BSDamageStage : public NiObjectCRTP<BSDamageStage, BSBlastNode> {
public:
	static constexpr const char* BlockName = "BSDamageStage";
	const char* GetBlockName() override { return BlockName; }
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

class NiBillboardNode : public NiObjectCRTP<NiBillboardNode, NiNode, true> {
private:
	BillboardMode billboardMode = ALWAYS_FACE_CAMERA;

public:
	static constexpr const char* BlockName = "NiBillboardNode";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

enum NiSwitchFlags : uint16_t { UPDATE_ONLY_ACTIVE_CHILD, UPDATE_CONTROLLERS };

class NiSwitchNode : public NiObjectCRTP<NiSwitchNode, NiNode, true> {
private:
	NiSwitchFlags flags = UPDATE_ONLY_ACTIVE_CHILD;
	uint32_t index = 0;

public:
	static constexpr const char* BlockName = "NiSwitchNode";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

struct LODRange {
	float nearExtent = 0.0f;
	float farExtent = 0.0f;
};

class NiLODData : public NiObjectCRTP<NiLODData, NiObject> {};

class NiRangeLODData : public NiObjectCRTP<NiRangeLODData, NiLODData, true> {
private:
	Vector3 lodCenter;
	uint32_t numLODLevels = 0;
	std::vector<LODRange> lodLevels;

public:
	static constexpr const char* BlockName = "NiRangeLODData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiScreenLODData : public NiObjectCRTP<NiScreenLODData, NiLODData, true> {
private:
	Vector3 boundCenter;
	float boundRadius = 0.0f;
	Vector3 worldCenter;
	float worldRadius = 0.0f;
	uint32_t numProportions = 0;
	std::vector<float> proportionLevels;

public:
	static constexpr const char* BlockName = "NiScreenLODData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiLODNode : public NiObjectCRTP<NiLODNode, NiSwitchNode, true> {
private:
	BlockRef<NiLODData> lodLevelData;

public:
	static constexpr const char* BlockName = "NiLODNode";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	int GetLodLevelDataRef();
	void SetLodLevelDataRef(int dataRef);
};

class NiBone : public NiObjectCRTP<NiBone, NiNode> {
public:
	static constexpr const char* BlockName = "NiBone";
	const char* GetBlockName() override { return BlockName; }
};

enum SortingMode { SORTING_INHERIT, SORTING_OFF };

class NiSortAdjustNode : public NiObjectCRTP<NiSortAdjustNode, NiNode, true> {
private:
	SortingMode sortingMode = SORTING_INHERIT;

public:
	static constexpr const char* BlockName = "NiSortAdjustNode";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};
} // namespace nifly
