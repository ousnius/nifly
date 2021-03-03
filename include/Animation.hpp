/*
BodySlide and Outfit Studio
See the included LICENSE file
*/

#pragma once

#include "BasicTypes.hpp"
#include "ExtraData.hpp"
#include "Keys.hpp"

namespace nifly {
class NiKeyframeData : public NiObject {
private:
	uint32_t numRotationKeys = 0;
	KeyType rotationType = NO_INTERP;
	std::vector<Key<Quaternion>> quaternionKeys;
	KeyGroup<float> xRotations;
	KeyGroup<float> yRotations;
	KeyGroup<float> zRotations;
	KeyGroup<Vector3> translations;
	KeyGroup<float> scales;

public:
	static constexpr const char* BlockName = "NiKeyframeData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiKeyframeData* Clone() override { return new NiKeyframeData(*this); }
};

class NiTransformData : public NiKeyframeData {
public:
	static constexpr const char* BlockName = "NiTransformData";
	const char* GetBlockName() override { return BlockName; }

	NiTransformData* Clone() override { return new NiTransformData(*this); }
};

class NiPosData : public NiObject {
private:
	KeyGroup<Vector3> data;

public:
	static constexpr const char* BlockName = "NiPosData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiPosData* Clone() override { return new NiPosData(*this); }
};

class NiBoolData : public NiObject {
private:
	KeyGroup<uint8_t> data;

public:
	static constexpr const char* BlockName = "NiBoolData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiBoolData* Clone() override { return new NiBoolData(*this); }
};

class NiFloatData : public NiObject {
private:
	KeyGroup<float> data;

public:
	static constexpr const char* BlockName = "NiFloatData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiFloatData* Clone() override { return new NiFloatData(*this); }
};

class NiBSplineData : public NiObject {
private:
	uint32_t numFloatControlPoints = 0;
	std::vector<float> floatControlPoints;

	uint32_t numShortControlPoints = 0;
	std::vector<short> shortControlPoints;

public:
	static constexpr const char* BlockName = "NiBSplineData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiBSplineData* Clone() override { return new NiBSplineData(*this); }
};

class NiBSplineBasisData : public NiObject {
private:
	uint32_t numControlPoints = 0;

public:
	static constexpr const char* BlockName = "NiBSplineBasisData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiBSplineBasisData* Clone() override { return new NiBSplineBasisData(*this); }
};

class NiInterpolator : public NiObject {};

class NiBSplineInterpolator : public NiInterpolator {
private:
	float startTime = 0.0f;
	float stopTime = 0.0f;
	BlockRef<NiBSplineData> splineDataRef;
	BlockRef<NiBSplineBasisData> basisDataRef;

public:
	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class NiBSplineFloatInterpolator : public NiBSplineInterpolator {};

class NiBSplineCompFloatInterpolator : public NiBSplineFloatInterpolator {
private:
	float base = 0.0f;
	uint32_t offset = 0;
	float bias = 0.0f;
	float multiplier = 0.0f;

public:
	static constexpr const char* BlockName = "NiBSplineCompFloatInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiBSplineCompFloatInterpolator* Clone() override { return new NiBSplineCompFloatInterpolator(*this); }
};

class NiBSplinePoint3Interpolator : public NiBSplineInterpolator {
private:
	float unkFloat1 = 0.0f;
	float unkFloat2 = 0.0f;
	float unkFloat3 = 0.0f;
	float unkFloat4 = 0.0f;
	float unkFloat5 = 0.0f;
	float unkFloat6 = 0.0f;

public:
	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
};

class NiBSplineCompPoint3Interpolator : public NiBSplinePoint3Interpolator {
public:
	static constexpr const char* BlockName = "NiBSplineCompPoint3Interpolator";
	const char* GetBlockName() override { return BlockName; }

	NiBSplineCompPoint3Interpolator* Clone() override { return new NiBSplineCompPoint3Interpolator(*this); }
};

class NiBSplineTransformInterpolator : public NiBSplineInterpolator {
private:
	Vector3 translation;
	Quaternion rotation;
	float scale = 1.0f;

