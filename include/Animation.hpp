/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#pragma once

#include "BasicTypes.hpp"
#include "ExtraData.hpp"
#include "Keys.hpp"

namespace nifly {
struct QuatTransform {
	Vector3 translation;
	Quaternion rotation;
	float scale = 1.0f;
	bool trsValid[3]{};

	void Sync(NiStreamReversible& stream) {
		stream.Sync(translation);
		stream.Sync(rotation);
		stream.Sync(scale);

		if (stream.GetVersion().File() < V10_1_0_110)
			stream.Sync(trsValid);
	}
};

class NiKeyframeData : public NiCloneableStreamable<NiKeyframeData, NiObject> {
public:
	NiKeyType rotationType = NO_INTERP;
	std::vector<NiAnimationKey<Quaternion>> quaternionKeys;
	NiAnimationKeyGroup<float> xRotations;
	NiAnimationKeyGroup<float> yRotations;
	NiAnimationKeyGroup<float> zRotations;
	NiAnimationKeyGroup<Vector3> translations;
	NiAnimationKeyGroup<float> scales;

	static constexpr const char* BlockName = "NiKeyframeData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiTransformData : public NiCloneable<NiTransformData, NiKeyframeData> {
public:
	static constexpr const char* BlockName = "NiTransformData";

	const char* GetBlockName() override { return BlockName; }
};

class NiPosData : public NiCloneableStreamable<NiPosData, NiObject> {
public:
	NiAnimationKeyGroup<Vector3> data;

	static constexpr const char* BlockName = "NiPosData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiBoolData : public NiCloneableStreamable<NiBoolData, NiObject> {
public:
	NiAnimationKeyGroup<uint8_t> data;

	static constexpr const char* BlockName = "NiBoolData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiFloatData : public NiCloneableStreamable<NiFloatData, NiObject> {
public:
	NiAnimationKeyGroup<float> data;

	static constexpr const char* BlockName = "NiFloatData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiBSplineData : public NiCloneableStreamable<NiBSplineData, NiObject> {
public:
	NiVector<float> floatControlPoints;
	NiVector<short> shortControlPoints;

	static constexpr const char* BlockName = "NiBSplineData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiBSplineBasisData : public NiCloneableStreamable<NiBSplineBasisData, NiObject> {
public:
	uint32_t numControlPoints = 0;

