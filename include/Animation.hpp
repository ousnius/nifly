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
class NiKeyframeData : public NiObjectCRTP<NiKeyframeData, NiObject, true> {
private:
	uint32_t numRotationKeys = 0;
	std::vector<Key<Quaternion>> quaternionKeys;

public:
	KeyType rotationType = NO_INTERP;
	KeyGroup<float> xRotations;
	KeyGroup<float> yRotations;
	KeyGroup<float> zRotations;
	KeyGroup<Vector3> translations;
	KeyGroup<float> scales;

	static constexpr const char* BlockName = "NiKeyframeData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	std::vector<Key<Quaternion>> GetQuaternionKeys() const;
	void SetQuaternionKeys(const std::vector<Key<Quaternion>>& qk);
};

class NiTransformData : public NiObjectCRTP<NiTransformData, NiKeyframeData> {
public:
	static constexpr const char* BlockName = "NiTransformData";

	const char* GetBlockName() override { return BlockName; }
};

class NiPosData : public NiObjectCRTP<NiPosData, NiObject, true> {
public:
	KeyGroup<Vector3> data;

	static constexpr const char* BlockName = "NiPosData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiBoolData : public NiObjectCRTP<NiBoolData, NiObject, true> {
public:
	KeyGroup<uint8_t> data;

	static constexpr const char* BlockName = "NiBoolData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiFloatData : public NiObjectCRTP<NiFloatData, NiObject, true> {
public:
	KeyGroup<float> data;

	static constexpr const char* BlockName = "NiFloatData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiBSplineData : public NiObjectCRTP<NiBSplineData, NiObject, true> {
public:
	NiVector<float> floatControlPoints;
	NiVector<short> shortControlPoints;

	static constexpr const char* BlockName = "NiBSplineData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiBSplineBasisData : public NiObjectCRTP<NiBSplineBasisData, NiObject, true> {
public:
	uint32_t numControlPoints = 0;