	uint32_t translationOffset = 0;
	uint32_t rotationOffset = 0;
	uint32_t scaleOffset = 0;

public:
	static constexpr const char* BlockName = "NiBSplineTransformInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiBSplineTransformInterpolator* Clone() override { return new NiBSplineTransformInterpolator(*this); }
};

class NiBSplineCompTransformInterpolator : public NiBSplineTransformInterpolator {
private:
	float translationBias = 0.0f;
	float translationMultiplier = 0.0f;
	float rotationBias = 0.0f;
	float rotationMultiplier = 0.0f;
	float scaleBias = 0.0f;
	float scaleMultiplier = 0.0f;

public:
	static constexpr const char* BlockName = "NiBSplineCompTransformInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiBSplineCompTransformInterpolator* Clone() override {
		return new NiBSplineCompTransformInterpolator(*this);
	}
};

class NiBlendInterpolator : public NiInterpolator {
private:
	uint16_t flags = 0;
	uint32_t unkInt = 0;

public:
	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
};

class NiBlendBoolInterpolator : public NiBlendInterpolator {
private:
	bool value = false;

public:
	static constexpr const char* BlockName = "NiBlendBoolInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiBlendBoolInterpolator* Clone() override { return new NiBlendBoolInterpolator(*this); }
};

class NiBlendFloatInterpolator : public NiBlendInterpolator {
private:
	float value = 0.0f;

public:
	static constexpr const char* BlockName = "NiBlendFloatInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiBlendFloatInterpolator* Clone() override { return new NiBlendFloatInterpolator(*this); }
};

class NiBlendPoint3Interpolator : public NiBlendInterpolator {
private:
	Vector3 point;

public:
	static constexpr const char* BlockName = "NiBlendPoint3Interpolator";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiBlendPoint3Interpolator* Clone() override { return new NiBlendPoint3Interpolator(*this); }
};

class NiBlendTransformInterpolator : public NiBlendInterpolator {
public:
	static constexpr const char* BlockName = "NiBlendTransformInterpolator";
	const char* GetBlockName() override { return BlockName; }

	NiBlendTransformInterpolator* Clone() override { return new NiBlendTransformInterpolator(*this); }
};

class NiKeyBasedInterpolator : public NiInterpolator {};

class NiBoolInterpolator : public NiKeyBasedInterpolator {
private:
	uint8_t boolValue = 0;
	BlockRef<NiBoolData> dataRef;

public:
	static constexpr const char* BlockName = "NiBoolInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	NiBoolInterpolator* Clone() override { return new NiBoolInterpolator(*this); }

	int GetDataRef() { return dataRef.GetIndex(); }
	void SetDataRef(int datRef) { dataRef.SetIndex(datRef); }
};

class NiBoolTimelineInterpolator : public NiBoolInterpolator {
public:
	static constexpr const char* BlockName = "NiBoolTimelineInterpolator";
	const char* GetBlockName() override { return BlockName; }

	NiBoolTimelineInterpolator* Clone() override { return new NiBoolTimelineInterpolator(*this); }
};

class NiFloatInterpolator : public NiKeyBasedInterpolator {
private:
	float floatValue = 0.0f;
	BlockRef<NiFloatData> dataRef;

public:
	static constexpr const char* BlockName = "NiFloatInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	NiFloatInterpolator* Clone() override { return new NiFloatInterpolator(*this); }

	int GetDataRef() { return dataRef.GetIndex(); }
	void SetDataRef(int datRef) { dataRef.SetIndex(datRef); }
};

class NiTransformInterpolator : public NiKeyBasedInterpolator {
private:
	Vector3 translation;
	Quaternion rotation;
	float scale = 0.0f;
	BlockRef<NiTransformData> dataRef;

public:
	static constexpr const char* BlockName = "NiTransformInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	NiTransformInterpolator* Clone() override { return new NiTransformInterpolator(*this); }

	int GetDataRef() { return dataRef.GetIndex(); }
	void SetDataRef(int datRef) { dataRef.SetIndex(datRef); }
};

class BSRotAccumTransfInterpolator : public NiTransformInterpolator {
public:
	static constexpr const char* BlockName = "BSRotAccumTransfInterpolator";
	const char* GetBlockName() override { return BlockName; }

	BSRotAccumTransfInterpolator* Clone() override { return new BSRotAccumTransfInterpolator(*this); }
};

class NiPoint3Interpolator : public NiKeyBasedInterpolator {
private:
	Vector3 point3Value;
	BlockRef<NiPosData> dataRef;

public:
	static constexpr const char* BlockName = "NiPoint3Interpolator";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	NiPoint3Interpolator* Clone() override { return new NiPoint3Interpolator(*this); }

	int GetDataRef() { return dataRef.GetIndex(); }
	void SetDataRef(int datRef) { dataRef.SetIndex(datRef); }
};

class NiPathInterpolator : public NiKeyBasedInterpolator {
private:
	uint16_t flags = 0;
	uint32_t bankDir = 0;
	float maxBankAngle = 0.0f;
	float smoothing = 0.0f;
	uint16_t followAxis = 0;

	BlockRef<NiPosData> pathDataRef;
	BlockRef<NiFloatData> percentDataRef;

public:
	static constexpr const char* BlockName = "NiPathInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	NiPathInterpolator* Clone() override { return new NiPathInterpolator(*this); }
};

enum LookAtFlags : uint16_t {
	LOOK_X_AXIS = 0x0000,
	LOOK_FLIP = 0x0001,
	LOOK_Y_AXIS = 0x0002,
	LOOK_Z_AXIS = 0x0004
};

class NiNode;

class NiLookAtInterpolator : public NiInterpolator {
private:
	LookAtFlags flags = LOOK_X_AXIS;
	BlockRef<NiNode> lookAtRef;

	StringRef lookAtName;