	static constexpr const char* BlockName = "NiBSplineBasisData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiInterpolator : public NiCloneable<NiInterpolator, NiObject> {};

class NiBSplineInterpolator : public NiCloneableStreamable<NiBSplineInterpolator, NiInterpolator> {
public:
	float startTime = 0.0f;
	float stopTime = 0.0f;
	NiBlockRef<NiBSplineData> splineDataRef;
	NiBlockRef<NiBSplineBasisData> basisDataRef;

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class NiBSplineFloatInterpolator : public NiCloneable<NiBSplineFloatInterpolator, NiBSplineInterpolator> {};

class NiBSplineCompFloatInterpolator
	: public NiCloneableStreamable<NiBSplineCompFloatInterpolator, NiBSplineFloatInterpolator> {
public:
	float base = 0.0f;
	uint32_t offset = 0;
	float bias = 0.0f;
	float multiplier = 0.0f;

	static constexpr const char* BlockName = "NiBSplineCompFloatInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiBSplinePoint3Interpolator
	: public NiCloneableStreamable<NiBSplinePoint3Interpolator, NiBSplineInterpolator> {
public:
	Vector3 value = NiVec3Min;
	uint32_t handle = NiUShortMax;

	void Sync(NiStreamReversible& stream);
};

class NiBSplineCompPoint3Interpolator
	: public NiCloneableStreamable<NiBSplineCompPoint3Interpolator, NiBSplinePoint3Interpolator> {
public:
	float positionOffset = NiFloatMax;
	float positionHalfRange = NiFloatMax;

	static constexpr const char* BlockName = "NiBSplineCompPoint3Interpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiBSplineTransformInterpolator
	: public NiCloneableStreamable<NiBSplineTransformInterpolator, NiBSplineInterpolator> {
public:
	Vector3 translation;
	Quaternion rotation;
	float scale = 1.0f;

	uint32_t translationOffset = 0;
	uint32_t rotationOffset = 0;
	uint32_t scaleOffset = 0;

	static constexpr const char* BlockName = "NiBSplineTransformInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiBSplineCompTransformInterpolator
	: public NiCloneableStreamable<NiBSplineCompTransformInterpolator, NiBSplineTransformInterpolator> {
public:
	float translationBias = 0.0f;
	float translationMultiplier = 0.0f;
	float rotationBias = 0.0f;
	float rotationMultiplier = 0.0f;
	float scaleBias = 0.0f;
	float scaleMultiplier = 0.0f;

	static constexpr const char* BlockName = "NiBSplineCompTransformInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

enum InterpBlendFlags : uint8_t { INTERP_BLEND_NONE = 0x00, INTERP_BLEND_MANAGER_CONTROLLED = 0x01 };

class InterpBlendItem {
public:
	NiBlockRef<NiInterpolator> interpolatorRef;
	float weight = 0.0f;
	float normalizedWeight = 0.0f;
	uint32_t priorityInt = 0;
	uint8_t priority = 0;
	float easeSpinner = 0.0f;

	void Sync(NiStreamReversible& stream);
};

class NiBlendInterpolator : public NiCloneableStreamable<NiBlendInterpolator, NiInterpolator> {
public:
	InterpBlendFlags flags = INTERP_BLEND_MANAGER_CONTROLLED;
	uint16_t arraySize = 0;
	uint16_t arrayGrowBy = 0;
	float weightThreshold = 0.0f;

	uint16_t interpCount = 0;
	uint8_t singleIndex = NiByteMax;
	uint16_t singleIndexShort = NiUShortMax;
	char highPriority = NiCharMin;
	int highPriorityInt = NiIntMin;
	char nextHighPriority = NiCharMin;
	int nextHighPriorityInt = NiIntMin;
	float singleTime = NiFloatMin;
	float highWeightsSum = NiFloatMin;
	float nextHighWeightsSum = NiFloatMin;
	float highEaseSpinner = NiFloatMin;
	std::vector<InterpBlendItem> interpItems;

	bool managerControlled = false;
	bool onlyUseHighestWeight = false;
	NiBlockRef<NiInterpolator> singleInterpolatorRef;

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class NiBlendBoolInterpolator : public NiCloneableStreamable<NiBlendBoolInterpolator, NiBlendInterpolator> {
public:
	bool value = false;

	static constexpr const char* BlockName = "NiBlendBoolInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiBlendFloatInterpolator : public NiCloneableStreamable<NiBlendFloatInterpolator, NiBlendInterpolator> {
public:
	float value = 0.0f;

	static constexpr const char* BlockName = "NiBlendFloatInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiBlendPoint3Interpolator
	: public NiCloneableStreamable<NiBlendPoint3Interpolator, NiBlendInterpolator> {
public:
	Vector3 point;

	static constexpr const char* BlockName = "NiBlendPoint3Interpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiBlendTransformInterpolator
	: public NiCloneableStreamable<NiBlendTransformInterpolator, NiBlendInterpolator> {
public:
	QuatTransform value;

	static constexpr const char* BlockName = "NiBlendTransformInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiKeyBasedInterpolator : public NiInterpolator {};

class NiBoolInterpolator : public NiCloneableStreamable<NiBoolInterpolator, NiKeyBasedInterpolator> {
public:
	uint8_t boolValue = 0;
	NiBlockRef<NiBoolData> dataRef;

	static constexpr const char* BlockName = "NiBoolInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class NiBoolTimelineInterpolator : public NiCloneable<NiBoolTimelineInterpolator, NiBoolInterpolator> {
public:
	static constexpr const char* BlockName = "NiBoolTimelineInterpolator";
	const char* GetBlockName() override { return BlockName; }
};

class NiFloatInterpolator : public NiCloneableStreamable<NiFloatInterpolator, NiKeyBasedInterpolator> {
public:
	float floatValue = 0.0f;
	NiBlockRef<NiFloatData> dataRef;

	static constexpr const char* BlockName = "NiFloatInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class NiTransformInterpolator
	: public NiCloneableStreamable<NiTransformInterpolator, NiKeyBasedInterpolator> {
public:
	Vector3 translation;
	Quaternion rotation;
	float scale = 0.0f;
	NiBlockRef<NiTransformData> dataRef;

	static constexpr const char* BlockName = "NiTransformInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class BSRotAccumTransfInterpolator
	: public NiCloneable<BSRotAccumTransfInterpolator, NiTransformInterpolator> {
public:
	static constexpr const char* BlockName = "BSRotAccumTransfInterpolator";
	const char* GetBlockName() override { return BlockName; }
};

class NiPoint3Interpolator : public NiCloneableStreamable<NiPoint3Interpolator, NiKeyBasedInterpolator> {
public:
	Vector3 point3Value;
	NiBlockRef<NiPosData> dataRef;

	static constexpr const char* BlockName = "NiPoint3Interpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

enum PathFlags : uint16_t {
	PATH_NONE = 0x0000,
	PATH_CVDATANEEDSUPDATE = 0x0001,
	PATH_CURVETYPEOPEN = 0x0002,
	PATH_ALLOWFLIP = 0x0004,
	PATH_BANK = 0x0008,
	PATH_CONSTANTVELOCITY = 0x0016,
	PATH_FOLLOW = 0x0032,
	PATH_FLIP = 0x0064
};

class NiPathInterpolator : public NiCloneableStreamable<NiPathInterpolator, NiKeyBasedInterpolator> {
private:
	PathFlags pathFlags = static_cast<PathFlags>(PATH_CVDATANEEDSUPDATE | PATH_CURVETYPEOPEN);
	int bankDir = 1;
	float maxBankAngle = 0.0f;
	float smoothing = 0.0f;
	uint16_t followAxis = 0;

	NiBlockRef<NiPosData> pathDataRef;
	NiBlockRef<NiFloatData> percentDataRef;

public:
	static constexpr const char* BlockName = "NiPathInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

enum LookAtFlags : uint16_t {
	LOOK_X_AXIS = 0x0000,
	LOOK_FLIP = 0x0001,
	LOOK_Y_AXIS = 0x0002,
	LOOK_Z_AXIS = 0x0004
};

class NiNode;

class NiLookAtInterpolator : public NiCloneableStreamable<NiLookAtInterpolator, NiInterpolator> {
public:
	LookAtFlags flags = LOOK_X_AXIS;
	NiBlockPtr<NiNode> lookAtRef;

	NiStringRef lookAtName;

	QuatTransform transform;
	NiBlockRef<NiPoint3Interpolator> translateInterpRef;
	NiBlockRef<NiFloatInterpolator> rollInterpRef;
	NiBlockRef<NiFloatInterpolator> scaleInterpRef;

	static constexpr const char* BlockName = "NiLookAtInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
	void GetPtrs(std::set<NiPtr*>& ptrs) override;
};

struct BSTreadTransformData {
	Vector3 translation;
	Quaternion rotation;
	float scale = 1.0f;
};

struct BSTreadTransform {
	NiStringRef name;
	BSTreadTransformData transform1;
	BSTreadTransformData transform2;

	void Sync(NiStreamReversible& stream) {
		name.Sync(stream);
		stream.Sync(transform1);
		stream.Sync(transform2);
	}

	void GetStringRefs(std::vector<NiStringRef*>& refs) { refs.emplace_back(&name); }
};

class BSTreadTransfInterpolator : public NiCloneableStreamable<BSTreadTransfInterpolator, NiInterpolator> {
public:
	NiSyncVector<BSTreadTransform> treadTransforms;
	NiBlockRef<NiFloatData> dataRef;

	static constexpr const char* BlockName = "BSTreadTransfInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class NiObjectNET;

class NiTimeController : public NiCloneableStreamable<NiTimeController, NiObject> {
public:
	NiBlockRef<NiTimeController> nextControllerRef;
	uint16_t flags = 0x000C;
	float frequency = 1.0f;
	float phase = 0.0f;
	float startTime = NiFloatMax;
	float stopTime = NiFloatMin;
	NiBlockPtr<NiObjectNET> targetRef;

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
	void GetPtrs(std::set<NiPtr*>& ptrs) override;
};

class NiLookAtController : public NiCloneableStreamable<NiLookAtController, NiTimeController> {
public:
	LookAtFlags lookAtFlags = LOOK_X_AXIS;
	NiBlockPtr<NiNode> lookAtNodePtr;

	static constexpr const char* BlockName = "NiLookAtController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<NiPtr*>& ptrs) override;
};

class NiPathController : public NiCloneableStreamable<NiPathController, NiTimeController> {
public:
	PathFlags pathFlags = PATH_NONE;
	int bankDir = 1;
	float maxBankAngle = 0.0f;
	float smoothing = 0.0f;
	uint16_t followAxis = 0;
	NiBlockRef<NiPosData> pathDataRef;
	NiBlockRef<NiFloatData> percentDataRef;

	static constexpr const char* BlockName = "NiPathController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class NiPSysResetOnLoopCtlr : public NiCloneable<NiPSysResetOnLoopCtlr, NiTimeController> {
public:
	static constexpr const char* BlockName = "NiPSysResetOnLoopCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiUVData : public NiCloneableStreamable<NiUVData, NiObject> {
public:
	NiAnimationKeyGroup<float> uTrans;
	NiAnimationKeyGroup<float> vTrans;
	NiAnimationKeyGroup<float> uScale;
	NiAnimationKeyGroup<float> vScale;

	static constexpr const char* BlockName = "NiUVData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiUVController : public NiCloneableStreamable<NiUVController, NiTimeController> {
public:
	uint16_t textureSet = 0;
	NiBlockRef<NiUVData> dataRef;

	static constexpr const char* BlockName = "NiUVController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class BSFrustumFOVController : public NiCloneableStreamable<BSFrustumFOVController, NiTimeController> {
public:
	NiBlockRef<NiInterpolator> interpolatorRef;

	static constexpr const char* BlockName = "BSFrustumFOVController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class BSLagBoneController : public NiCloneableStreamable<BSLagBoneController, NiTimeController> {
public:
	float linearVelocity = 0.0f;
	float linearRotation = 0.0f;
	float maxDistance = 0.0f;

	static constexpr const char* BlockName = "BSLagBoneController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSShaderProperty;

class BSProceduralLightningController
	: public NiCloneableStreamable<BSProceduralLightningController, NiTimeController> {
public:
	NiBlockRef<NiInterpolator> generationInterpRef;
	NiBlockRef<NiInterpolator> mutationInterpRef;
	NiBlockRef<NiInterpolator> subdivisionInterpRef;
	NiBlockRef<NiInterpolator> numBranchesInterpRef;
	NiBlockRef<NiInterpolator> numBranchesVarInterpRef;
	NiBlockRef<NiInterpolator> lengthInterpRef;
	NiBlockRef<NiInterpolator> lengthVarInterpRef;
	NiBlockRef<NiInterpolator> widthInterpRef;
	NiBlockRef<NiInterpolator> arcOffsetInterpRef;

	uint16_t subdivisions = 0;
	uint16_t numBranches = 0;
	uint16_t numBranchesPerVariation = 0;

	float length = 0.0f;
	float lengthVariation = 0.0f;
	float width = 0.0f;
	float childWidthMult = 0.0f;
	float arcOffset = 0.0f;
	bool fadeMainBolt = 0.0f;
	bool fadeChildBolts = 0.0f;
	bool animateArcOffset = 0.0f;

	NiBlockRef<BSShaderProperty> shaderPropertyRef;

	static constexpr const char* BlockName = "BSProceduralLightningController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class NiBoneLODController : public NiCloneableStreamable<NiBoneLODController, NiTimeController> {
public:
	uint32_t lod = 0;
	uint32_t numLODs = 0;
	NiSyncVector<NiBlockPtrArray<NiNode>> boneArrays;

	static constexpr const char* BlockName = "NiBoneLODController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<NiPtr*>& ptrs) override;
};

class NiBSBoneLODController : public NiCloneable<NiBSBoneLODController, NiBoneLODController> {
public:
	static constexpr const char* BlockName = "NiBSBoneLODController";
	const char* GetBlockName() override { return BlockName; }
};

struct Morph {
	NiStringRef frameName;
	float legacyWeight = 0.0f;
	std::vector<Vector3> vectors;

	void Sync(NiStreamReversible& stream, uint32_t numVerts) {
		if (stream.GetVersion().File() >= V10_1_0_106)
			frameName.Sync(stream);

		if (stream.GetVersion().File() >= V10_1_0_104 && stream.GetVersion().File() < V20_1_0_3 && stream.GetVersion().Stream() < 10)
			stream.Sync(legacyWeight);

		vectors.resize(numVerts);
		for (uint32_t i = 0; i < numVerts; i++)
			stream.Sync(vectors[i]);
	}

	void GetStringRefs(std::vector<NiStringRef*>& refs) { refs.emplace_back(&frameName); }
};

class NiMorphData : public NiCloneableStreamable<NiMorphData, NiObject> {
private:
	uint32_t numMorphs = 0;
	std::vector<Morph> morphs;

public:
	uint32_t numVertices = 0;
	uint8_t relativeTargets = 1;

	static constexpr const char* BlockName = "NiMorphData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;

	std::vector<Morph> GetMorphs() const;
	void SetMorphs(const uint32_t numVerts, const std::vector<Morph>& m);
};

class NiInterpController : public NiCloneableStreamable<NiInterpController, NiTimeController> {
public:
	bool managerControlled = false;

	void Sync(NiStreamReversible& stream);
};

class MorphWeight {
public:
	NiBlockRef<NiInterpolator> interpRef;
	float weight = 0.0f;

	void Sync(NiStreamReversible& stream) {
		interpRef.Sync(stream);
		stream.Sync(weight);
	}

	void GetChildRefs(std::set<NiRef*>& refs) { refs.insert(&interpRef); }
	void GetChildIndices(std::vector<uint32_t>& indices) { indices.push_back(interpRef.index); }
};

enum GeomMorpherFlags : uint16_t { GM_UPDATE_NORMALS_DISABLED, GM_UPDATE_NORMALS_ENABLED };

class NiGeomMorpherController : public NiCloneableStreamable<NiGeomMorpherController, NiInterpController> {
public:
	GeomMorpherFlags morpherFlags = GM_UPDATE_NORMALS_DISABLED;
	NiBlockRef<NiMorphData> dataRef;
	bool alwaysUpdate = false;
	NiBlockRefArray<NiInterpolator> interpolatorRefs;
	NiSyncVector<MorphWeight> interpWeights;

	NiVector<uint32_t> unknownInts;

	static constexpr const char* BlockName = "NiGeomMorpherController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class NiSingleInterpController : public NiCloneableStreamable<NiSingleInterpController, NiInterpController> {
public:
	NiBlockRef<NiInterpController> interpolatorRef;

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class NiRollController : public NiCloneableStreamable<NiRollController, NiSingleInterpController> {
public:
	NiBlockRef<NiFloatData> dataRef;

	static constexpr const char* BlockName = "NiRollController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

enum TargetColor : uint16_t { TC_AMBIENT, TC_DIFFUSE, TC_SPECULAR, TC_SELF_ILLUM };

class NiPoint3InterpController
	: public NiCloneableStreamable<NiPoint3InterpController, NiSingleInterpController> {
public:
	TargetColor targetColor = TC_AMBIENT;

	void Sync(NiStreamReversible& stream);
};

class NiMaterialColorController : public NiCloneable<NiMaterialColorController, NiPoint3InterpController> {
public:
	static constexpr const char* BlockName = "NiMaterialColorController";
	const char* GetBlockName() override { return BlockName; }
};

class NiLightColorController : public NiCloneable<NiLightColorController, NiPoint3InterpController> {
public:
	static constexpr const char* BlockName = "NiLightColorController";
	const char* GetBlockName() override { return BlockName; }
};

class NiExtraDataController : public NiCloneable<NiExtraDataController, NiSingleInterpController> {};

class NiFloatExtraDataController
	: public NiCloneableStreamable<NiFloatExtraDataController, NiExtraDataController> {
public:
	NiStringRef extraData;

	static constexpr const char* BlockName = "NiFloatExtraDataController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
};

class NiVisData : public NiCloneableStreamable<NiVisData, NiObject> {
public:
	NiSyncVector<NiAnimationKey<uint8_t>> keys;

	static constexpr const char* BlockName = "NiVisData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiBoolInterpController : public NiSingleInterpController {};

class NiVisController : public NiCloneable<NiVisController, NiBoolInterpController> {
public:
	static constexpr const char* BlockName = "NiVisController";
	const char* GetBlockName() override { return BlockName; }
};

enum TexType : uint32_t {
	BASE_MAP,
	DARK_MAP,
	DETAIL_MAP,
	GLOSS_MAP,
	GLOW_MAP,
	BUMP_MAP,
	NORMAL_MAP,
	PARALLAX_MAP,
	DECAL_0_MAP,
	DECAL_1_MAP,
	DECAL_2_MAP,
	DECAL_3_MAP
};

class NiFloatInterpController : public NiCloneable<NiFloatInterpController, NiSingleInterpController> {};

class BSRefractionFirePeriodController
	: public NiCloneable<BSRefractionFirePeriodController, NiSingleInterpController> {
public:
	static constexpr const char* BlockName = "BSRefractionFirePeriodController";
	const char* GetBlockName() override { return BlockName; }
};

class NiSourceTexture;

class NiFlipController : public NiCloneableStreamable<NiFlipController, NiFloatInterpController> {
public:
	TexType textureSlot = BASE_MAP;
	NiBlockRefArray<NiSourceTexture> sourceRefs;

	static constexpr const char* BlockName = "NiFlipController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

enum TexTransformType : uint32_t { TT_TRANSLATE_U, TT_TRANSLATE_V, TT_ROTATE, TT_SCALE_U, TT_SCALE_V };

class NiTextureTransformController
	: public NiCloneableStreamable<NiTextureTransformController, NiFloatInterpController> {
public:
	bool shaderMap = false;
	TexType textureSlot = BASE_MAP;
	TexTransformType operation = TT_TRANSLATE_U;

	static constexpr const char* BlockName = "NiTextureTransformController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiLightDimmerController : public NiCloneable<NiLightDimmerController, NiFloatInterpController> {
public:
	static constexpr const char* BlockName = "NiLightDimmerController";
	const char* GetBlockName() override { return BlockName; }
};

class NiLightRadiusController : public NiCloneable<NiLightRadiusController, NiFloatInterpController> {
public:
	static constexpr const char* BlockName = "NiLightRadiusController";
	const char* GetBlockName() override { return BlockName; }
};

class NiAlphaController : public NiCloneable<NiAlphaController, NiFloatInterpController> {
public:
	static constexpr const char* BlockName = "NiAlphaController";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysUpdateCtlr : public NiCloneable<NiPSysUpdateCtlr, NiTimeController> {
public:
	static constexpr const char* BlockName = "NiPSysUpdateCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class BSNiAlphaPropertyTestRefController
	: public NiCloneable<BSNiAlphaPropertyTestRefController, NiAlphaController> {
public:
	static constexpr const char* BlockName = "BSNiAlphaPropertyTestRefController";
	const char* GetBlockName() override { return BlockName; }
};

class NiKeyframeController : public NiCloneableStreamable<NiKeyframeController, NiSingleInterpController> {
public:
	NiBlockRef<NiKeyframeData> dataRef;

	static constexpr const char* BlockName = "NiKeyframeController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class NiTransformController : public NiCloneable<NiTransformController, NiKeyframeController> {
public:
	static constexpr const char* BlockName = "NiTransformController";
	const char* GetBlockName() override { return BlockName; }
};

class BSMaterialEmittanceMultController
	: public NiCloneable<BSMaterialEmittanceMultController, NiFloatInterpController> {
public:
	static constexpr const char* BlockName = "BSMaterialEmittanceMultController";
	const char* GetBlockName() override { return BlockName; }
};

class BSRefractionStrengthController
	: public NiCloneable<BSRefractionStrengthController, NiFloatInterpController> {
public:
	static constexpr const char* BlockName = "BSRefractionStrengthController";
	const char* GetBlockName() override { return BlockName; }
};

class BSLightingShaderPropertyColorController
	: public NiCloneableStreamable<BSLightingShaderPropertyColorController, NiFloatInterpController> {
public:
	uint32_t typeOfControlledColor = 0;

	static constexpr const char* BlockName = "BSLightingShaderPropertyColorController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSLightingShaderPropertyFloatController
	: public NiCloneableStreamable<BSLightingShaderPropertyFloatController, NiFloatInterpController> {
public:
	uint32_t typeOfControlledVariable = 0;

	static constexpr const char* BlockName = "BSLightingShaderPropertyFloatController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSLightingShaderPropertyUShortController
	: public NiCloneableStreamable<BSLightingShaderPropertyUShortController, NiFloatInterpController> {
public:
	uint32_t typeOfControlledVariable = 0;

	static constexpr const char* BlockName = "BSLightingShaderPropertyUShortController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSEffectShaderPropertyColorController
	: public NiCloneableStreamable<BSEffectShaderPropertyColorController, NiFloatInterpController> {
public:
	uint32_t typeOfControlledColor = 0;

	static constexpr const char* BlockName = "BSEffectShaderPropertyColorController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSEffectShaderPropertyFloatController
	: public NiCloneableStreamable<BSEffectShaderPropertyFloatController, NiFloatInterpController> {
public:
	uint32_t typeOfControlledVariable = 0;

	static constexpr const char* BlockName = "BSEffectShaderPropertyFloatController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiAVObject;

class NiMultiTargetTransformController
	: public NiCloneableStreamable<NiMultiTargetTransformController, NiInterpController> {
public:
	NiBlockPtrShortArray<NiAVObject> targetRefs;

	static constexpr const char* BlockName = "NiMultiTargetTransformController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<NiPtr*>& ptrs) override;
};

class NiPSysModifierCtlr : public NiCloneableStreamable<NiPSysModifierCtlr, NiSingleInterpController> {
public:
	NiStringRef modifierName;

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
};

class NiPSysModifierBoolCtlr : public NiCloneable<NiPSysModifierBoolCtlr, NiPSysModifierCtlr> {};

class NiPSysModifierActiveCtlr : public NiCloneable<NiPSysModifierActiveCtlr, NiPSysModifierBoolCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysModifierActiveCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysModifierFloatCtlr : public NiCloneable<NiPSysModifierFloatCtlr, NiPSysModifierCtlr> {};

class NiPSysEmitterLifeSpanCtlr : public NiCloneable<NiPSysEmitterLifeSpanCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysEmitterLifeSpanCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysEmitterSpeedCtlr : public NiCloneable<NiPSysEmitterSpeedCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysEmitterSpeedCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysEmitterInitialRadiusCtlr
	: public NiCloneable<NiPSysEmitterInitialRadiusCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysEmitterInitialRadiusCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysEmitterDeclinationCtlr
	: public NiCloneable<NiPSysEmitterDeclinationCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysEmitterDeclinationCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysGravityStrengthCtlr : public NiCloneable<NiPSysGravityStrengthCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysGravityStrengthCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysEmitterDeclinationVarCtlr
	: public NiCloneable<NiPSysEmitterDeclinationVarCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysEmitterDeclinationVarCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysFieldMagnitudeCtlr : public NiCloneable<NiPSysFieldMagnitudeCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysFieldMagnitudeCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysFieldAttenuationCtlr : public NiCloneable<NiPSysFieldAttenuationCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysFieldAttenuationCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysFieldMaxDistanceCtlr : public NiCloneable<NiPSysFieldMaxDistanceCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysFieldMaxDistanceCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysAirFieldAirFrictionCtlr
	: public NiCloneable<NiPSysAirFieldAirFrictionCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysAirFieldAirFrictionCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysAirFieldInheritVelocityCtlr
	: public NiCloneable<NiPSysAirFieldInheritVelocityCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysAirFieldInheritVelocityCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysAirFieldSpreadCtlr : public NiCloneable<NiPSysAirFieldSpreadCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysAirFieldSpreadCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysInitialRotSpeedCtlr : public NiCloneable<NiPSysInitialRotSpeedCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysInitialRotSpeedCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysInitialRotSpeedVarCtlr
	: public NiCloneable<NiPSysInitialRotSpeedVarCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysInitialRotSpeedVarCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysInitialRotAngleCtlr : public NiCloneable<NiPSysInitialRotAngleCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysInitialRotAngleCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysInitialRotAngleVarCtlr
	: public NiCloneable<NiPSysInitialRotAngleVarCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysInitialRotAngleVarCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysEmitterPlanarAngleCtlr
	: public NiCloneable<NiPSysEmitterPlanarAngleCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysEmitterPlanarAngleCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysEmitterPlanarAngleVarCtlr
	: public NiCloneable<NiPSysEmitterPlanarAngleVarCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysEmitterPlanarAngleVarCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysRotDampeningCtlr
	: public NiCloneable<NiPSysRotDampeningCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysRotDampeningCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiStringPalette : public NiCloneableStreamable<NiStringPalette, NiObject> {
public:
	NiString palette;
	uint32_t length = 0;

	static constexpr const char* BlockName = "NiStringPalette";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class ControllerLink {
public:
	NiString targetName;
	NiBlockRef<NiInterpolator> interpolatorRef;
	NiBlockRef<NiTimeController> controllerRef;

	NiBlockRef<NiBlendInterpolator> blendInterpolatorRef;
	uint16_t blendIndex = 0;

	uint8_t priority = 0;

	NiBlockRef<NiStringPalette> stringPaletteRef;
	uint32_t nodeNameOffset = 0;
	uint32_t propertyTypeOffset = 0;
	uint32_t controllerTypeOffset = 0;
	uint32_t controllerIDOffset = 0;
	uint32_t interpIDOffset = 0;

	NiStringRef nodeName;
	NiStringRef propType;
	NiStringRef ctrlType;
	NiStringRef ctrlID;
	NiStringRef interpID;

	void Sync(NiStreamReversible& stream) {
		if (stream.GetVersion().File() < V10_1_0_104)
			targetName.Sync(stream, 4);

		if (stream.GetVersion().File() >= V10_1_0_106)
			interpolatorRef.Sync(stream);

		if (stream.GetVersion().File() <= V20_5_0_0)
			controllerRef.Sync(stream);

		if (stream.GetVersion().File() >= V10_1_0_104 && stream.GetVersion().File() <= V10_1_0_110) {
			blendInterpolatorRef.Sync(stream);
			stream.Sync(blendIndex);
		}

		if (stream.GetVersion().File() >= V10_1_0_106 && stream.GetVersion().Stream() > 0)
			stream.Sync(priority);

		if ((stream.GetVersion().File() >= V10_1_0_104 && stream.GetVersion().File() < V10_1_0_114) ||
			(stream.GetVersion().File() >= V20_1_0_1)) {
			nodeName.Sync(stream);
			propType.Sync(stream);
			ctrlType.Sync(stream);
			ctrlID.Sync(stream);
			interpID.Sync(stream);
		}

		if (stream.GetVersion().File() >= V10_2_0_0 && stream.GetVersion().File() < V20_1_0_1) {
			stringPaletteRef.Sync(stream);
			stream.Sync(nodeNameOffset);
			stream.Sync(propertyTypeOffset);
			stream.Sync(controllerTypeOffset);
			stream.Sync(controllerIDOffset);
			stream.Sync(interpIDOffset);
		}
	}

	void GetStringRefs(std::vector<NiStringRef*>& refs) {
		refs.emplace_back(&nodeName);
		refs.emplace_back(&propType);
		refs.emplace_back(&ctrlType);
		refs.emplace_back(&ctrlID);
		refs.emplace_back(&interpID);
	}

	void GetChildRefs(std::set<NiRef*>& refs) {
		refs.insert(&interpolatorRef);
		refs.insert(&controllerRef);
		refs.insert(&blendInterpolatorRef);
		refs.insert(&stringPaletteRef);
	}

	void GetChildIndices(std::vector<uint32_t>& indices) {
		indices.push_back(interpolatorRef.index);
		indices.push_back(controllerRef.index);
		indices.push_back(blendInterpolatorRef.index);
		indices.push_back(stringPaletteRef.index);
	}
};

class NiSequence : public NiCloneableStreamable<NiSequence, NiObject> {
public:
	NiStringRef name;
	uint32_t arrayGrowBy = 0;

	NiSyncVector<ControllerLink> controlledBlocks;

	static constexpr const char* BlockName = "NiSequence";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

enum CycleType : uint32_t { CYCLE_LOOP, CYCLE_REVERSE, CYCLE_CLAMP };

class BSAnimNote : public NiCloneableStreamable<BSAnimNote, NiObject> {
public:
	enum AnimNoteType : uint32_t { ANT_INVALID, ANT_GRABIK, ANT_LOOKIK };

	AnimNoteType type = ANT_INVALID;
	float time = 0.0f;
	uint32_t arm = 0;
	float gain = 0.0f;
	uint32_t state = 0;

	static constexpr const char* BlockName = "BSAnimNote";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSAnimNotes : public NiCloneableStreamable<BSAnimNotes, NiObject> {
public:
	NiBlockRefShortArray<BSAnimNote> animNoteRefs;

	static constexpr const char* BlockName = "BSAnimNotes";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class NiControllerManager;

class NiControllerSequence : public NiCloneableStreamable<NiControllerSequence, NiSequence> {
public:
	float weight = 1.0f;
	NiBlockRef<NiTextKeyExtraData> textKeyRef;
	CycleType cycleType = CYCLE_LOOP;
	float frequency = 0.0f;
	float phase = 0.0f;
	float startTime = 0.0f;
	float stopTime = 0.0f;
	bool playBackwards = false;
	NiBlockPtr<NiControllerManager> managerRef;
	NiStringRef accumRootName;

	NiBlockRef<NiStringPalette> stringPaletteRef;

	NiBlockRef<BSAnimNotes> animNotesRef;
	NiBlockRefShortArray<BSAnimNotes> animNotesRefs;

	static constexpr const char* BlockName = "NiControllerSequence";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
	void GetPtrs(std::set<NiPtr*>& ptrs) override;
};

class NiDefaultAVObjectPalette;

class NiControllerManager : public NiCloneableStreamable<NiControllerManager, NiTimeController> {
public:
	bool cumulative = false;
	NiBlockRefArray<NiControllerSequence> controllerSequenceRefs;
	NiBlockRef<NiDefaultAVObjectPalette> objectPaletteRef;

	static constexpr const char* BlockName = "NiControllerManager";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};
} // namespace nifly