	static constexpr const char* BlockName = "NiBSplineBasisData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiInterpolator : public NiObjectCRTP<NiInterpolator, NiObject> {};

class NiBSplineInterpolator : public NiObjectCRTP<NiBSplineInterpolator, NiInterpolator, true> {
public:
	float startTime = 0.0f;
	float stopTime = 0.0f;
	NiBlockRef<NiBSplineData> splineDataRef;
	NiBlockRef<NiBSplineBasisData> basisDataRef;

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class NiBSplineFloatInterpolator : public NiObjectCRTP<NiBSplineFloatInterpolator, NiBSplineInterpolator> {};

class NiBSplineCompFloatInterpolator
	: public NiObjectCRTP<NiBSplineCompFloatInterpolator, NiBSplineFloatInterpolator, true> {
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
	: public NiObjectCRTP<NiBSplinePoint3Interpolator, NiBSplineInterpolator, true> {
public:
	float unkFloat1 = 0.0f;
	float unkFloat2 = 0.0f;
	float unkFloat3 = 0.0f;
	float unkFloat4 = 0.0f;
	float unkFloat5 = 0.0f;
	float unkFloat6 = 0.0f;

	void Sync(NiStreamReversible& stream);
};

class NiBSplineCompPoint3Interpolator
	: public NiObjectCRTP<NiBSplineCompPoint3Interpolator, NiBSplinePoint3Interpolator> {
public:
	static constexpr const char* BlockName = "NiBSplineCompPoint3Interpolator";

	const char* GetBlockName() override { return BlockName; }
};

class NiBSplineTransformInterpolator
	: public NiObjectCRTP<NiBSplineTransformInterpolator, NiBSplineInterpolator, true> {
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
	: public NiObjectCRTP<NiBSplineCompTransformInterpolator, NiBSplineTransformInterpolator, true> {
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

class NiBlendInterpolator : public NiObjectCRTP<NiBlendInterpolator, NiInterpolator, true> {
public:
	uint16_t flags = 0;
	uint32_t unkInt = 0;

	void Sync(NiStreamReversible& stream);
};

class NiBlendBoolInterpolator : public NiObjectCRTP<NiBlendBoolInterpolator, NiBlendInterpolator, true> {
public:
	bool value = false;

	static constexpr const char* BlockName = "NiBlendBoolInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiBlendFloatInterpolator : public NiObjectCRTP<NiBlendFloatInterpolator, NiBlendInterpolator, true> {
public:
	float value = 0.0f;

	static constexpr const char* BlockName = "NiBlendFloatInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiBlendPoint3Interpolator : public NiObjectCRTP<NiBlendPoint3Interpolator, NiBlendInterpolator, true> {
public:
	Vector3 point;

	static constexpr const char* BlockName = "NiBlendPoint3Interpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiBlendTransformInterpolator
	: public NiObjectCRTP<NiBlendTransformInterpolator, NiBlendInterpolator, true> {
public:
	static constexpr const char* BlockName = "NiBlendTransformInterpolator";
	const char* GetBlockName() override { return BlockName; }
};

class NiKeyBasedInterpolator : public NiObjectCRTP<NiKeyBasedInterpolator, NiInterpolator> {};

class NiBoolInterpolator : public NiObjectCRTP<NiBoolInterpolator, NiKeyBasedInterpolator, true> {
public:
	uint8_t boolValue = 0;
	NiBlockRef<NiBoolData> dataRef;

	static constexpr const char* BlockName = "NiBoolInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class NiBoolTimelineInterpolator : public NiObjectCRTP<NiBoolTimelineInterpolator, NiBoolInterpolator> {
public:
	static constexpr const char* BlockName = "NiBoolTimelineInterpolator";
	const char* GetBlockName() override { return BlockName; }
};

class NiFloatInterpolator : public NiObjectCRTP<NiFloatInterpolator, NiKeyBasedInterpolator, true> {
public:
	float floatValue = 0.0f;
	NiBlockRef<NiFloatData> dataRef;

	static constexpr const char* BlockName = "NiFloatInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class NiTransformInterpolator : public NiObjectCRTP<NiTransformInterpolator, NiKeyBasedInterpolator, true> {
public:
	Vector3 translation;
	Quaternion rotation;
	float scale = 0.0f;
	NiBlockRef<NiTransformData> dataRef;

	static constexpr const char* BlockName = "NiTransformInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class BSRotAccumTransfInterpolator
	: public NiObjectCRTP<BSRotAccumTransfInterpolator, NiTransformInterpolator> {
public:
	static constexpr const char* BlockName = "BSRotAccumTransfInterpolator";
	const char* GetBlockName() override { return BlockName; }
};

class NiPoint3Interpolator : public NiObjectCRTP<NiPoint3Interpolator, NiKeyBasedInterpolator, true> {
public:
	Vector3 point3Value;
	NiBlockRef<NiPosData> dataRef;

	static constexpr const char* BlockName = "NiPoint3Interpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class NiPathInterpolator : public NiObjectCRTP<NiPathInterpolator, NiKeyBasedInterpolator, true> {
private:
	uint16_t flags = 0;
	uint32_t bankDir = 0;
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
	void GetChildIndices(std::vector<int>& indices) override;
};

enum LookAtFlags : uint16_t {
	LOOK_X_AXIS = 0x0000,
	LOOK_FLIP = 0x0001,
	LOOK_Y_AXIS = 0x0002,
	LOOK_Z_AXIS = 0x0004
};

class NiNode;

class NiLookAtInterpolator : public NiObjectCRTP<NiLookAtInterpolator, NiInterpolator, true> {
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
	void GetChildIndices(std::vector<int>& indices) override;
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

class BSTreadTransfInterpolator : public NiObjectCRTP<BSTreadTransfInterpolator, NiInterpolator, true> {
private:
	uint32_t numTreadTransforms = 0;
	std::vector<BSTreadTransform> treadTransforms;

public:
	NiBlockRef<NiFloatData> dataRef;

	static constexpr const char* BlockName = "BSTreadTransfInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	std::vector<BSTreadTransform> GetTreadTransforms() const;
	void SetTreadTransforms(const std::vector<BSTreadTransform>& tt);
};

class NiObjectNET;

class NiTimeController : public NiObjectCRTP<NiTimeController, NiObject, true> {
public:
	NiBlockRef<NiTimeController> nextControllerRef;
	uint16_t flags = 0x000C;
	float frequency = 1.0f;
	float phase = 0.0f;
	float startTime = 0.0f;
	float stopTime = 0.0f;
	NiBlockPtr<NiObjectNET> targetRef;

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	void GetPtrs(std::set<NiPtr*>& ptrs) override;
};

class NiLookAtController : public NiObjectCRTP<NiLookAtController, NiTimeController, true> {
public:
	uint16_t unkShort1 = 0;
	NiBlockPtr<NiNode> lookAtNodePtr;

	static constexpr const char* BlockName = "NiLookAtController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<NiPtr*>& ptrs) override;
};

class NiPathController : public NiObjectCRTP<NiPathController, NiTimeController, true> {
public:
	uint16_t unkShort1 = 0;
	uint32_t unkInt1 = 1;
	float unkFloat1 = 0.0f;
	float unkFloat2 = 0.0f;
	uint16_t unkShort2 = 0;
	NiBlockRef<NiPosData> posDataRef;
	NiBlockRef<NiFloatData> floatDataRef;

	static constexpr const char* BlockName = "NiPathController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class NiPSysResetOnLoopCtlr : public NiObjectCRTP<NiPSysResetOnLoopCtlr, NiTimeController> {
public:
	static constexpr const char* BlockName = "NiPSysResetOnLoopCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiUVData : public NiObjectCRTP<NiUVData, NiObject, true> {
public:
	KeyGroup<float> uTrans;
	KeyGroup<float> vTrans;
	KeyGroup<float> uScale;
	KeyGroup<float> vScale;

	static constexpr const char* BlockName = "NiUVData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiUVController : public NiObjectCRTP<NiUVController, NiTimeController, true> {
public:
	uint16_t textureSet = 0;
	NiBlockRef<NiUVData> dataRef;

	static constexpr const char* BlockName = "NiUVController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class BSFrustumFOVController : public NiObjectCRTP<BSFrustumFOVController, NiTimeController, true> {
public:
	NiBlockRef<NiInterpolator> interpolatorRef;

	static constexpr const char* BlockName = "BSFrustumFOVController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class BSLagBoneController : public NiObjectCRTP<BSLagBoneController, NiTimeController, true> {
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
	: public NiObjectCRTP<BSProceduralLightningController, NiTimeController, true> {
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
	void GetChildIndices(std::vector<int>& indices) override;
};

class NiBoneLODController : public NiObjectCRTP<NiBoneLODController, NiTimeController, true> {
private:
	uint32_t boneArraysSize = 0;
	std::vector<NiBlockPtrArray<NiNode>> boneArrays;

public:
	uint32_t lod = 0;
	uint32_t numLODs = 0;

	static constexpr const char* BlockName = "NiBoneLODController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<NiPtr*>& ptrs) override;

	std::vector<NiBlockPtrArray<NiNode>> GetBoneArrays() const;
	void SetBoneArrays(const std::vector<NiBlockPtrArray<NiNode>>& ba);
};

class NiBSBoneLODController : public NiObjectCRTP<NiBSBoneLODController, NiBoneLODController> {
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

class NiMorphData : public NiObjectCRTP<NiMorphData, NiObject, true> {
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
	void SetMorphs(const std::vector<Morph>& m);
};

class NiInterpController : public NiObjectCRTP<NiInterpController, NiTimeController> {};

struct MorphWeight {
	NiBlockRef<NiInterpolator> interpRef;
	float weight = 0.0f;

	void Sync(NiStreamReversible& stream) {
		interpRef.Sync(stream);
		stream.Sync(weight);
	}

	void GetChildRefs(std::set<NiRef*>& refs) { refs.insert(&interpRef); }
	void GetChildIndices(std::vector<int>& indices) { indices.push_back(interpRef.index); }
};

class NiGeomMorpherController : public NiObjectCRTP<NiGeomMorpherController, NiInterpController, true> {
private:
	uint32_t numTargets = 0;
	std::vector<MorphWeight> interpWeights;

public:
	uint16_t extraFlags = 0;
	NiBlockRef<NiMorphData> dataRef;
	bool alwaysUpdate = false;

	static constexpr const char* BlockName = "NiGeomMorpherController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	std::vector<MorphWeight> GetInterpWeights() const;
	void SetInterpWeights(const std::vector<MorphWeight>& m);
};

class NiSingleInterpController : public NiObjectCRTP<NiSingleInterpController, NiInterpController, true> {
public:
	NiBlockRef<NiInterpController> interpolatorRef;

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class NiRollController : public NiObjectCRTP<NiRollController, NiSingleInterpController, true> {
public:
	NiBlockRef<NiFloatData> dataRef;

	static constexpr const char* BlockName = "NiRollController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

enum TargetColor : uint16_t { TC_AMBIENT, TC_DIFFUSE, TC_SPECULAR, TC_SELF_ILLUM };

class NiPoint3InterpController
	: public NiObjectCRTP<NiPoint3InterpController, NiSingleInterpController, true> {
public:
	TargetColor targetColor = TC_AMBIENT;

	void Sync(NiStreamReversible& stream);
};

class NiMaterialColorController : public NiObjectCRTP<NiMaterialColorController, NiPoint3InterpController> {
public:
	static constexpr const char* BlockName = "NiMaterialColorController";
	const char* GetBlockName() override { return BlockName; }
};

class NiLightColorController : public NiObjectCRTP<NiLightColorController, NiPoint3InterpController> {
public:
	static constexpr const char* BlockName = "NiLightColorController";
	const char* GetBlockName() override { return BlockName; }
};

class NiExtraDataController : public NiObjectCRTP<NiExtraDataController, NiSingleInterpController> {};

class NiFloatExtraDataController
	: public NiObjectCRTP<NiFloatExtraDataController, NiExtraDataController, true> {
public:
	NiStringRef extraData;

	static constexpr const char* BlockName = "NiFloatExtraDataController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
};

class NiVisData : public NiObjectCRTP<NiVisData, NiObject, true> {
private:
	uint32_t numKeys = 0;
	std::vector<Key<uint8_t>> keys;

public:
	static constexpr const char* BlockName = "NiVisData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	std::vector<Key<uint8_t>> GetKeys() const;
	void SetKeys(const std::vector<Key<uint8_t>>& k);
};

class NiBoolInterpController : public NiObjectCRTP<NiBoolInterpController, NiSingleInterpController> {};

class NiVisController : public NiObjectCRTP<NiVisController, NiBoolInterpController> {
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
	UNKNOWN2_MAP,
	DECAL_0_MAP,
	DECAL_1_MAP,
	DECAL_2_MAP,
	DECAL_3_MAP
};

class NiFloatInterpController : public NiObjectCRTP<NiFloatInterpController, NiSingleInterpController> {};

class BSRefractionFirePeriodController
	: public NiObjectCRTP<BSRefractionFirePeriodController, NiSingleInterpController> {
public:
	static constexpr const char* BlockName = "BSRefractionFirePeriodController";
	const char* GetBlockName() override { return BlockName; }
};

class NiSourceTexture;

class NiFlipController : public NiObjectCRTP<NiFlipController, NiFloatInterpController, true> {
public:
	TexType textureSlot = BASE_MAP;
	NiBlockRefArray<NiSourceTexture> sourceRefs;

	static constexpr const char* BlockName = "NiFlipController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

enum TexTransformType : uint32_t { TT_TRANSLATE_U, TT_TRANSLATE_V, TT_ROTATE, TT_SCALE_U, TT_SCALE_V };

class NiTextureTransformController
	: public NiObjectCRTP<NiTextureTransformController, NiFloatInterpController, true> {
public:
	uint8_t unkByte1 = 0;
	TexType textureSlot = BASE_MAP;
	TexTransformType operation = TT_TRANSLATE_U;

	static constexpr const char* BlockName = "NiTextureTransformController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiLightDimmerController : public NiObjectCRTP<NiLightDimmerController, NiFloatInterpController> {
public:
	static constexpr const char* BlockName = "NiLightDimmerController";
	const char* GetBlockName() override { return BlockName; }
};

class NiLightRadiusController : public NiObjectCRTP<NiLightRadiusController, NiFloatInterpController> {
public:
	static constexpr const char* BlockName = "NiLightRadiusController";
	const char* GetBlockName() override { return BlockName; }
};

class NiAlphaController : public NiObjectCRTP<NiAlphaController, NiFloatInterpController> {
public:
	static constexpr const char* BlockName = "NiAlphaController";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysUpdateCtlr : public NiObjectCRTP<NiPSysUpdateCtlr, NiTimeController> {
public:
	static constexpr const char* BlockName = "NiPSysUpdateCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class BSNiAlphaPropertyTestRefController
	: public NiObjectCRTP<BSNiAlphaPropertyTestRefController, NiAlphaController> {
public:
	static constexpr const char* BlockName = "BSNiAlphaPropertyTestRefController";
	const char* GetBlockName() override { return BlockName; }
};

class NiKeyframeController : public NiObjectCRTP<NiKeyframeController, NiSingleInterpController> {
public:
	static constexpr const char* BlockName = "NiKeyframeController";
	const char* GetBlockName() override { return BlockName; }
};

class NiTransformController : public NiObjectCRTP<NiTransformController, NiKeyframeController> {
public:
	static constexpr const char* BlockName = "NiTransformController";
	const char* GetBlockName() override { return BlockName; }
};

class BSMaterialEmittanceMultController
	: public NiObjectCRTP<BSMaterialEmittanceMultController, NiFloatInterpController> {
public:
	static constexpr const char* BlockName = "BSMaterialEmittanceMultController";
	const char* GetBlockName() override { return BlockName; }
};

class BSRefractionStrengthController
	: public NiObjectCRTP<BSRefractionStrengthController, NiFloatInterpController> {
public:
	static constexpr const char* BlockName = "BSRefractionStrengthController";
	const char* GetBlockName() override { return BlockName; }
};

class BSLightingShaderPropertyColorController
	: public NiObjectCRTP<BSLightingShaderPropertyColorController, NiFloatInterpController, true> {
public:
	uint32_t typeOfControlledColor = 0;

	static constexpr const char* BlockName = "BSLightingShaderPropertyColorController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSLightingShaderPropertyFloatController
	: public NiObjectCRTP<BSLightingShaderPropertyFloatController, NiFloatInterpController, true> {
public:
	uint32_t typeOfControlledVariable = 0;

	static constexpr const char* BlockName = "BSLightingShaderPropertyFloatController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSLightingShaderPropertyUShortController
	: public NiObjectCRTP<BSLightingShaderPropertyUShortController, NiFloatInterpController, true> {
public:
	uint32_t typeOfControlledVariable = 0;

	static constexpr const char* BlockName = "BSLightingShaderPropertyUShortController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSEffectShaderPropertyColorController
	: public NiObjectCRTP<BSEffectShaderPropertyColorController, NiFloatInterpController, true> {
public:
	uint32_t typeOfControlledColor = 0;

	static constexpr const char* BlockName = "BSEffectShaderPropertyColorController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSEffectShaderPropertyFloatController
	: public NiObjectCRTP<BSEffectShaderPropertyFloatController, NiFloatInterpController, true> {
public:
	uint32_t typeOfControlledVariable = 0;

	static constexpr const char* BlockName = "BSEffectShaderPropertyFloatController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiAVObject;

class NiMultiTargetTransformController
	: public NiObjectCRTP<NiMultiTargetTransformController, NiInterpController, true> {
public:
	NiBlockPtrShortArray<NiAVObject> targetRefs;

	static constexpr const char* BlockName = "NiMultiTargetTransformController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<NiPtr*>& ptrs) override;
};

class NiPSysModifierCtlr : public NiObjectCRTP<NiPSysModifierCtlr, NiSingleInterpController, true> {
public:
	NiStringRef modifierName;

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
};

class NiPSysModifierBoolCtlr : public NiObjectCRTP<NiPSysModifierBoolCtlr, NiPSysModifierCtlr> {};

class NiPSysModifierActiveCtlr : public NiObjectCRTP<NiPSysModifierActiveCtlr, NiPSysModifierBoolCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysModifierActiveCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysModifierFloatCtlr : public NiObjectCRTP<NiPSysModifierFloatCtlr, NiPSysModifierCtlr> {};

class NiPSysEmitterLifeSpanCtlr : public NiObjectCRTP<NiPSysEmitterLifeSpanCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysEmitterLifeSpanCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysEmitterSpeedCtlr : public NiObjectCRTP<NiPSysEmitterSpeedCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysEmitterSpeedCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysEmitterInitialRadiusCtlr
	: public NiObjectCRTP<NiPSysEmitterInitialRadiusCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysEmitterInitialRadiusCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysEmitterDeclinationCtlr
	: public NiObjectCRTP<NiPSysEmitterDeclinationCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysEmitterDeclinationCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysGravityStrengthCtlr : public NiObjectCRTP<NiPSysGravityStrengthCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysGravityStrengthCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysEmitterDeclinationVarCtlr
	: public NiObjectCRTP<NiPSysEmitterDeclinationVarCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysEmitterDeclinationVarCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysFieldMagnitudeCtlr : public NiObjectCRTP<NiPSysFieldMagnitudeCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysFieldMagnitudeCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysFieldAttenuationCtlr : public NiObjectCRTP<NiPSysFieldAttenuationCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysFieldAttenuationCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysFieldMaxDistanceCtlr : public NiObjectCRTP<NiPSysFieldMaxDistanceCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysFieldMaxDistanceCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysAirFieldAirFrictionCtlr
	: public NiObjectCRTP<NiPSysAirFieldAirFrictionCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysAirFieldAirFrictionCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysAirFieldInheritVelocityCtlr
	: public NiObjectCRTP<NiPSysAirFieldInheritVelocityCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysAirFieldInheritVelocityCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysAirFieldSpreadCtlr : public NiObjectCRTP<NiPSysAirFieldSpreadCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysAirFieldSpreadCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysInitialRotSpeedCtlr : public NiObjectCRTP<NiPSysInitialRotSpeedCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysInitialRotSpeedCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysInitialRotSpeedVarCtlr
	: public NiObjectCRTP<NiPSysInitialRotSpeedVarCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysInitialRotSpeedVarCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysInitialRotAngleCtlr : public NiObjectCRTP<NiPSysInitialRotAngleCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysInitialRotAngleCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysInitialRotAngleVarCtlr
	: public NiObjectCRTP<NiPSysInitialRotAngleVarCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysInitialRotAngleVarCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysEmitterPlanarAngleCtlr
	: public NiObjectCRTP<NiPSysEmitterPlanarAngleCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysEmitterPlanarAngleCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysEmitterPlanarAngleVarCtlr
	: public NiObjectCRTP<NiPSysEmitterPlanarAngleVarCtlr, NiPSysModifierFloatCtlr> {
public:
	static constexpr const char* BlockName = "NiPSysEmitterPlanarAngleVarCtlr";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysEmitterCtlr : public NiObjectCRTP<NiPSysEmitterCtlr, NiPSysModifierCtlr, true> {
public:
	NiBlockRef<NiInterpolator> visInterpolatorRef;

	static constexpr const char* BlockName = "NiPSysEmitterCtlr";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class BSMasterParticleSystem;

class BSPSysMultiTargetEmitterCtlr
	: public NiObjectCRTP<BSPSysMultiTargetEmitterCtlr, NiPSysEmitterCtlr, true> {
public:
	uint16_t maxEmitters = 0;
	NiBlockPtr<BSMasterParticleSystem> masterParticleSystemRef;

	static constexpr const char* BlockName = "BSPSysMultiTargetEmitterCtlr";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<NiPtr*>& ptrs) override;
};

class NiStringPalette : public NiObjectCRTP<NiStringPalette, NiObject, true> {
public:
	NiString palette;
	uint32_t length = 0;

	static constexpr const char* BlockName = "NiStringPalette";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

struct ControllerLink {
	NiBlockRef<NiInterpolator> interpolatorRef;
	NiBlockRef<NiTimeController> controllerRef;
	uint8_t priority = 0;

	NiStringRef nodeName;
	NiStringRef propType;
	NiStringRef ctrlType;
	NiStringRef ctrlID;
	NiStringRef interpID;
};

class NiSequence : public NiObjectCRTP<NiSequence, NiObject, true> {
private:
	uint32_t numControlledBlocks = 0;
	std::vector<ControllerLink> controlledBlocks;

public:
	NiStringRef name;
	uint32_t arrayGrowBy = 0;

	static constexpr const char* BlockName = "NiSequence";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	std::vector<ControllerLink> GetControlledBlocks() const;
	void SetControlledBlocks(const std::vector<ControllerLink>& cl);
};

enum CycleType : uint32_t { CYCLE_LOOP, CYCLE_REVERSE, CYCLE_CLAMP };

class BSAnimNote : public NiObjectCRTP<BSAnimNote, NiObject, true> {
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

class BSAnimNotes : public NiObjectCRTP<BSAnimNotes, NiObject, true> {
public:
	NiBlockRefShortArray<BSAnimNote> animNoteRefs;

	static constexpr const char* BlockName = "BSAnimNotes";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class NiControllerManager;

class NiControllerSequence : public NiObjectCRTP<NiControllerSequence, NiSequence, true> {
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
	void GetChildIndices(std::vector<int>& indices) override;
	void GetPtrs(std::set<NiPtr*>& ptrs) override;
};

class NiDefaultAVObjectPalette;

class NiControllerManager : public NiObjectCRTP<NiControllerManager, NiTimeController, true> {
public:
	bool cumulative = false;
	NiBlockRefArray<NiControllerSequence> controllerSequenceRefs;
	NiBlockRef<NiDefaultAVObjectPalette> objectPaletteRef;

	static constexpr const char* BlockName = "NiControllerManager";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};
} // namespace nifly