	QuatTransform transform;
	BlockRef<NiPoint3Interpolator> translateInterpRef;
	BlockRef<NiFloatInterpolator> rollInterpRef;
	BlockRef<NiFloatInterpolator> scaleInterpRef;

public:
	static constexpr const char* BlockName = "NiLookAtInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::set<StringRef*>& refs) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;
	NiLookAtInterpolator* Clone() override { return new NiLookAtInterpolator(*this); }
};

struct BSTreadTransformData {
	Vector3 translation;
	Quaternion rotation;
	float scale = 1.0f;
};

struct BSTreadTransform {
	StringRef name;
	BSTreadTransformData transform1;
	BSTreadTransformData transform2;

	void Get(NiStream& stream) {
		name.Get(stream);
		stream >> transform1;
		stream >> transform2;
	}

	void Put(NiStream& stream) {
		name.Put(stream);
		stream << transform1;
		stream << transform2;
	}

	void GetStringRefs(std::set<StringRef*>& refs) { refs.insert(&name); }
};

class BSTreadTransfInterpolator : public NiInterpolator {
private:
	uint32_t numTreadTransforms = 0;
	std::vector<BSTreadTransform> treadTransforms;
	BlockRef<NiFloatData> dataRef;

public:
	static constexpr const char* BlockName = "BSTreadTransfInterpolator";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::set<StringRef*>& refs) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	BSTreadTransfInterpolator* Clone() override { return new BSTreadTransfInterpolator(*this); }
};

class NiObjectNET;

class NiTimeController : public NiObject {
private:
	BlockRef<NiTimeController> nextControllerRef;
	uint16_t flags = 0x000C;
	float frequency = 1.0f;
	float phase = 0.0f;
	float startTime = 0.0f;
	float stopTime = 0.0f;
	BlockRef<NiObjectNET> targetRef;

public:
	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;

	int GetNextControllerRef() { return nextControllerRef.GetIndex(); }
	void SetNextControllerRef(int ctlrRef) { nextControllerRef.SetIndex(ctlrRef); }

	int GetTargetRef() { return targetRef.GetIndex(); }
	void SetTargetRef(int targRef) { targetRef.SetIndex(targRef); }
};

class NiLookAtController : public NiTimeController {
private:
	uint16_t unkShort1 = 0;
	BlockRef<NiNode> lookAtNodePtr;

public:
	static constexpr const char* BlockName = "NiLookAtController";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;
	NiLookAtController* Clone() override { return new NiLookAtController(*this); }
};

class NiPathController : public NiTimeController {
private:
	uint16_t unkShort1 = 0;
	uint32_t unkInt1 = 1;
	float unkFloat1 = 0.0f;
	float unkFloat2 = 0.0f;
	uint16_t unkShort2 = 0;
	BlockRef<NiPosData> posDataRef;
	BlockRef<NiFloatData> floatDataRef;

public:
	static constexpr const char* BlockName = "NiPathController";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	NiPathController* Clone() override { return new NiPathController(*this); }
};

class NiPSysResetOnLoopCtlr : public NiTimeController {
public:
	static constexpr const char* BlockName = "NiPSysResetOnLoopCtlr";
	const char* GetBlockName() override { return BlockName; }

	NiPSysResetOnLoopCtlr* Clone() override { return new NiPSysResetOnLoopCtlr(*this); }
};

class NiUVData : public NiObject {
private:
	KeyGroup<float> uTrans;
	KeyGroup<float> vTrans;
	KeyGroup<float> uScale;
	KeyGroup<float> vScale;

public:
	static constexpr const char* BlockName = "NiUVData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiUVData* Clone() override { return new NiUVData(*this); }
};

class NiUVController : public NiTimeController {
private:
	uint16_t textureSet = 0;
	BlockRef<NiUVData> dataRef;

public:
	static constexpr const char* BlockName = "NiUVController";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	NiUVController* Clone() override { return new NiUVController(*this); }
};

class BSFrustumFOVController : public NiTimeController {
private:
	BlockRef<NiInterpolator> interpolatorRef;

public:
	static constexpr const char* BlockName = "BSFrustumFOVController";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	BSFrustumFOVController* Clone() override { return new BSFrustumFOVController(*this); }

	int GetInterpolatorRef() { return interpolatorRef.GetIndex(); }
	void SetInterpolatorRef(int interpRef) { interpolatorRef.SetIndex(interpRef); }
};

class BSLagBoneController : public NiTimeController {
private:
	float linearVelocity = 0.0f;
	float linearRotation = 0.0f;
	float maxDistance = 0.0f;

public:
	static constexpr const char* BlockName = "BSLagBoneController";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSLagBoneController* Clone() override { return new BSLagBoneController(*this); }
};

class BSShaderProperty;

class BSProceduralLightningController : public NiTimeController {
private:
	BlockRef<NiInterpolator> generationInterpRef;
	BlockRef<NiInterpolator> mutationInterpRef;
	BlockRef<NiInterpolator> subdivisionInterpRef;
	BlockRef<NiInterpolator> numBranchesInterpRef;
	BlockRef<NiInterpolator> numBranchesVarInterpRef;
	BlockRef<NiInterpolator> lengthInterpRef;
	BlockRef<NiInterpolator> lengthVarInterpRef;
	BlockRef<NiInterpolator> widthInterpRef;
	BlockRef<NiInterpolator> arcOffsetInterpRef;

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

