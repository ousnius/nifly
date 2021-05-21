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
STREAMABLECLASSDEF(NiKeyframeData, NiObject) {
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

CLONEABLECLASSDEF(NiTransformData, NiKeyframeData) {
public:
	static constexpr const char* BlockName = "NiTransformData";

	const char* GetBlockName() override { return BlockName; }
};

STREAMABLECLASSDEF(NiPosData, NiObject) {
public:
	NiAnimationKeyGroup<Vector3> data;

	static constexpr const char* BlockName = "NiPosData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

STREAMABLECLASSDEF(NiBoolData, NiObject) {
public:
	NiAnimationKeyGroup<uint8_t> data;

	static constexpr const char* BlockName = "NiBoolData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

STREAMABLECLASSDEF(NiFloatData, NiObject) {
public:
	NiAnimationKeyGroup<float> data;

	static constexpr const char* BlockName = "NiFloatData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

STREAMABLECLASSDEF(NiBSplineData, NiObject) {
public:
	NiVector<float> floatControlPoints;
	NiVector<short> shortControlPoints;

	static constexpr const char* BlockName = "NiBSplineData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

STREAMABLECLASSDEF(NiBSplineBasisData, NiObject) {
public:
	uint32_t numControlPoints = 0;

	static constexpr const char* BlockName = "NiBSplineBasisData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

CLONEABLECLASSDEF(NiInterpolator, NiObject) {};

STREAMABLECLASSDEF(NiBSplineInterpolator, NiInterpolator) {
public:
	float startTime = 0.0f;
	float stopTime = 0.0f;
	NiBlockRef<NiBSplineData> splineDataRef;
	NiBlockRef<NiBSplineBasisData> basisDataRef;

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

CLONEABLECLASSDEF(NiBSplineFloatInterpolator, NiBSplineInterpolator) {};

STREAMABLECLASSDEF(NiBSplineCompFloatInterpolator, NiBSplineFloatInterpolator) {
public:
	float radix = 0.0f;
	uint32_t offset = 0;
	float bias = 0.0f;
	float multiplier = 0.0f;

	static constexpr const char* BlockName = "NiBSplineCompFloatInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

STREAMABLECLASSDEF(NiBSplinePoint3Interpolator, NiBSplineInterpolator) {
public:
	Vector3 value = NiVec3Min;
	uint32_t handle = NiUShortMax;

	void Sync(NiStreamReversible& stream);
};

STREAMABLECLASSDEF(NiBSplineCompPoint3Interpolator, NiBSplinePoint3Interpolator) {
public:
	float positionOffset = NiFloatMax;
	float positionHalfRange = NiFloatMax;

	static constexpr const char* BlockName = "NiBSplineCompPoint3Interpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

STREAMABLECLASSDEF(NiBSplineTransformInterpolator, NiBSplineInterpolator) {
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

STREAMABLECLASSDEF(NiBSplineCompTransformInterpolator, NiBSplineTransformInterpolator) {
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
	uint8_t priority = 0;
	float easeSpinner = 0.0f;

	void Sync(NiStreamReversible& stream);
};

STREAMABLECLASSDEF(NiBlendInterpolator, NiInterpolator) {
public:
	InterpBlendFlags flags = INTERP_BLEND_MANAGER_CONTROLLED;
	uint8_t arraySize = 0;
	float weightThreshold = 0.0f;

	uint8_t interpCount = 0;
	uint8_t singleIndex = NiByteMax;
	char highPriority = NiCharMin;
	char nextHighPriority = NiCharMin;
	float singleTime = NiFloatMin;
	float highWeightsSum = NiFloatMin;
	float nextHighWeightsSum = NiFloatMin;
	float highEaseSpinner = NiFloatMin;
	std::vector<InterpBlendItem> interpItems;

	void Sync(NiStreamReversible& stream);
};

STREAMABLECLASSDEF(NiBlendBoolInterpolator, NiBlendInterpolator) {
public:
	bool value = false;

	static constexpr const char* BlockName = "NiBlendBoolInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

STREAMABLECLASSDEF(NiBlendFloatInterpolator, NiBlendInterpolator) {
public:
	float value = 0.0f;

	static constexpr const char* BlockName = "NiBlendFloatInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

STREAMABLECLASSDEF(NiBlendPoint3Interpolator, NiBlendInterpolator) {
public:
	Vector3 point;

	static constexpr const char* BlockName = "NiBlendPoint3Interpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

STREAMABLECLASSDEF(NiBlendTransformInterpolator, NiBlendInterpolator) {
public:
	static constexpr const char* BlockName = "NiBlendTransformInterpolator";
	const char* GetBlockName() override { return BlockName; }
};

class NiKeyBasedInterpolator : public NiInterpolator {};

STREAMABLECLASSDEF(NiBoolInterpolator, NiKeyBasedInterpolator) {
public:
	uint8_t boolValue = 0;
	NiBlockRef<NiBoolData> dataRef;

	static constexpr const char* BlockName = "NiBoolInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

CLONEABLECLASSDEF(NiBoolTimelineInterpolator, NiBoolInterpolator) {
public:
	static constexpr const char* BlockName = "NiBoolTimelineInterpolator";
	const char* GetBlockName() override { return BlockName; }
};

STREAMABLECLASSDEF(NiFloatInterpolator, NiKeyBasedInterpolator) {
public:
	float floatValue = 0.0f;
	NiBlockRef<NiFloatData> dataRef;

	static constexpr const char* BlockName = "NiFloatInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

STREAMABLECLASSDEF(NiTransformInterpolator, NiKeyBasedInterpolator) {
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

CLONEABLECLASSDEF(BSRotAccumTransfInterpolator, NiTransformInterpolator) {
public:
	static constexpr const char* BlockName = "BSRotAccumTransfInterpolator";
	const char* GetBlockName() override { return BlockName; }
};

STREAMABLECLASSDEF(NiPoint3Interpolator, NiKeyBasedInterpolator) {
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

STREAMABLECLASSDEF(NiPathInterpolator, NiKeyBasedInterpolator) {
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

STREAMABLECLASSDEF(NiLookAtInterpolator, NiInterpolator) {
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

STREAMABLECLASSDEF(BSTreadTransfInterpolator, NiInterpolator) {
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

STREAMABLECLASSDEF(NiTimeController, NiObject) {
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

STREAMABLECLASSDEF(NiLookAtController, NiTimeController) {
public:
	LookAtFlags lookAtFlags = LOOK_X_AXIS;
	NiBlockPtr<NiNode> lookAtNodePtr;

	static constexpr const char* BlockName = "NiLookAtController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<NiPtr*>& ptrs) override;
};

STREAMABLECLASSDEF(NiPathController, NiTimeController) {
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

CLONEABLECLASSDEF(NiPSysResetOnLoopCtlr, NiTimeController) {
public:
	static constexpr const char* BlockName = "NiPSysResetOnLoopCtlr";
	const char* GetBlockName() override { return BlockName; }
};

STREAMABLECLASSDEF(NiUVData, NiObject) {
public:
	NiAnimationKeyGroup<float> uTrans;
	NiAnimationKeyGroup<float> vTrans;
	NiAnimationKeyGroup<float> uScale;
	NiAnimationKeyGroup<float> vScale;

	static constexpr const char* BlockName = "NiUVData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

STREAMABLECLASSDEF(NiUVController, NiTimeController) {
public:
	uint16_t textureSet = 0;
	NiBlockRef<NiUVData> dataRef;

	static constexpr const char* BlockName = "NiUVController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

STREAMABLECLASSDEF(BSFrustumFOVController, NiTimeController) {
public:
	NiBlockRef<NiInterpolator> interpolatorRef;

	static constexpr const char* BlockName = "BSFrustumFOVController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

STREAMABLECLASSDEF(BSLagBoneController, NiTimeController) {
public:
	float linearVelocity = 0.0f;
	float linearRotation = 0.0f;
	float maxDistance = 0.0f;

	static constexpr const char* BlockName = "BSLagBoneController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSShaderProperty;

STREAMABLECLASSDEF(BSProceduralLightningController, NiTimeController) {
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

STREAMABLECLASSDEF(NiBoneLODController, NiTimeController) {
public:
	uint32_t lod = 0;
	uint32_t numLODs = 0;
	NiSyncVector<NiBlockPtrArray<NiNode>> boneArrays;

	static constexpr const char* BlockName = "NiBoneLODController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<NiPtr*>& ptrs) override;
};

CLONEABLECLASSDEF(NiBSBoneLODController, NiBoneLODController) {
public:
	static constexpr const char* BlockName = "NiBSBoneLODController";
	const char* GetBlockName() override { return BlockName; }
};

struct Morph {
	NiStringRef frameName;
	std::vector<Vector3> vectors;

	void Sync(NiStreamReversible& stream, uint32_t numVerts) {
		frameName.Sync(stream);
		vectors.resize(numVerts);
		for (uint32_t i = 0; i < numVerts; i++)
			stream.Sync(vectors[i]);
	}

	void GetStringRefs(std::vector<NiStringRef*>& refs) { refs.emplace_back(&frameName); }
};

STREAMABLECLASSDEF(NiMorphData, NiObject) {
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

CLONEABLECLASSDEF(NiInterpController, NiTimeController) {};

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

STREAMABLECLASSDEF(NiGeomMorpherController, NiInterpController) {
public:
	GeomMorpherFlags morpherFlags = GM_UPDATE_NORMALS_DISABLED;
	NiBlockRef<NiMorphData> dataRef;
	bool alwaysUpdate = false;
	NiSyncVector<MorphWeight> interpWeights;

	static constexpr const char* BlockName = "NiGeomMorpherController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

STREAMABLECLASSDEF(NiSingleInterpController, NiInterpController) {
public:
	NiBlockRef<NiInterpController> interpolatorRef;

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

STREAMABLECLASSDEF(NiRollController, NiSingleInterpController) {
public:
	NiBlockRef<NiFloatData> dataRef;

	static constexpr const char* BlockName = "NiRollController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

enum TargetColor : uint16_t { TC_AMBIENT, TC_DIFFUSE, TC_SPECULAR, TC_SELF_ILLUM };

STREAMABLECLASSDEF(NiPoint3InterpController, NiSingleInterpController) {
public:
	TargetColor targetColor = TC_AMBIENT;

	void Sync(NiStreamReversible& stream);
};

CLONEABLECLASSDEF(NiMaterialColorController, NiPoint3InterpController) {
public:
	static constexpr const char* BlockName = "NiMaterialColorController";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiLightColorController, NiPoint3InterpController) {
public:
	static constexpr const char* BlockName = "NiLightColorController";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiExtraDataController, NiSingleInterpController) {};

STREAMABLECLASSDEF(NiFloatExtraDataController, NiExtraDataController) {
public:
	NiStringRef extraData;

	static constexpr const char* BlockName = "NiFloatExtraDataController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
};

STREAMABLECLASSDEF(NiVisData, NiObject) {
public:
	NiSyncVector<NiAnimationKey<uint8_t>> keys;

	static constexpr const char* BlockName = "NiVisData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiBoolInterpController : public NiSingleInterpController {};

CLONEABLECLASSDEF(NiVisController, NiBoolInterpController) {
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

CLONEABLECLASSDEF(NiFloatInterpController, NiSingleInterpController) {};

CLONEABLECLASSDEF(BSRefractionFirePeriodController, NiSingleInterpController) {
public:
	static constexpr const char* BlockName = "BSRefractionFirePeriodController";
	const char* GetBlockName() override { return BlockName; }
};

class NiSourceTexture;

STREAMABLECLASSDEF(NiFlipController, NiFloatInterpController) {
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

STREAMABLECLASSDEF(NiTextureTransformController, NiFloatInterpController) {
public:
	bool shaderMap = false;
	TexType textureSlot = BASE_MAP;
	TexTransformType operation = TT_TRANSLATE_U;

	static constexpr const char* BlockName = "NiTextureTransformController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

CLONEABLECLASSDEF(NiLightDimmerController, NiFloatInterpController) {
public:
	static constexpr const char* BlockName = "NiLightDimmerController";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiLightRadiusController, NiFloatInterpController) {
public:
	static constexpr const char* BlockName = "NiLightRadiusController";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiAlphaController, NiFloatInterpController) {
public:
	static constexpr const char* BlockName = "NiAlphaController";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiPSysUpdateCtlr, NiTimeController) {
public:
	static constexpr const char* BlockName = "NiPSysUpdateCtlr";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(BSNiAlphaPropertyTestRefController, NiAlphaController) {
public:
	static constexpr const char* BlockName = "BSNiAlphaPropertyTestRefController";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiKeyframeController, NiSingleInterpController) {
public:
	static constexpr const char* BlockName = "NiKeyframeController";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiTransformController, NiKeyframeController) {
public:
	static constexpr const char* BlockName = "NiTransformController";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(BSMaterialEmittanceMultController, NiFloatInterpController) {
public:
	static constexpr const char* BlockName = "BSMaterialEmittanceMultController";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(BSRefractionStrengthController, NiFloatInterpController) {
public:
	static constexpr const char* BlockName = "BSRefractionStrengthController";
	const char* GetBlockName() override { return BlockName; }
};

STREAMABLECLASSDEF(BSLightingShaderPropertyColorController, NiFloatInterpController) {
public:
	uint32_t typeOfControlledColor = 0;

	static constexpr const char* BlockName = "BSLightingShaderPropertyColorController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

STREAMABLECLASSDEF(BSLightingShaderPropertyFloatController, NiFloatInterpController) {
public:
	uint32_t typeOfControlledVariable = 0;

	static constexpr const char* BlockName = "BSLightingShaderPropertyFloatController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

STREAMABLECLASSDEF(BSLightingShaderPropertyUShortController, NiFloatInterpController) {
public:
	uint32_t typeOfControlledVariable = 0;

	static constexpr const char* BlockName = "BSLightingShaderPropertyUShortController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

STREAMABLECLASSDEF(BSEffectShaderPropertyColorController, NiFloatInterpController) {
public:
	uint32_t typeOfControlledColor = 0;

	static constexpr const char* BlockName = "BSEffectShaderPropertyColorController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

STREAMABLECLASSDEF(BSEffectShaderPropertyFloatController, NiFloatInterpController) {
public:
	uint32_t typeOfControlledVariable = 0;

	static constexpr const char* BlockName = "BSEffectShaderPropertyFloatController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiAVObject;

STREAMABLECLASSDEF(NiMultiTargetTransformController, NiInterpController) {
public:
	NiBlockPtrShortArray<NiAVObject> targetRefs;

	static constexpr const char* BlockName = "NiMultiTargetTransformController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<NiPtr*>& ptrs) override;
};

STREAMABLECLASSDEF(NiPSysModifierCtlr, NiSingleInterpController) {
public:
	NiStringRef modifierName;

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
};

CLONEABLECLASSDEF(NiPSysModifierBoolCtlr, NiPSysModifierCtlr) {};

CLONEABLECLASSDEF(NiPSysModifierActiveCtlr, NiPSysModifierBoolCtlr) {
public:
	static constexpr const char* BlockName = "NiPSysModifierActiveCtlr";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiPSysModifierFloatCtlr, NiPSysModifierCtlr) {};

CLONEABLECLASSDEF(NiPSysEmitterLifeSpanCtlr, NiPSysModifierFloatCtlr) {
public:
	static constexpr const char* BlockName = "NiPSysEmitterLifeSpanCtlr";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiPSysEmitterSpeedCtlr, NiPSysModifierFloatCtlr) {
public:
	static constexpr const char* BlockName = "NiPSysEmitterSpeedCtlr";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiPSysEmitterInitialRadiusCtlr, NiPSysModifierFloatCtlr) {
public:
	static constexpr const char* BlockName = "NiPSysEmitterInitialRadiusCtlr";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiPSysEmitterDeclinationCtlr, NiPSysModifierFloatCtlr) {
public:
	static constexpr const char* BlockName = "NiPSysEmitterDeclinationCtlr";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiPSysGravityStrengthCtlr, NiPSysModifierFloatCtlr) {
public:
	static constexpr const char* BlockName = "NiPSysGravityStrengthCtlr";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiPSysEmitterDeclinationVarCtlr, NiPSysModifierFloatCtlr) {
public:
	static constexpr const char* BlockName = "NiPSysEmitterDeclinationVarCtlr";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiPSysFieldMagnitudeCtlr, NiPSysModifierFloatCtlr) {
public:
	static constexpr const char* BlockName = "NiPSysFieldMagnitudeCtlr";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiPSysFieldAttenuationCtlr, NiPSysModifierFloatCtlr) {
public:
	static constexpr const char* BlockName = "NiPSysFieldAttenuationCtlr";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiPSysFieldMaxDistanceCtlr, NiPSysModifierFloatCtlr) {
public:
	static constexpr const char* BlockName = "NiPSysFieldMaxDistanceCtlr";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiPSysAirFieldAirFrictionCtlr, NiPSysModifierFloatCtlr) {
public:
	static constexpr const char* BlockName = "NiPSysAirFieldAirFrictionCtlr";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiPSysAirFieldInheritVelocityCtlr, NiPSysModifierFloatCtlr) {
public:
	static constexpr const char* BlockName = "NiPSysAirFieldInheritVelocityCtlr";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiPSysAirFieldSpreadCtlr, NiPSysModifierFloatCtlr) {
public:
	static constexpr const char* BlockName = "NiPSysAirFieldSpreadCtlr";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiPSysInitialRotSpeedCtlr, NiPSysModifierFloatCtlr) {
public:
	static constexpr const char* BlockName = "NiPSysInitialRotSpeedCtlr";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiPSysInitialRotSpeedVarCtlr, NiPSysModifierFloatCtlr) {
public:
	static constexpr const char* BlockName = "NiPSysInitialRotSpeedVarCtlr";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiPSysInitialRotAngleCtlr, NiPSysModifierFloatCtlr) {
public:
	static constexpr const char* BlockName = "NiPSysInitialRotAngleCtlr";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiPSysInitialRotAngleVarCtlr, NiPSysModifierFloatCtlr) {
public:
	static constexpr const char* BlockName = "NiPSysInitialRotAngleVarCtlr";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiPSysEmitterPlanarAngleCtlr, NiPSysModifierFloatCtlr) {
public:
	static constexpr const char* BlockName = "NiPSysEmitterPlanarAngleCtlr";
	const char* GetBlockName() override { return BlockName; }
};

CLONEABLECLASSDEF(NiPSysEmitterPlanarAngleVarCtlr, NiPSysModifierFloatCtlr) {
public:
	static constexpr const char* BlockName = "NiPSysEmitterPlanarAngleVarCtlr";
	const char* GetBlockName() override { return BlockName; }
};

STREAMABLECLASSDEF(NiPSysEmitterCtlr, NiPSysModifierCtlr) {
public:
	NiBlockRef<NiInterpolator> visInterpolatorRef;

	static constexpr const char* BlockName = "NiPSysEmitterCtlr";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class BSMasterParticleSystem;

STREAMABLECLASSDEF(BSPSysMultiTargetEmitterCtlr, NiPSysEmitterCtlr) {
public:
	uint16_t maxEmitters = 0;
	NiBlockPtr<BSMasterParticleSystem> masterParticleSystemRef;

	static constexpr const char* BlockName = "BSPSysMultiTargetEmitterCtlr";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<NiPtr*>& ptrs) override;
};

STREAMABLECLASSDEF(NiStringPalette, NiObject) {
public:
	NiString palette;
	uint32_t length = 0;

	static constexpr const char* BlockName = "NiStringPalette";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class ControllerLink {
public:
	NiBlockRef<NiInterpolator> interpolatorRef;
	NiBlockRef<NiTimeController> controllerRef;
	uint8_t priority = 0;

	NiStringRef nodeName;
	NiStringRef propType;
	NiStringRef ctrlType;
	NiStringRef ctrlID;
	NiStringRef interpID;

	void Sync(NiStreamReversible& stream) {
		interpolatorRef.Sync(stream);
		controllerRef.Sync(stream);
		stream.Sync(priority);

		nodeName.Sync(stream);
		propType.Sync(stream);
		ctrlType.Sync(stream);
		ctrlID.Sync(stream);
		interpID.Sync(stream);
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
	}

	void GetChildIndices(std::vector<uint32_t>& indices) {
		indices.push_back(interpolatorRef.index);
		indices.push_back(controllerRef.index);
	}
};

STREAMABLECLASSDEF(NiSequence, NiObject) {
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

STREAMABLECLASSDEF(BSAnimNote, NiObject) {
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

STREAMABLECLASSDEF(BSAnimNotes, NiObject) {
public:
	NiBlockRefShortArray<BSAnimNote> animNoteRefs;

	static constexpr const char* BlockName = "BSAnimNotes";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class NiControllerManager;

STREAMABLECLASSDEF(NiControllerSequence, NiSequence) {
public:
	float weight = 1.0f;
	NiBlockRef<NiTextKeyExtraData> textKeyRef;
	CycleType cycleType = CYCLE_LOOP;
	float frequency = 0.0f;
	float startTime = 0.0f;
	float stopTime = 0.0f;
	NiBlockPtr<NiControllerManager> managerRef;
	NiStringRef accumRootName;

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

STREAMABLECLASSDEF(NiControllerManager, NiTimeController) {
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
