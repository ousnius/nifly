/*
BodySlide and Outfit Studio
See the included LICENSE file
*/

#pragma once

#include "BasicTypes.hpp"
#include "Objects.hpp"

namespace nifly {
class NiNode : public CloneInherit<NiNode, NiAVObject> {
private:
	BlockRefArray<NiAVObject> childRefs;
	BlockRefArray<NiDynamicEffect> effectRefs;

public:
	static constexpr const char* BlockName = "NiNode";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	BlockRefArray<NiAVObject>& GetChildren();
	BlockRefArray<NiDynamicEffect>& GetEffects();
};

class BSFadeNode : public CloneInherit<BSFadeNode, NiNode> {
public:
	static constexpr const char* BlockName = "BSFadeNode";
	const char* GetBlockName() override { return BlockName; }
};

enum BSValueNodeFlags : uint8_t {
	BSVN_NONE = 0x0,
	BSVN_BILLBOARD_WORLD_Z = 0x1,
	BSVN_USE_PLAYER_ADJUST = 0x2
};

class BSValueNode : public CloneInherit<BSValueNode, NiNode> {
private:
	int value = 0;
	BSValueNodeFlags valueFlags = BSVN_NONE;

public:
	static constexpr const char* BlockName = "BSValueNode";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
};

class BSLeafAnimNode : public CloneInherit<BSLeafAnimNode, NiNode> {
public:
	static constexpr const char* BlockName = "BSLeafAnimNode";
	const char* GetBlockName() override { return BlockName; }
};

class BSTreeNode : public CloneInherit<BSTreeNode, NiNode> {
private:
	BlockRefArray<NiNode> bones1;
	BlockRefArray<NiNode> bones2;

public:
	static constexpr const char* BlockName = "BSTreeNode";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	BlockRefArray<NiNode>& GetBones1();
	BlockRefArray<NiNode>& GetBones2();
};

class BSOrderedNode : public CloneInherit<BSOrderedNode, NiNode> {
private:
	Vector4 alphaSortBound;
	bool isStaticBound = false;

public:
	static constexpr const char* BlockName = "BSOrderedNode";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
};

class BSMultiBoundData : public CloneInherit<BSMultiBoundData, NiObject> {};

class BSMultiBoundOBB : public CloneInherit<BSMultiBoundOBB, BSMultiBoundData> {
private:
	Vector3 center;
	Vector3 size;
	Matrix3 rotation;

public:
	static constexpr const char* BlockName = "BSMultiBoundOBB";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
};

class BSMultiBoundAABB : public CloneInherit<BSMultiBoundAABB, BSMultiBoundData> {
private:
	Vector3 center;
	Vector3 halfExtent;

public:
	static constexpr const char* BlockName = "BSMultiBoundAABB";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
};

class BSMultiBoundSphere : public CloneInherit<BSMultiBoundSphere, BSMultiBoundData> {
private:
	Vector3 center;
	float radius = 0.0f;

public:
	static constexpr const char* BlockName = "BSMultiBoundSphere";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
};

class BSMultiBound : public CloneInherit<BSMultiBound, NiObject> {
private:
	BlockRef<BSMultiBoundData> dataRef;

public:
	static constexpr const char* BlockName = "BSMultiBound";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
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

class BSMultiBoundNode : public CloneInherit<BSMultiBoundNode, NiNode> {
private:
	BlockRef<BSMultiBound> multiBoundRef;
	BSCPCullingType cullingMode = BSCP_CULL_NORMAL;

public:
	static constexpr const char* BlockName = "BSMultiBoundNode";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	int GetMultiBoundRef();
	void SetMultiBoundRef(int multBoundRef);
};

class BSRangeNode : public CloneInherit<BSRangeNode, NiNode> {
private:
	uint8_t min = 0;
	uint8_t max = 0;
	uint8_t current = 0;

public:
	static constexpr const char* BlockName = "BSRangeNode";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
};

class BSDebrisNode : public CloneInherit<BSDebrisNode, BSRangeNode> {
public:
	static constexpr const char* BlockName = "BSDebrisNode";
	const char* GetBlockName() override { return BlockName; }
};

class BSBlastNode : public CloneInherit<BSBlastNode, BSRangeNode> {
public:
	static constexpr const char* BlockName = "BSBlastNode";
	const char* GetBlockName() override { return BlockName; }
};

class BSDamageStage : public CloneInherit<BSDamageStage, BSBlastNode> {
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

class NiBillboardNode : public CloneInherit<NiBillboardNode, NiNode> {
private:
	BillboardMode billboardMode = ALWAYS_FACE_CAMERA;

public:
	static constexpr const char* BlockName = "NiBillboardNode";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
};

enum NiSwitchFlags : uint16_t { UPDATE_ONLY_ACTIVE_CHILD, UPDATE_CONTROLLERS };

class NiSwitchNode : public CloneInherit<NiSwitchNode, NiNode> {
private:
	NiSwitchFlags flags = UPDATE_ONLY_ACTIVE_CHILD;
	uint32_t index = 0;

public:
	static constexpr const char* BlockName = "NiSwitchNode";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
};

struct LODRange {
	float nearExtent = 0.0f;
	float farExtent = 0.0f;
};

class NiLODData : public CloneInherit<NiLODData, NiObject> {};

class NiRangeLODData : public CloneInherit<NiRangeLODData, NiLODData> {
private:
	Vector3 lodCenter;
	uint32_t numLODLevels = 0;
	std::vector<LODRange> lodLevels;

public:
	static constexpr const char* BlockName = "NiRangeLODData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
};

class NiScreenLODData : public CloneInherit<NiScreenLODData, NiLODData> {
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

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
};

class NiLODNode : public CloneInherit<NiLODNode, NiSwitchNode> {
private:
	BlockRef<NiLODData> lodLevelData;

public:
	static constexpr const char* BlockName = "NiLODNode";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	int GetLodLevelDataRef();
	void SetLodLevelDataRef(int dataRef);
};

class NiBone : public CloneInherit<NiBone, NiNode> {
public:
	static constexpr const char* BlockName = "NiBone";
	const char* GetBlockName() override { return BlockName; }
};

enum SortingMode { SORTING_INHERIT, SORTING_OFF };

class NiSortAdjustNode : public CloneInherit<NiSortAdjustNode, NiNode> {
private:
	SortingMode sortingMode = SORTING_INHERIT;

public:
	static constexpr const char* BlockName = "NiSortAdjustNode";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
};
} // namespace nifly