	BlockRef<BSShaderProperty> shaderPropertyRef;

public:
	static constexpr const char* BlockName = "BSProceduralLightningController";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	BSProceduralLightningController* Clone() override { return new BSProceduralLightningController(*this); }

	int GetGenerationInterpRef() { return generationInterpRef.GetIndex(); }
	void SetGenerationInterpRef(int interpRef) { generationInterpRef.SetIndex(interpRef); }

	int GetMutationInterpRef() { return mutationInterpRef.GetIndex(); }
	void SetMutationInterpRef(int interpRef) { mutationInterpRef.SetIndex(interpRef); }

	int GetSubdivisionInterpRef() { return subdivisionInterpRef.GetIndex(); }
	void SetSubdivisionInterpRef(int interpRef) { subdivisionInterpRef.SetIndex(interpRef); }

	int GetNumBranchesInterpRef() { return numBranchesInterpRef.GetIndex(); }
	void SetNumBranchesInterpRef(int interpRef) { numBranchesInterpRef.SetIndex(interpRef); }

	int GetNumBranchesVarInterpRef() { return numBranchesVarInterpRef.GetIndex(); }
	void SetNumBranchesVarInterpRef(int interpRef) { numBranchesVarInterpRef.SetIndex(interpRef); }

	int GetLengthInterpRef() { return lengthInterpRef.GetIndex(); }
	void SetLengthInterpRef(int interpRef) { lengthInterpRef.SetIndex(interpRef); }

	int GetLengthVarInterpRef() { return lengthVarInterpRef.GetIndex(); }
	void SetLengthVarInterpRef(int interpRef) { lengthVarInterpRef.SetIndex(interpRef); }

	int GetWidthInterpRef() { return widthInterpRef.GetIndex(); }
	void SetWidthInterpRef(int interpRef) { widthInterpRef.SetIndex(interpRef); }

	int GetArcOffsetInterpRef() { return arcOffsetInterpRef.GetIndex(); }
	void SetArcOffsetInterpRef(int interpRef) { arcOffsetInterpRef.SetIndex(interpRef); }

	int GetShaderPropertyRef() { return shaderPropertyRef.GetIndex(); }
	void SetShaderPropertyRef(int shaderRef) { shaderPropertyRef.SetIndex(shaderRef); }
};

class NiBoneLODController : public NiTimeController {
private:
	uint32_t lod = 0;
	uint32_t numLODs = 0;
	uint32_t boneArraysSize = 0;
	std::vector<BlockRefArray<NiNode>> boneArrays;

public:
	static constexpr const char* BlockName = "NiBoneLODController";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;
	NiBoneLODController* Clone() override { return new NiBoneLODController(*this); }
};

class NiBSBoneLODController : public NiBoneLODController {
public:
	static constexpr const char* BlockName = "NiBSBoneLODController";
	const char* GetBlockName() override { return BlockName; }

	NiBSBoneLODController* Clone() override { return new NiBSBoneLODController(*this); }
};

struct Morph {
	StringRef frameName;
	std::vector<Vector3> vectors;

	void Get(NiStream& stream, uint32_t numVerts) {
		frameName.Get(stream);
		vectors.resize(numVerts);
		for (int i = 0; i < numVerts; i++)
			stream >> vectors[i];
	}

	void Put(NiStream& stream, uint32_t numVerts) {
		frameName.Put(stream);
		for (int i = 0; i < numVerts; i++)
			stream << vectors[i];
	}

	void GetStringRefs(std::set<StringRef*>& refs) { refs.insert(&frameName); }
};

class NiMorphData : public NiObject {
private:
	uint32_t numMorphs = 0;
	uint32_t numVertices = 0;
	uint8_t relativeTargets = 1;
	std::vector<Morph> morphs;

public:
	static constexpr const char* BlockName = "NiMorphData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::set<StringRef*>& refs) override;

	NiMorphData* Clone() override { return new NiMorphData(*this); }
};

class NiInterpController : public NiTimeController {};

struct MorphWeight {
	BlockRef<NiInterpolator> interpRef;
	float weight = 0.0f;

	void Get(NiStream& stream) {
		interpRef.Get(stream);
		stream >> weight;
	}

	void Put(NiStream& stream) {
		interpRef.Put(stream);
		stream << weight;
	}

	void GetChildRefs(std::set<Ref*>& refs) { refs.insert(&interpRef); }

	void GetChildIndices(std::vector<int>& indices) { indices.push_back(interpRef.GetIndex()); }
};

class NiGeomMorpherController : public NiInterpController {
private:
	uint16_t extraFlags = 0;
	BlockRef<NiMorphData> dataRef;
	bool alwaysUpdate = false;

	uint32_t numTargets = 0;
	std::vector<MorphWeight> interpWeights;

public:
	static constexpr const char* BlockName = "NiGeomMorpherController";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	NiGeomMorpherController* Clone() override { return new NiGeomMorpherController(*this); }
};

