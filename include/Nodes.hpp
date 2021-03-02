/*
BodySlide and Outfit Studio
See the included LICENSE file
*/

#pragma once

#include "BasicTypes.hpp"
#include "Objects.hpp"

namespace nifly {
class NiNode : public NiAVObject {
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
	NiNode* Clone() override { return new NiNode(*this); }

	BlockRefArray<NiAVObject>& GetChildren();
	BlockRefArray<NiDynamicEffect>& GetEffects();
};

class BSFadeNode : public NiNode {
public:
	static constexpr const char* BlockName = "BSFadeNode";
	const char* GetBlockName() override { return BlockName; }

	BSFadeNode* Clone() override { return new BSFadeNode(*this); }
};

enum BSValueNodeFlags : uint8_t { BSVN_NONE = 0x0, BSVN_BILLBOARD_WORLD_Z = 0x1, BSVN_USE_PLAYER_ADJUST = 0x2 };

class BSValueNode : public NiNode {
private:
	int value = 0;
	BSValueNodeFlags valueFlags = BSVN_NONE;

public:
	static constexpr const char* BlockName = "BSValueNode";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	BSValueNode* Clone() override { return new BSValueNode(*this); }
};

class BSLeafAnimNode : public NiNode {
public:
	static constexpr const char* BlockName = "BSLeafAnimNode";
	const char* GetBlockName() override { return BlockName; }
};

class BSTreeNode : public NiNode {
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
	BSTreeNode* Clone() override { return new BSTreeNode(*this); }

	BlockRefArray<NiNode>& GetBones1();
	BlockRefArray<NiNode>& GetBones2();
};

class BSOrderedNode : public NiNode {
private:
	Vector4 alphaSortBound;
	bool isStaticBound = false;

public:
	static constexpr const char* BlockName = "BSOrderedNode";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	BSOrderedNode* Clone() override { return new BSOrderedNode(*this); }
};

class BSMultiBoundData : public NiObject {};

class BSMultiBoundOBB : public BSMultiBoundData {
private:
	Vector3 center;
	Vector3 size;
	Matrix3 rotation;

public:
	static constexpr const char* BlockName = "BSMultiBoundOBB";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSMultiBoundOBB* Clone() override { return new BSMultiBoundOBB(*this); }
};

class BSMultiBoundAABB : public BSMultiBoundData {
private:
	Vector3 center;
	Vector3 halfExtent;

public:
	static constexpr const char* BlockName = "BSMultiBoundAABB";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSMultiBoundAABB* Clone() override { return new BSMultiBoundAABB(*this); }
};

class BSMultiBoundSphere : public BSMultiBoundData {
private:
	Vector3 center;
	float radius = 0.0f;

public:
	static constexpr const char* BlockName = "BSMultiBoundSphere";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSMultiBoundSphere* Clone() override { return new BSMultiBoundSphere(*this); }
};

class BSMultiBound : public NiObject {
private:
	BlockRef<BSMultiBoundData> dataRef;

public:
	static constexpr const char* BlockName = "BSMultiBound";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	BSMultiBound* Clone() override { return new BSMultiBound(*this); }

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

class BSMultiBoundNode : public NiNode {
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
	BSMultiBoundNode* Clone() override { return new BSMultiBoundNode(*this); }

	int GetMultiBoundRef();
	void SetMultiBoundRef(int multBoundRef);
};

class BSRangeNode : public NiNode {
private:
	uint8_t min = 0;
	uint8_t max = 0;
	uint8_t current = 0;

public:
	static constexpr const char* BlockName = "BSRangeNode";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	BSRangeNode* Clone() override { return new BSRangeNode(*this); }
};

class BSDebrisNode : public BSRangeNode {
public:
	static constexpr const char* BlockName = "BSDebrisNode";
	const char* GetBlockName() override { return BlockName; }

	BSDebrisNode* Clone() override { return new BSDebrisNode(*this); }
};

class BSBlastNode : public BSRangeNode {
public:
	static constexpr const char* BlockName = "BSBlastNode";
	const char* GetBlockName() override { return BlockName; }

	BSBlastNode* Clone() override { return new BSBlastNode(*this); }
};

class BSDamageStage : public BSBlastNode {
public:
	static constexpr const char* BlockName = "BSDamageStage";
	const char* GetBlockName() override { return BlockName; }

	BSDamageStage* Clone() override { return new BSDamageStage(*this); }
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

class NiBillboardNode : public NiNode {
private:
	BillboardMode billboardMode = ALWAYS_FACE_CAMERA;

public:
	static constexpr const char* BlockName = "NiBillboardNode";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	NiBillboardNode* Clone() override { return new NiBillboardNode(*this); }
};

enum NiSwitchFlags : uint16_t { UPDATE_ONLY_ACTIVE_CHILD, UPDATE_CONTROLLERS };

class NiSwitchNode : public NiNode {
private:
	NiSwitchFlags flags = UPDATE_ONLY_ACTIVE_CHILD;
	uint32_t index = 0;

public:
	static constexpr const char* BlockName = "NiSwitchNode";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	NiSwitchNode* Clone() override { return new NiSwitchNode(*this); }
};

struct LODRange {
	float nearExtent = 0.0f;
	float farExtent = 0.0f;
};

class NiLODData : public NiObject {};

class NiRangeLODData : public NiLODData {
private:
	Vector3 lodCenter;
	uint32_t numLODLevels = 0;
	std::vector<LODRange> lodLevels;

public:
	static constexpr const char* BlockName = "NiRangeLODData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	NiRangeLODData* Clone() override { return new NiRangeLODData(*this); }
};

class NiScreenLODData : public NiLODData {
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

	NiScreenLODData* Clone() override { return new NiScreenLODData(*this); }
};

class NiLODNode : public NiSwitchNode {
private:
	BlockRef<NiLODData> lodLevelData;

public:
	static constexpr const char* BlockName = "NiLODNode";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	NiLODNode* Clone() override { return new NiLODNode(*this); }

	int GetLodLevelDataRef();
	void SetLodLevelDataRef(int dataRef);
};

class NiBone : public NiNode {
public:
	static constexpr const char* BlockName = "NiBone";
	const char* GetBlockName() override { return BlockName; }

	NiBone* Clone() override { return new NiBone(*this); }
};

enum SortingMode { SORTING_INHERIT, SORTING_OFF };

class NiSortAdjustNode : public NiNode {
private:
	SortingMode sortingMode = SORTING_INHERIT;

public:
	static constexpr const char* BlockName = "NiSortAdjustNode";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	NiSortAdjustNode* Clone() override { return new NiSortAdjustNode(*this); }
};
} // namespace nifly