class NiSingleInterpController : public NiInterpController {
private:
	BlockRef<NiInterpController> interpolatorRef;

public:
	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	int GetInterpolatorRef() { return interpolatorRef.GetIndex(); }
	void SetInterpolatorRef(int interpRef) { interpolatorRef.SetIndex(interpRef); }
};

class NiRollController : public NiSingleInterpController {
private:
	BlockRef<NiFloatData> dataRef;

public:
	static constexpr const char* BlockName = "NiRollController";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	NiRollController* Clone() override { return new NiRollController(*this); }
};

enum TargetColor : uint16_t { TC_AMBIENT, TC_DIFFUSE, TC_SPECULAR, TC_SELF_ILLUM };

class NiPoint3InterpController : public NiSingleInterpController {
private:
	TargetColor targetColor = TC_AMBIENT;

public:
	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
};

class NiMaterialColorController : public NiPoint3InterpController {
public:
	static constexpr const char* BlockName = "NiMaterialColorController";
	const char* GetBlockName() override { return BlockName; }

	NiMaterialColorController* Clone() override { return new NiMaterialColorController(*this); }
};

class NiLightColorController : public NiPoint3InterpController {
public:
	static constexpr const char* BlockName = "NiLightColorController";
	const char* GetBlockName() override { return BlockName; }

	NiLightColorController* Clone() override { return new NiLightColorController(*this); }
};

class NiExtraDataController : public NiSingleInterpController {};

class NiFloatExtraDataController : public NiExtraDataController {
private:
	StringRef extraData;

public:
	static constexpr const char* BlockName = "NiFloatExtraDataController";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::set<StringRef*>& refs) override;
	NiFloatExtraDataController* Clone() override { return new NiFloatExtraDataController(*this); }
};

class NiVisData : public NiObject {
private:
	uint32_t numKeys = 0;
	std::vector<Key<uint8_t>> keys;

public:
	static constexpr const char* BlockName = "NiVisData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	NiVisData* Clone() override { return new NiVisData(*this); }
};

class NiBoolInterpController : public NiSingleInterpController {};

class NiVisController : public NiBoolInterpController {
public:
	static constexpr const char* BlockName = "NiVisController";
	const char* GetBlockName() override { return BlockName; }

	NiVisController* Clone() override { return new NiVisController(*this); }
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

class NiFloatInterpController : public NiSingleInterpController {};

class BSRefractionFirePeriodController : public NiSingleInterpController {
public:
	static constexpr const char* BlockName = "BSRefractionFirePeriodController";
	const char* GetBlockName() override { return BlockName; }

	BSRefractionFirePeriodController* Clone() override { return new BSRefractionFirePeriodController(*this); }
};

class NiSourceTexture;

class NiFlipController : public NiFloatInterpController {
private:
	TexType textureSlot = BASE_MAP;
	BlockRefArray<NiSourceTexture> sourceRefs;

public:
	static constexpr const char* BlockName = "NiFlipController";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	NiFlipController* Clone() override { return new NiFlipController(*this); }

	BlockRefArray<NiSourceTexture>& GetSources();
};

enum TexTransformType : uint32_t { TT_TRANSLATE_U, TT_TRANSLATE_V, TT_ROTATE, TT_SCALE_U, TT_SCALE_V };

class NiTextureTransformController : public NiFloatInterpController {
private:
	uint8_t unkByte1 = 0;
	TexType textureSlot = BASE_MAP;
	TexTransformType operation = TT_TRANSLATE_U;

public:
	static constexpr const char* BlockName = "NiTextureTransformController";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	NiTextureTransformController* Clone() override { return new NiTextureTransformController(*this); }
};

class NiLightDimmerController : public NiFloatInterpController {
public:
	static constexpr const char* BlockName = "NiLightDimmerController";
	const char* GetBlockName() override { return BlockName; }

	NiLightDimmerController* Clone() override { return new NiLightDimmerController(*this); }
};

class NiLightRadiusController : public NiFloatInterpController {
public:
	static constexpr const char* BlockName = "NiLightRadiusController";
	const char* GetBlockName() override { return BlockName; }

	NiLightRadiusController* Clone() override { return new NiLightRadiusController(*this); }
};

class NiAlphaController : public NiFloatInterpController {
public:
	static constexpr const char* BlockName = "NiAlphaController";
	const char* GetBlockName() override { return BlockName; }

	NiAlphaController* Clone() override { return new NiAlphaController(*this); }
};

class NiPSysUpdateCtlr : public NiTimeController {
public:
	static constexpr const char* BlockName = "NiPSysUpdateCtlr";
	const char* GetBlockName() override { return BlockName; }

	NiPSysUpdateCtlr* Clone() override { return new NiPSysUpdateCtlr(*this); }
};

class BSNiAlphaPropertyTestRefController : public NiAlphaController {
public:
	static constexpr const char* BlockName = "BSNiAlphaPropertyTestRefController";
	const char* GetBlockName() override { return BlockName; }

	BSNiAlphaPropertyTestRefController* Clone() override {
		return new BSNiAlphaPropertyTestRefController(*this);
	}
};

class NiKeyframeController : public NiSingleInterpController {
public:
	static constexpr const char* BlockName = "NiKeyframeController";
	const char* GetBlockName() override { return BlockName; }

	NiKeyframeController* Clone() override { return new NiKeyframeController(*this); }
};

class NiTransformController : public NiKeyframeController {
public:
	static constexpr const char* BlockName = "NiTransformController";
	const char* GetBlockName() override { return BlockName; }

	NiTransformController* Clone() override { return new NiTransformController(*this); }
};

class BSMaterialEmittanceMultController : public NiFloatInterpController {
public:
	static constexpr const char* BlockName = "BSMaterialEmittanceMultController";
	const char* GetBlockName() override { return BlockName; }

	BSMaterialEmittanceMultController* Clone() override {
		return new BSMaterialEmittanceMultController(*this);
	}
};

class BSRefractionStrengthController : public NiFloatInterpController {
public:
	static constexpr const char* BlockName = "BSRefractionStrengthController";
	const char* GetBlockName() override { return BlockName; }

	BSRefractionStrengthController* Clone() override { return new BSRefractionStrengthController(*this); }
};

class BSLightingShaderPropertyColorController : public NiFloatInterpController {
private:
	uint32_t typeOfControlledColor = 0;

public:
	static constexpr const char* BlockName = "BSLightingShaderPropertyColorController";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSLightingShaderPropertyColorController* Clone() {
		return new BSLightingShaderPropertyColorController(*this);
	}
};

class BSLightingShaderPropertyFloatController : public NiFloatInterpController {
private:
	uint32_t typeOfControlledVariable = 0;

public:
	static constexpr const char* BlockName = "BSLightingShaderPropertyFloatController";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSLightingShaderPropertyFloatController* Clone() {
		return new BSLightingShaderPropertyFloatController(*this);
	}
};

class BSLightingShaderPropertyUShortController : public NiFloatInterpController {
private:
	uint32_t typeOfControlledVariable = 0;

public:
	static constexpr const char* BlockName = "BSLightingShaderPropertyUShortController";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSLightingShaderPropertyUShortController* Clone() {
		return new BSLightingShaderPropertyUShortController(*this);
	}
};

class BSEffectShaderPropertyColorController : public NiFloatInterpController {
private:
	uint32_t typeOfControlledColor = 0;

public:
	static constexpr const char* BlockName = "BSEffectShaderPropertyColorController";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSEffectShaderPropertyColorController* Clone() {
		return new BSEffectShaderPropertyColorController(*this);
	}
};

class BSEffectShaderPropertyFloatController : public NiFloatInterpController {
private:
	uint32_t typeOfControlledVariable = 0;

public:
	static constexpr const char* BlockName = "BSEffectShaderPropertyFloatController";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSEffectShaderPropertyFloatController* Clone() {
		return new BSEffectShaderPropertyFloatController(*this);
	}
};

class NiAVObject;

class NiMultiTargetTransformController : public NiInterpController {
private:
	BlockRefShortArray<NiAVObject> targetRefs;

public:
	static constexpr const char* BlockName = "NiMultiTargetTransformController";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;
	NiMultiTargetTransformController* Clone() override { return new NiMultiTargetTransformController(*this); }

	BlockRefShortArray<NiAVObject>& GetTargets();
};

class NiPSysModifierCtlr : public NiSingleInterpController {
private:
	StringRef modifierName;

public:
	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::set<StringRef*>& refs) override;
};

class NiPSysModifierBoolCtlr : public NiPSysModifierCtlr {};

class NiPSysModifierActiveCtlr : public NiPSysModifierBoolCtlr {
public:
	static constexpr const char* BlockName = "NiPSysModifierActiveCtlr";
	const char* GetBlockName() override { return BlockName; }

	NiPSysModifierActiveCtlr* Clone() override { return new NiPSysModifierActiveCtlr(*this); }
};

class NiPSysModifierFloatCtlr : public NiPSysModifierCtlr {};

class NiPSysEmitterLifeSpanCtlr : public NiPSysModifierFloatCtlr {
public:
	static constexpr const char* BlockName = "NiPSysEmitterLifeSpanCtlr";
	const char* GetBlockName() override { return BlockName; }

	NiPSysEmitterLifeSpanCtlr* Clone() override { return new NiPSysEmitterLifeSpanCtlr(*this); }
};

class NiPSysEmitterSpeedCtlr : public NiPSysModifierFloatCtlr {
public:
	static constexpr const char* BlockName = "NiPSysEmitterSpeedCtlr";
	const char* GetBlockName() override { return BlockName; }

	NiPSysEmitterSpeedCtlr* Clone() override { return new NiPSysEmitterSpeedCtlr(*this); }
};

class NiPSysEmitterInitialRadiusCtlr : public NiPSysModifierFloatCtlr {
public:
	static constexpr const char* BlockName = "NiPSysEmitterInitialRadiusCtlr";
	const char* GetBlockName() override { return BlockName; }

	NiPSysEmitterInitialRadiusCtlr* Clone() override { return new NiPSysEmitterInitialRadiusCtlr(*this); }
};

class NiPSysEmitterDeclinationCtlr : public NiPSysModifierFloatCtlr {
public:
	static constexpr const char* BlockName = "NiPSysEmitterDeclinationCtlr";
	const char* GetBlockName() override { return BlockName; }

	NiPSysEmitterDeclinationCtlr* Clone() override { return new NiPSysEmitterDeclinationCtlr(*this); }
};

class NiPSysGravityStrengthCtlr : public NiPSysModifierFloatCtlr {
public:
	static constexpr const char* BlockName = "NiPSysGravityStrengthCtlr";
	const char* GetBlockName() override { return BlockName; }

	NiPSysGravityStrengthCtlr* Clone() override { return new NiPSysGravityStrengthCtlr(*this); }
};

class NiPSysEmitterDeclinationVarCtlr : public NiPSysModifierFloatCtlr {
public:
	static constexpr const char* BlockName = "NiPSysEmitterDeclinationVarCtlr";
	const char* GetBlockName() override { return BlockName; }

	NiPSysEmitterDeclinationVarCtlr* Clone() override { return new NiPSysEmitterDeclinationVarCtlr(*this); }
};

class NiPSysFieldMagnitudeCtlr : public NiPSysModifierFloatCtlr {
public:
	static constexpr const char* BlockName = "NiPSysFieldMagnitudeCtlr";
	const char* GetBlockName() override { return BlockName; }

	NiPSysFieldMagnitudeCtlr* Clone() override { return new NiPSysFieldMagnitudeCtlr(*this); }
};

class NiPSysFieldAttenuationCtlr : public NiPSysModifierFloatCtlr {
public:
	static constexpr const char* BlockName = "NiPSysFieldAttenuationCtlr";
	const char* GetBlockName() override { return BlockName; }

	NiPSysFieldAttenuationCtlr* Clone() override { return new NiPSysFieldAttenuationCtlr(*this); }
};

class NiPSysFieldMaxDistanceCtlr : public NiPSysModifierFloatCtlr {
public:
	static constexpr const char* BlockName = "NiPSysFieldMaxDistanceCtlr";
	const char* GetBlockName() override { return BlockName; }

	NiPSysFieldMaxDistanceCtlr* Clone() override { return new NiPSysFieldMaxDistanceCtlr(*this); }
};

class NiPSysAirFieldAirFrictionCtlr : public NiPSysModifierFloatCtlr {
public:
	static constexpr const char* BlockName = "NiPSysAirFieldAirFrictionCtlr";
	const char* GetBlockName() override { return BlockName; }

	NiPSysAirFieldAirFrictionCtlr* Clone() override { return new NiPSysAirFieldAirFrictionCtlr(*this); }
};

class NiPSysAirFieldInheritVelocityCtlr : public NiPSysModifierFloatCtlr {
public:
	static constexpr const char* BlockName = "NiPSysAirFieldInheritVelocityCtlr";
	const char* GetBlockName() override { return BlockName; }

	NiPSysAirFieldInheritVelocityCtlr* Clone() override {
		return new NiPSysAirFieldInheritVelocityCtlr(*this);
	}
};

class NiPSysAirFieldSpreadCtlr : public NiPSysModifierFloatCtlr {
public:
	static constexpr const char* BlockName = "NiPSysAirFieldSpreadCtlr";
	const char* GetBlockName() override { return BlockName; }

	NiPSysAirFieldSpreadCtlr* Clone() override { return new NiPSysAirFieldSpreadCtlr(*this); }
};

class NiPSysInitialRotSpeedCtlr : public NiPSysModifierFloatCtlr {
public:
	static constexpr const char* BlockName = "NiPSysInitialRotSpeedCtlr";
	const char* GetBlockName() override { return BlockName; }

	NiPSysInitialRotSpeedCtlr* Clone() override { return new NiPSysInitialRotSpeedCtlr(*this); }
};

class NiPSysInitialRotSpeedVarCtlr : public NiPSysModifierFloatCtlr {
public:
	static constexpr const char* BlockName = "NiPSysInitialRotSpeedVarCtlr";
	const char* GetBlockName() override { return BlockName; }

	NiPSysInitialRotSpeedVarCtlr* Clone() override { return new NiPSysInitialRotSpeedVarCtlr(*this); }
};

class NiPSysInitialRotAngleCtlr : public NiPSysModifierFloatCtlr {
public:
	static constexpr const char* BlockName = "NiPSysInitialRotAngleCtlr";
	const char* GetBlockName() override { return BlockName; }

	NiPSysInitialRotAngleCtlr* Clone() override { return new NiPSysInitialRotAngleCtlr(*this); }
};

class NiPSysInitialRotAngleVarCtlr : public NiPSysModifierFloatCtlr {
public:
	static constexpr const char* BlockName = "NiPSysInitialRotAngleVarCtlr";
	const char* GetBlockName() override { return BlockName; }

	NiPSysInitialRotAngleVarCtlr* Clone() override { return new NiPSysInitialRotAngleVarCtlr(*this); }
};

class NiPSysEmitterPlanarAngleCtlr : public NiPSysModifierFloatCtlr {
public:
	static constexpr const char* BlockName = "NiPSysEmitterPlanarAngleCtlr";
	const char* GetBlockName() override { return BlockName; }

	NiPSysEmitterPlanarAngleCtlr* Clone() override { return new NiPSysEmitterPlanarAngleCtlr(*this); }
};

class NiPSysEmitterPlanarAngleVarCtlr : public NiPSysModifierFloatCtlr {
public:
	static constexpr const char* BlockName = "NiPSysEmitterPlanarAngleVarCtlr";
	const char* GetBlockName() override { return BlockName; }

	NiPSysEmitterPlanarAngleVarCtlr* Clone() override { return new NiPSysEmitterPlanarAngleVarCtlr(*this); }
};

class NiPSysEmitterCtlr : public NiPSysModifierCtlr {
private:
	BlockRef<NiInterpolator> visInterpolatorRef;

public:
	static constexpr const char* BlockName = "NiPSysEmitterCtlr";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	NiPSysEmitterCtlr* Clone() override { return new NiPSysEmitterCtlr(*this); }
};

class BSMasterParticleSystem;

class BSPSysMultiTargetEmitterCtlr : public NiPSysEmitterCtlr {
private:
	uint16_t maxEmitters = 0;
	BlockRef<BSMasterParticleSystem> masterParticleSystemRef;

public:
	static constexpr const char* BlockName = "BSPSysMultiTargetEmitterCtlr";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;
	BSPSysMultiTargetEmitterCtlr* Clone() override { return new BSPSysMultiTargetEmitterCtlr(*this); }
};

class NiStringPalette : public NiObject {
private:
	NiString palette;
	uint32_t length = 0;

public:
	static constexpr const char* BlockName = "NiStringPalette";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	NiStringPalette* Clone() override { return new NiStringPalette(*this); }
};

struct ControllerLink {
	BlockRef<NiInterpolator> interpolatorRef;
	BlockRef<NiTimeController> controllerRef;
	uint8_t priority = 0;

	StringRef nodeName;
	StringRef propType;
	StringRef ctrlType;
	StringRef ctrlID;
	StringRef interpID;
};

class NiSequence : public NiObject {
private:
	StringRef name;
	uint32_t numControlledBlocks = 0;
	uint32_t arrayGrowBy = 0;
	std::vector<ControllerLink> controlledBlocks;

public:
	static constexpr const char* BlockName = "NiSequence";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::set<StringRef*>& refs) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	NiSequence* Clone() override { return new NiSequence(*this); }
};

enum CycleType : uint32_t { CYCLE_LOOP, CYCLE_REVERSE, CYCLE_CLAMP };

class BSAnimNote : public NiObject {
public:
	enum AnimNoteType : uint32_t { ANT_INVALID, ANT_GRABIK, ANT_LOOKIK };

private:
	AnimNoteType type = ANT_INVALID;
	float time = 0.0f;
	uint32_t arm = 0;
	float gain = 0.0f;
	uint32_t state = 0;

public:
	static constexpr const char* BlockName = "BSAnimNote";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	BSAnimNote* Clone() override { return new BSAnimNote(*this); }
};

class BSAnimNotes : public NiObject {
private:
	BlockRefShortArray<BSAnimNote> animNoteRefs;

public:
	static constexpr const char* BlockName = "BSAnimNotes";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	BSAnimNotes* Clone() override { return new BSAnimNotes(*this); }

	BlockRefShortArray<BSAnimNote>& GetAnimNotes();
};

class NiControllerManager;

class NiControllerSequence : public NiSequence {
private:
	float weight = 1.0f;
	BlockRef<NiTextKeyExtraData> textKeyRef;
	CycleType cycleType = CYCLE_LOOP;
	float frequency = 0.0f;
	float startTime = 0.0f;
	float stopTime = 0.0f;
	BlockRef<NiControllerManager> managerRef;
	StringRef accumRootName;

	BlockRef<BSAnimNotes> animNotesRef;
	BlockRefShortArray<BSAnimNotes> animNotesRefs;

public:
	static constexpr const char* BlockName = "NiControllerSequence";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::set<StringRef*>& refs) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;
	NiControllerSequence* Clone() override { return new NiControllerSequence(*this); }

	int GetAnimNotesRef();
	void SetAnimNotesRef(int notesRef);

	BlockRefShortArray<BSAnimNotes>& GetAnimNotes();
};

class NiDefaultAVObjectPalette;

class NiControllerManager : public NiTimeController {
private:
	bool cumulative = false;
	BlockRefArray<NiControllerSequence> controllerSequenceRefs;
	BlockRef<NiDefaultAVObjectPalette> objectPaletteRef;

public:
	static constexpr const char* BlockName = "NiControllerManager";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	NiControllerManager* Clone() override { return new NiControllerManager(*this); }

	BlockRefArray<NiControllerSequence>& GetControllerSequences();

	int GetObjectPaletteRef();
	void SetObjectPaletteRef(int paletteRef);
};
} // namespace nifly
