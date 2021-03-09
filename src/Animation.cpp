/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#include "Animation.hpp"

using namespace nifly;

void NiKeyframeData::Get(NiStream& stream) {
	NiObject::Get(stream);

	stream >> numRotationKeys;

	if (numRotationKeys > 0) {
		stream >> rotationType;

		if (rotationType != XYZ_ROTATION_KEY) {
			quaternionKeys.resize(numRotationKeys);

			for (uint32_t i = 0; i < numRotationKeys; i++) {
				stream >> quaternionKeys[i].time;
				stream >> quaternionKeys[i].value;

				if (rotationType == TBC_KEY)
					stream >> quaternionKeys[i].tbc;
			}
		}
		else {
			xRotations.Get(stream);
			yRotations.Get(stream);
			zRotations.Get(stream);
		}
	}

	translations.Get(stream);
	scales.Get(stream);
}

void NiKeyframeData::Put(NiStream& stream) {
	NiObject::Put(stream);

	stream << numRotationKeys;

	if (numRotationKeys > 0) {
		stream << rotationType;

		if (rotationType != XYZ_ROTATION_KEY) {
			for (uint32_t i = 0; i < numRotationKeys; i++) {
				stream << quaternionKeys[i].time;
				stream << quaternionKeys[i].value;

				if (rotationType == TBC_KEY)
					stream << quaternionKeys[i].tbc;
			}
		}
		else {
			xRotations.Put(stream);
			yRotations.Put(stream);
			zRotations.Put(stream);
		}
	}

	translations.Put(stream);
	scales.Put(stream);
}


void NiPosData::Get(NiStream& stream) {
	NiObject::Get(stream);

	data.Get(stream);
}

void NiPosData::Put(NiStream& stream) {
	NiObject::Put(stream);

	data.Put(stream);
}


void NiBoolData::Get(NiStream& stream) {
	NiObject::Get(stream);

	data.Get(stream);
}

void NiBoolData::Put(NiStream& stream) {
	NiObject::Put(stream);

	data.Put(stream);
}


void NiFloatData::Get(NiStream& stream) {
	NiObject::Get(stream);

	data.Get(stream);
}

void NiFloatData::Put(NiStream& stream) {
	NiObject::Put(stream);

	data.Put(stream);
}


void NiBSplineData::Get(NiStream& stream) {
	NiObject::Get(stream);

	stream >> numFloatControlPoints;
	floatControlPoints.resize(numFloatControlPoints);
	for (uint32_t i = 0; i < numFloatControlPoints; i++)
		stream >> floatControlPoints[i];

	stream >> numShortControlPoints;
	shortControlPoints.resize(numShortControlPoints);
	for (uint32_t i = 0; i < numShortControlPoints; i++)
		stream >> shortControlPoints[i];
}

void NiBSplineData::Put(NiStream& stream) {
	NiObject::Put(stream);

	stream << numFloatControlPoints;
	for (uint32_t i = 0; i < numFloatControlPoints; i++)
		stream << floatControlPoints[i];

	stream << numShortControlPoints;
	for (uint32_t i = 0; i < numShortControlPoints; i++)
		stream << shortControlPoints[i];
}


void NiBSplineBasisData::Get(NiStream& stream) {
	NiObject::Get(stream);

	stream >> numControlPoints;
}

void NiBSplineBasisData::Put(NiStream& stream) {
	NiObject::Put(stream);

	stream << numControlPoints;
}


void NiTimeController::Get(NiStream& stream) {
	NiObject::Get(stream);

	nextControllerRef.Get(stream);
	stream >> flags;
	stream >> frequency;
	stream >> phase;
	stream >> startTime;
	stream >> stopTime;
	targetRef.Get(stream);
}

void NiTimeController::Put(NiStream& stream) {
	NiObject::Put(stream);

	nextControllerRef.Put(stream);
	stream << flags;
	stream << frequency;
	stream << phase;
	stream << startTime;
	stream << stopTime;
	targetRef.Put(stream);
}

void NiTimeController::GetChildRefs(std::set<Ref*>& refs) {
	NiObject::GetChildRefs(refs);

	refs.insert(&nextControllerRef);
}

void NiTimeController::GetChildIndices(std::vector<int>& indices) {
	NiObject::GetChildIndices(indices);

	indices.push_back(nextControllerRef.GetIndex());
}

void NiTimeController::GetPtrs(std::set<Ref*>& ptrs) {
	NiObject::GetPtrs(ptrs);

	ptrs.insert(&targetRef);
}


void NiLookAtController::Get(NiStream& stream) {
	NiTimeController::Get(stream);

	stream >> unkShort1;
	lookAtNodePtr.Get(stream);
}

void NiLookAtController::Put(NiStream& stream) {
	NiTimeController::Put(stream);

	stream << unkShort1;
	lookAtNodePtr.Put(stream);
}

void NiLookAtController::GetPtrs(std::set<Ref*>& ptrs) {
	NiTimeController::GetPtrs(ptrs);

	ptrs.insert(&lookAtNodePtr);
}


void NiPathController::Get(NiStream& stream) {
	NiTimeController::Get(stream);

	stream >> unkShort1;
	stream >> unkInt1;
	stream >> unkFloat1;
	stream >> unkFloat2;
	stream >> unkShort2;
	posDataRef.Get(stream);
	floatDataRef.Get(stream);
}

void NiPathController::Put(NiStream& stream) {
	NiTimeController::Put(stream);

	stream << unkShort1;
	stream << unkInt1;
	stream << unkFloat1;
	stream << unkFloat2;
	stream << unkShort2;
	posDataRef.Put(stream);
	floatDataRef.Put(stream);
}

void NiPathController::GetChildRefs(std::set<Ref*>& refs) {
	NiTimeController::GetChildRefs(refs);

	refs.insert(&posDataRef);
	refs.insert(&floatDataRef);
}

void NiPathController::GetChildIndices(std::vector<int>& indices) {
	NiTimeController::GetChildIndices(indices);

	indices.push_back(posDataRef.GetIndex());
	indices.push_back(floatDataRef.GetIndex());
}


void NiUVData::Get(NiStream& stream) {
	NiObject::Get(stream);

	uTrans.Get(stream);
	vTrans.Get(stream);
	uScale.Get(stream);
	vScale.Get(stream);
}

void NiUVData::Put(NiStream& stream) {
	NiObject::Put(stream);

	uTrans.Put(stream);
	vTrans.Put(stream);
	uScale.Put(stream);
	vScale.Put(stream);
}


void NiUVController::Get(NiStream& stream) {
	NiTimeController::Get(stream);

	stream >> textureSet;
	dataRef.Get(stream);
}

void NiUVController::Put(NiStream& stream) {
	NiTimeController::Put(stream);

	stream << textureSet;
	dataRef.Put(stream);
}

void NiUVController::GetChildRefs(std::set<Ref*>& refs) {
	NiTimeController::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void NiUVController::GetChildIndices(std::vector<int>& indices) {
	NiTimeController::GetChildIndices(indices);

	indices.push_back(dataRef.GetIndex());
}


void BSFrustumFOVController::Get(NiStream& stream) {
	NiTimeController::Get(stream);

	interpolatorRef.Get(stream);
}

void BSFrustumFOVController::Put(NiStream& stream) {
	NiTimeController::Put(stream);

	interpolatorRef.Put(stream);
}

void BSFrustumFOVController::GetChildRefs(std::set<Ref*>& refs) {
	NiTimeController::GetChildRefs(refs);

	refs.insert(&interpolatorRef);
}

void BSFrustumFOVController::GetChildIndices(std::vector<int>& indices) {
	NiTimeController::GetChildIndices(indices);

	indices.push_back(interpolatorRef.GetIndex());
}


void BSLagBoneController::Get(NiStream& stream) {
	NiTimeController::Get(stream);

	stream >> linearVelocity;
	stream >> linearRotation;
	stream >> maxDistance;
}

void BSLagBoneController::Put(NiStream& stream) {
	NiTimeController::Put(stream);

	stream << linearVelocity;
	stream << linearRotation;
	stream << maxDistance;
}


void BSProceduralLightningController::Get(NiStream& stream) {
	NiTimeController::Get(stream);

	generationInterpRef.Get(stream);
	mutationInterpRef.Get(stream);
	subdivisionInterpRef.Get(stream);
	numBranchesInterpRef.Get(stream);
	numBranchesVarInterpRef.Get(stream);
	lengthInterpRef.Get(stream);
	lengthVarInterpRef.Get(stream);
	widthInterpRef.Get(stream);
	arcOffsetInterpRef.Get(stream);

	stream >> subdivisions;
	stream >> numBranches;
	stream >> numBranchesPerVariation;

	stream >> length;
	stream >> lengthVariation;
	stream >> width;
	stream >> childWidthMult;
	stream >> arcOffset;

	stream >> fadeMainBolt;
	stream >> fadeChildBolts;
	stream >> animateArcOffset;

	shaderPropertyRef.Get(stream);
}

void BSProceduralLightningController::Put(NiStream& stream) {
	NiTimeController::Put(stream);

	generationInterpRef.Put(stream);
	mutationInterpRef.Put(stream);
	subdivisionInterpRef.Put(stream);
	numBranchesInterpRef.Put(stream);
	numBranchesVarInterpRef.Put(stream);
	lengthInterpRef.Put(stream);
	lengthVarInterpRef.Put(stream);
	widthInterpRef.Put(stream);
	arcOffsetInterpRef.Put(stream);

	stream << subdivisions;
	stream << numBranches;
	stream << numBranchesPerVariation;

	stream << length;
	stream << lengthVariation;
	stream << width;
	stream << childWidthMult;
	stream << arcOffset;

	stream << fadeMainBolt;
	stream << fadeChildBolts;
	stream << animateArcOffset;

	shaderPropertyRef.Put(stream);
}

void BSProceduralLightningController::GetChildRefs(std::set<Ref*>& refs) {
	NiTimeController::GetChildRefs(refs);

	refs.insert(&generationInterpRef);
	refs.insert(&mutationInterpRef);
	refs.insert(&subdivisionInterpRef);
	refs.insert(&numBranchesInterpRef);
	refs.insert(&numBranchesVarInterpRef);
	refs.insert(&lengthInterpRef);
	refs.insert(&lengthVarInterpRef);
	refs.insert(&widthInterpRef);
	refs.insert(&arcOffsetInterpRef);
	refs.insert(&shaderPropertyRef);
}

void BSProceduralLightningController::GetChildIndices(std::vector<int>& indices) {
	NiTimeController::GetChildIndices(indices);

	indices.push_back(generationInterpRef.GetIndex());
	indices.push_back(mutationInterpRef.GetIndex());
	indices.push_back(subdivisionInterpRef.GetIndex());
	indices.push_back(numBranchesInterpRef.GetIndex());
	indices.push_back(numBranchesVarInterpRef.GetIndex());
	indices.push_back(lengthInterpRef.GetIndex());
	indices.push_back(lengthVarInterpRef.GetIndex());
	indices.push_back(widthInterpRef.GetIndex());
	indices.push_back(arcOffsetInterpRef.GetIndex());
	indices.push_back(shaderPropertyRef.GetIndex());
}


void NiBoneLODController::Get(NiStream& stream) {
	NiTimeController::Get(stream);

	stream >> lod;
	stream >> numLODs;

	stream >> boneArraysSize;
	boneArrays.resize(boneArraysSize);
	for (uint32_t i = 0; i < boneArraysSize; i++)
		boneArrays[i].Get(stream);
}

void NiBoneLODController::Put(NiStream& stream) {
	NiTimeController::Put(stream);

	stream << lod;
	stream << numLODs;

	stream << boneArraysSize;
	for (uint32_t i = 0; i < boneArraysSize; i++)
		boneArrays[i].Put(stream);
}

void NiBoneLODController::GetPtrs(std::set<Ref*>& ptrs) {
	NiTimeController::GetPtrs(ptrs);

	for (uint32_t i = 0; i < boneArraysSize; i++)
		boneArrays[i].GetIndexPtrs(ptrs);
}


void NiMorphData::Get(NiStream& stream) {
	NiObject::Get(stream);

	stream >> numMorphs;
	stream >> numVertices;
	stream >> relativeTargets;

	morphs.resize(numMorphs);
	for (uint32_t i = 0; i < numMorphs; i++)
		morphs[i].Get(stream, numVertices);
}

void NiMorphData::Put(NiStream& stream) {
	NiObject::Put(stream);

	stream << numMorphs;
	stream << numVertices;
	stream << relativeTargets;

	for (uint32_t i = 0; i < numMorphs; i++)
		morphs[i].Put(stream, numVertices);
}

void NiMorphData::GetStringRefs(std::vector<StringRef*>& refs) {
	NiObject::GetStringRefs(refs);

	for (auto& m : morphs)
		m.GetStringRefs(refs);
}


void NiGeomMorpherController::Get(NiStream& stream) {
	NiInterpController::Get(stream);

	stream >> extraFlags;
	dataRef.Get(stream);
	stream >> alwaysUpdate;

	stream >> numTargets;
	interpWeights.resize(numTargets);
	for (uint32_t i = 0; i < numTargets; i++)
		interpWeights[i].Get(stream);
}

void NiGeomMorpherController::Put(NiStream& stream) {
	NiInterpController::Put(stream);

	stream << extraFlags;
	dataRef.Put(stream);
	stream << alwaysUpdate;

	stream << numTargets;
	for (uint32_t i = 0; i < numTargets; i++)
		interpWeights[i].Put(stream);
}

void NiGeomMorpherController::GetChildRefs(std::set<Ref*>& refs) {
	NiInterpController::GetChildRefs(refs);

	refs.insert(&dataRef);

	for (auto& m : interpWeights)
		m.GetChildRefs(refs);
}

void NiGeomMorpherController::GetChildIndices(std::vector<int>& indices) {
	NiInterpController::GetChildIndices(indices);

	indices.push_back(dataRef.GetIndex());

	for (auto& m : interpWeights)
		m.GetChildIndices(indices);
}


void NiSingleInterpController::Get(NiStream& stream) {
	NiInterpController::Get(stream);

	if (stream.GetVersion().File() >= V10_1_0_104)
		interpolatorRef.Get(stream);
}

void NiSingleInterpController::Put(NiStream& stream) {
	NiInterpController::Put(stream);

	if (stream.GetVersion().File() >= V10_1_0_104)
		interpolatorRef.Put(stream);
}

void NiSingleInterpController::GetChildRefs(std::set<Ref*>& refs) {
	NiInterpController::GetChildRefs(refs);

	refs.insert(&interpolatorRef);
}

void NiSingleInterpController::GetChildIndices(std::vector<int>& indices) {
	NiInterpController::GetChildIndices(indices);

	indices.push_back(interpolatorRef.GetIndex());
}


void NiRollController::Get(NiStream& stream) {
	NiSingleInterpController::Get(stream);

	dataRef.Get(stream);
}

void NiRollController::Put(NiStream& stream) {
	NiSingleInterpController::Put(stream);

	dataRef.Put(stream);
}

void NiRollController::GetChildRefs(std::set<Ref*>& refs) {
	NiSingleInterpController::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void NiRollController::GetChildIndices(std::vector<int>& indices) {
	NiSingleInterpController::GetChildIndices(indices);

	indices.push_back(dataRef.GetIndex());
}


void NiPoint3InterpController::Get(NiStream& stream) {
	NiSingleInterpController::Get(stream);

	stream >> targetColor;
}

void NiPoint3InterpController::Put(NiStream& stream) {
	NiSingleInterpController::Put(stream);

	stream << targetColor;
}


void NiFloatExtraDataController::Get(NiStream& stream) {
	NiExtraDataController::Get(stream);

	extraData.Get(stream);
}

void NiFloatExtraDataController::Put(NiStream& stream) {
	NiExtraDataController::Put(stream);

	extraData.Put(stream);
}

void NiFloatExtraDataController::GetStringRefs(std::vector<StringRef*>& refs) {
	NiExtraDataController::GetStringRefs(refs);

	refs.emplace_back(&extraData);
}


void NiVisData::Get(NiStream& stream) {
	NiObject::Get(stream);

	stream >> numKeys;
	keys.resize(numKeys);
	for (uint32_t i = 0; i < numKeys; i++) {
		stream >> keys[i].time;
		stream >> keys[i].value;
	}
}

void NiVisData::Put(NiStream& stream) {
	NiObject::Put(stream);

	stream << numKeys;
	for (uint32_t i = 0; i < numKeys; i++) {
		stream << keys[i].time;
		stream << keys[i].value;
	}
}


void NiFlipController::Get(NiStream& stream) {
	NiFloatInterpController::Get(stream);

	stream >> textureSlot;
	sourceRefs.Get(stream);
}

void NiFlipController::Put(NiStream& stream) {
	NiFloatInterpController::Put(stream);

	stream << textureSlot;
	sourceRefs.Put(stream);
}

void NiFlipController::GetChildRefs(std::set<Ref*>& refs) {
	NiFloatInterpController::GetChildRefs(refs);

	sourceRefs.GetIndexPtrs(refs);
}

void NiFlipController::GetChildIndices(std::vector<int>& indices) {
	NiFloatInterpController::GetChildIndices(indices);

	sourceRefs.GetIndices(indices);
}

BlockRefArray<NiSourceTexture>& NiFlipController::GetSources() {
	return sourceRefs;
}


void NiTextureTransformController::Get(NiStream& stream) {
	NiFloatInterpController::Get(stream);

	stream >> unkByte1;
	stream >> textureSlot;
	stream >> operation;
}

void NiTextureTransformController::Put(NiStream& stream) {
	NiFloatInterpController::Put(stream);

	stream << unkByte1;
	stream << textureSlot;
	stream << operation;
}


void BSLightingShaderPropertyColorController::Get(NiStream& stream) {
	NiFloatInterpController::Get(stream);

	stream >> typeOfControlledColor;
}

void BSLightingShaderPropertyColorController::Put(NiStream& stream) {
	NiFloatInterpController::Put(stream);

	stream << typeOfControlledColor;
}


void BSLightingShaderPropertyFloatController::Get(NiStream& stream) {
	NiFloatInterpController::Get(stream);

	stream >> typeOfControlledVariable;
}

void BSLightingShaderPropertyFloatController::Put(NiStream& stream) {
	NiFloatInterpController::Put(stream);

	stream << typeOfControlledVariable;
}


void BSLightingShaderPropertyUShortController::Get(NiStream& stream) {
	NiFloatInterpController::Get(stream);

	stream >> typeOfControlledVariable;
}

void BSLightingShaderPropertyUShortController::Put(NiStream& stream) {
	NiFloatInterpController::Put(stream);

	stream << typeOfControlledVariable;
}


void BSEffectShaderPropertyColorController::Get(NiStream& stream) {
	NiFloatInterpController::Get(stream);

	stream >> typeOfControlledColor;
}

void BSEffectShaderPropertyColorController::Put(NiStream& stream) {
	NiFloatInterpController::Put(stream);

	stream << typeOfControlledColor;
}


void BSEffectShaderPropertyFloatController::Get(NiStream& stream) {
	NiFloatInterpController::Get(stream);

	stream >> typeOfControlledVariable;
}

void BSEffectShaderPropertyFloatController::Put(NiStream& stream) {
	NiFloatInterpController::Put(stream);

	stream << typeOfControlledVariable;
}


void NiMultiTargetTransformController::Get(NiStream& stream) {
	NiInterpController::Get(stream);

	targetRefs.Get(stream);
}

void NiMultiTargetTransformController::Put(NiStream& stream) {
	NiInterpController::Put(stream);

	targetRefs.SetKeepEmptyRefs();
	targetRefs.Put(stream);
}

void NiMultiTargetTransformController::GetPtrs(std::set<Ref*>& ptrs) {
	NiInterpController::GetPtrs(ptrs);

	targetRefs.GetIndexPtrs(ptrs);
}

BlockRefShortArray<NiAVObject>& NiMultiTargetTransformController::GetTargets() {
	return targetRefs;
}


void NiPSysModifierCtlr::Get(NiStream& stream) {
	NiSingleInterpController::Get(stream);

	modifierName.Get(stream);
}

void NiPSysModifierCtlr::Put(NiStream& stream) {
	NiSingleInterpController::Put(stream);

	modifierName.Put(stream);
}

void NiPSysModifierCtlr::GetStringRefs(std::vector<StringRef*>& refs) {
	NiSingleInterpController::GetStringRefs(refs);

	refs.emplace_back(&modifierName);
}


void NiPSysEmitterCtlr::Get(NiStream& stream) {
	NiPSysModifierCtlr::Get(stream);

	visInterpolatorRef.Get(stream);
}

void NiPSysEmitterCtlr::Put(NiStream& stream) {
	NiPSysModifierCtlr::Put(stream);

	visInterpolatorRef.Put(stream);
}

void NiPSysEmitterCtlr::GetChildRefs(std::set<Ref*>& refs) {
	NiPSysModifierCtlr::GetChildRefs(refs);

	refs.insert(&visInterpolatorRef);
}

void NiPSysEmitterCtlr::GetChildIndices(std::vector<int>& indices) {
	NiPSysModifierCtlr::GetChildIndices(indices);

	indices.push_back(visInterpolatorRef.GetIndex());
}


void BSPSysMultiTargetEmitterCtlr::Get(NiStream& stream) {
	NiPSysEmitterCtlr::Get(stream);

	stream >> maxEmitters;
	masterParticleSystemRef.Get(stream);
}

void BSPSysMultiTargetEmitterCtlr::Put(NiStream& stream) {
	NiPSysEmitterCtlr::Put(stream);

	stream << maxEmitters;
	masterParticleSystemRef.Put(stream);
}

void BSPSysMultiTargetEmitterCtlr::GetPtrs(std::set<Ref*>& ptrs) {
	NiPSysEmitterCtlr::GetPtrs(ptrs);

	ptrs.insert(&masterParticleSystemRef);
}


void NiBSplineInterpolator::Get(NiStream& stream) {
	NiInterpolator::Get(stream);

	stream >> startTime;
	stream >> stopTime;
	splineDataRef.Get(stream);
	basisDataRef.Get(stream);
}

void NiBSplineInterpolator::Put(NiStream& stream) {
	NiInterpolator::Put(stream);

	stream << startTime;
	stream << stopTime;
	splineDataRef.Put(stream);
	basisDataRef.Put(stream);
}

void NiBSplineInterpolator::GetChildRefs(std::set<Ref*>& refs) {
	NiInterpolator::GetChildRefs(refs);

	refs.insert(&splineDataRef);
	refs.insert(&basisDataRef);
}

void NiBSplineInterpolator::GetChildIndices(std::vector<int>& indices) {
	NiInterpolator::GetChildIndices(indices);

	indices.push_back(splineDataRef.GetIndex());
	indices.push_back(basisDataRef.GetIndex());
}


void NiBSplineCompFloatInterpolator::Get(NiStream& stream) {
	NiBSplineFloatInterpolator::Get(stream);

	stream >> base;
	stream >> offset;
	stream >> bias;
	stream >> multiplier;
}

void NiBSplineCompFloatInterpolator::Put(NiStream& stream) {
	NiBSplineFloatInterpolator::Put(stream);

	stream << base;
	stream << offset;
	stream << bias;
	stream << multiplier;
}


void NiBSplinePoint3Interpolator::Get(NiStream& stream) {
	NiBSplineInterpolator::Get(stream);

	stream >> unkFloat1;
	stream >> unkFloat2;
	stream >> unkFloat3;
	stream >> unkFloat4;
	stream >> unkFloat5;
	stream >> unkFloat6;
}

void NiBSplinePoint3Interpolator::Put(NiStream& stream) {
	NiBSplineInterpolator::Put(stream);

	stream << unkFloat1;
	stream << unkFloat2;
	stream << unkFloat3;
	stream << unkFloat4;
	stream << unkFloat5;
	stream << unkFloat6;
}


void NiBSplineTransformInterpolator::Get(NiStream& stream) {
	NiBSplineInterpolator::Get(stream);

	stream >> translation;
	stream >> rotation;
	stream >> scale;

	stream >> translationOffset;
	stream >> rotationOffset;
	stream >> scaleOffset;
}

void NiBSplineTransformInterpolator::Put(NiStream& stream) {
	NiBSplineInterpolator::Put(stream);

	stream << translation;
	stream << rotation;
	stream << scale;

	stream << translationOffset;
	stream << rotationOffset;
	stream << scaleOffset;
}


void NiBSplineCompTransformInterpolator::Get(NiStream& stream) {
	NiBSplineTransformInterpolator::Get(stream);

	stream >> translationBias;
	stream >> translationMultiplier;
	stream >> rotationBias;
	stream >> rotationMultiplier;
	stream >> scaleBias;
	stream >> scaleMultiplier;
}

void NiBSplineCompTransformInterpolator::Put(NiStream& stream) {
	NiBSplineTransformInterpolator::Put(stream);

	stream << translationBias;
	stream << translationMultiplier;
	stream << rotationBias;
	stream << rotationMultiplier;
	stream << scaleBias;
	stream << scaleMultiplier;
}


void NiBlendInterpolator::Get(NiStream& stream) {
	NiInterpolator::Get(stream);

	stream >> flags;
	stream >> unkInt;
}

void NiBlendInterpolator::Put(NiStream& stream) {
	NiInterpolator::Put(stream);

	stream << flags;
	stream << unkInt;
}


void NiBlendBoolInterpolator::Get(NiStream& stream) {
	NiBlendInterpolator::Get(stream);

	stream >> value;
}

void NiBlendBoolInterpolator::Put(NiStream& stream) {
	NiBlendInterpolator::Put(stream);

	stream << value;
}


void NiBlendFloatInterpolator::Get(NiStream& stream) {
	NiBlendInterpolator::Get(stream);

	stream >> value;
}

void NiBlendFloatInterpolator::Put(NiStream& stream) {
	NiBlendInterpolator::Put(stream);

	stream << value;
}


void NiBlendPoint3Interpolator::Get(NiStream& stream) {
	NiBlendInterpolator::Get(stream);

	stream >> point;
}

void NiBlendPoint3Interpolator::Put(NiStream& stream) {
	NiBlendInterpolator::Put(stream);

	stream << point;
}


void NiBoolInterpolator::Get(NiStream& stream) {
	NiKeyBasedInterpolator::Get(stream);

	stream >> boolValue;
	dataRef.Get(stream);
}

void NiBoolInterpolator::Put(NiStream& stream) {
	NiKeyBasedInterpolator::Put(stream);

	stream << boolValue;
	dataRef.Put(stream);
}

void NiBoolInterpolator::GetChildRefs(std::set<Ref*>& refs) {
	NiKeyBasedInterpolator::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void NiBoolInterpolator::GetChildIndices(std::vector<int>& indices) {
	NiKeyBasedInterpolator::GetChildIndices(indices);

	indices.push_back(dataRef.GetIndex());
}


void NiFloatInterpolator::Get(NiStream& stream) {
	NiKeyBasedInterpolator::Get(stream);

	stream >> floatValue;
	dataRef.Get(stream);
}

void NiFloatInterpolator::Put(NiStream& stream) {
	NiKeyBasedInterpolator::Put(stream);

	stream << floatValue;
	dataRef.Put(stream);
}

void NiFloatInterpolator::GetChildRefs(std::set<Ref*>& refs) {
	NiKeyBasedInterpolator::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void NiFloatInterpolator::GetChildIndices(std::vector<int>& indices) {
	NiKeyBasedInterpolator::GetChildIndices(indices);

	indices.push_back(dataRef.GetIndex());
}


void NiTransformInterpolator::Get(NiStream& stream) {
	NiKeyBasedInterpolator::Get(stream);

	stream >> translation;
	stream >> rotation;
	stream >> scale;
	dataRef.Get(stream);
}

void NiTransformInterpolator::Put(NiStream& stream) {
	NiKeyBasedInterpolator::Put(stream);

	stream << translation;
	stream << rotation;
	stream << scale;
	dataRef.Put(stream);
}

void NiTransformInterpolator::GetChildRefs(std::set<Ref*>& refs) {
	NiKeyBasedInterpolator::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void NiTransformInterpolator::GetChildIndices(std::vector<int>& indices) {
	NiKeyBasedInterpolator::GetChildIndices(indices);

	indices.push_back(dataRef.GetIndex());
}


void NiPoint3Interpolator::Get(NiStream& stream) {
	NiKeyBasedInterpolator::Get(stream);

	stream >> point3Value;
	dataRef.Get(stream);
}

void NiPoint3Interpolator::Put(NiStream& stream) {
	NiKeyBasedInterpolator::Put(stream);

	stream << point3Value;
	dataRef.Put(stream);
}

void NiPoint3Interpolator::GetChildRefs(std::set<Ref*>& refs) {
	NiKeyBasedInterpolator::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void NiPoint3Interpolator::GetChildIndices(std::vector<int>& indices) {
	NiKeyBasedInterpolator::GetChildIndices(indices);

	indices.push_back(dataRef.GetIndex());
}


void NiPathInterpolator::Get(NiStream& stream) {
	NiKeyBasedInterpolator::Get(stream);

	stream >> flags;
	stream >> bankDir;
	stream >> maxBankAngle;
	stream >> smoothing;
	stream >> followAxis;
	pathDataRef.Get(stream);
	percentDataRef.Get(stream);
}

void NiPathInterpolator::Put(NiStream& stream) {
	NiKeyBasedInterpolator::Put(stream);

	stream << flags;
	stream << bankDir;
	stream << maxBankAngle;
	stream << smoothing;
	stream << followAxis;
	pathDataRef.Put(stream);
	percentDataRef.Put(stream);
}

void NiPathInterpolator::GetChildRefs(std::set<Ref*>& refs) {
	NiKeyBasedInterpolator::GetChildRefs(refs);

	refs.insert(&pathDataRef);
	refs.insert(&percentDataRef);
}

void NiPathInterpolator::GetChildIndices(std::vector<int>& indices) {
	NiKeyBasedInterpolator::GetChildIndices(indices);

	indices.push_back(pathDataRef.GetIndex());
	indices.push_back(percentDataRef.GetIndex());
}


void NiLookAtInterpolator::Get(NiStream& stream) {
	NiInterpolator::Get(stream);

	stream >> flags;
	lookAtRef.Get(stream);
	lookAtName.Get(stream);
	stream >> transform;
	translateInterpRef.Get(stream);
	rollInterpRef.Get(stream);
	scaleInterpRef.Get(stream);
}

void NiLookAtInterpolator::Put(NiStream& stream) {
	NiInterpolator::Put(stream);

	stream << flags;
	lookAtRef.Put(stream);
	lookAtName.Put(stream);
	stream << transform;
	translateInterpRef.Put(stream);
	rollInterpRef.Put(stream);
	scaleInterpRef.Put(stream);
}

void NiLookAtInterpolator::GetStringRefs(std::vector<StringRef*>& refs) {
	NiInterpolator::GetStringRefs(refs);

	refs.emplace_back(&lookAtName);
}

void NiLookAtInterpolator::GetChildRefs(std::set<Ref*>& refs) {
	NiInterpolator::GetChildRefs(refs);

	refs.insert(&translateInterpRef);
	refs.insert(&rollInterpRef);
	refs.insert(&scaleInterpRef);
}

void NiLookAtInterpolator::GetChildIndices(std::vector<int>& indices) {
	NiInterpolator::GetChildIndices(indices);

	indices.push_back(translateInterpRef.GetIndex());
	indices.push_back(rollInterpRef.GetIndex());
	indices.push_back(scaleInterpRef.GetIndex());
}

void NiLookAtInterpolator::GetPtrs(std::set<Ref*>& ptrs) {
	NiInterpolator::GetPtrs(ptrs);

	ptrs.insert(&lookAtRef);
}


void BSTreadTransfInterpolator::Get(NiStream& stream) {
	NiInterpolator::Get(stream);

	stream >> numTreadTransforms;
	treadTransforms.resize(numTreadTransforms);
	for (uint32_t i = 0; i < numTreadTransforms; i++)
		treadTransforms[i].Get(stream);

	dataRef.Get(stream);
}

void BSTreadTransfInterpolator::Put(NiStream& stream) {
	NiInterpolator::Put(stream);

	stream << numTreadTransforms;
	for (uint32_t i = 0; i < numTreadTransforms; i++)
		treadTransforms[i].Put(stream);

	dataRef.Put(stream);
}

void BSTreadTransfInterpolator::GetStringRefs(std::vector<StringRef*>& refs) {
	NiInterpolator::GetStringRefs(refs);

	for (uint32_t i = 0; i < numTreadTransforms; i++)
		treadTransforms[i].GetStringRefs(refs);
}

void BSTreadTransfInterpolator::GetChildRefs(std::set<Ref*>& refs) {
	NiInterpolator::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void BSTreadTransfInterpolator::GetChildIndices(std::vector<int>& indices) {
	NiInterpolator::GetChildIndices(indices);

	indices.push_back(dataRef.GetIndex());
}


void NiStringPalette::Get(NiStream& stream) {
	NiObject::Get(stream);

	palette.Get(stream, 4);
	stream >> length;
}

void NiStringPalette::Put(NiStream& stream) {
	NiObject::Put(stream);

	length = palette.GetLength();

	palette.Put(stream, 4, false);
	stream << length;
}


void NiSequence::Get(NiStream& stream) {
	NiObject::Get(stream);

	name.Get(stream);

	stream >> numControlledBlocks;
	stream >> arrayGrowBy;

	controlledBlocks.resize(numControlledBlocks);
	for (uint32_t i = 0; i < numControlledBlocks; i++) {
		controlledBlocks[i].interpolatorRef.Get(stream);
		controlledBlocks[i].controllerRef.Get(stream);
		stream >> controlledBlocks[i].priority;

		controlledBlocks[i].nodeName.Get(stream);
		controlledBlocks[i].propType.Get(stream);
		controlledBlocks[i].ctrlType.Get(stream);
		controlledBlocks[i].ctrlID.Get(stream);
		controlledBlocks[i].interpID.Get(stream);
	}
}

void NiSequence::Put(NiStream& stream) {
	NiObject::Put(stream);

	name.Put(stream);

	stream << numControlledBlocks;
	stream << arrayGrowBy;

	for (uint32_t i = 0; i < numControlledBlocks; i++) {
		controlledBlocks[i].interpolatorRef.Put(stream);
		controlledBlocks[i].controllerRef.Put(stream);
		stream << controlledBlocks[i].priority;
		controlledBlocks[i].nodeName.Put(stream);
		controlledBlocks[i].propType.Put(stream);
		controlledBlocks[i].ctrlType.Put(stream);
		controlledBlocks[i].ctrlID.Put(stream);
		controlledBlocks[i].interpID.Put(stream);
	}
}

void NiSequence::GetStringRefs(std::vector<StringRef*>& refs) {
	NiObject::GetStringRefs(refs);

	refs.emplace_back(&name);

	for (uint32_t i = 0; i < numControlledBlocks; i++) {
		refs.emplace_back(&controlledBlocks[i].nodeName);
		refs.emplace_back(&controlledBlocks[i].propType);
		refs.emplace_back(&controlledBlocks[i].ctrlType);
		refs.emplace_back(&controlledBlocks[i].ctrlID);
		refs.emplace_back(&controlledBlocks[i].interpID);
	}
}

void NiSequence::GetChildRefs(std::set<Ref*>& refs) {
	NiObject::GetChildRefs(refs);

	for (uint32_t i = 0; i < numControlledBlocks; i++) {
		refs.insert(&controlledBlocks[i].interpolatorRef);
		refs.insert(&controlledBlocks[i].controllerRef);
	}
}

void NiSequence::GetChildIndices(std::vector<int>& indices) {
	NiObject::GetChildIndices(indices);

	for (uint32_t i = 0; i < numControlledBlocks; i++) {
		indices.push_back(controlledBlocks[i].interpolatorRef.GetIndex());
		indices.push_back(controlledBlocks[i].controllerRef.GetIndex());
	}
}


void BSAnimNote::Get(NiStream& stream) {
	NiObject::Get(stream);

	stream >> type;
	stream >> time;

	if (type == ANT_GRABIK)
		stream >> arm;

	if (type != ANT_INVALID) {
		stream >> gain;
		stream >> state;
	}
}

void BSAnimNote::Put(NiStream& stream) {
	NiObject::Put(stream);

	stream << type;
	stream << time;

	if (type == ANT_GRABIK)
		stream << arm;

	if (type != ANT_INVALID) {
		stream << gain;
		stream << state;
	}
}


void BSAnimNotes::Get(NiStream& stream) {
	NiObject::Get(stream);

	animNoteRefs.Get(stream);
}

void BSAnimNotes::Put(NiStream& stream) {
	NiObject::Put(stream);

	animNoteRefs.Put(stream);
}

void BSAnimNotes::GetChildRefs(std::set<Ref*>& refs) {
	NiObject::GetChildRefs(refs);

	animNoteRefs.GetIndexPtrs(refs);
}

void BSAnimNotes::GetChildIndices(std::vector<int>& indices) {
	NiObject::GetChildIndices(indices);

	animNoteRefs.GetIndices(indices);
}

BlockRefShortArray<BSAnimNote>& BSAnimNotes::GetAnimNotes() {
	return animNoteRefs;
}


void NiControllerSequence::Get(NiStream& stream) {
	NiSequence::Get(stream);

	stream >> weight;
	textKeyRef.Get(stream);
	stream >> cycleType;
	stream >> frequency;
	stream >> startTime;
	stream >> stopTime;
	managerRef.Get(stream);
	accumRootName.Get(stream);

	if (stream.GetVersion().Stream() >= 24 && stream.GetVersion().Stream() <= 28)
		animNotesRef.Get(stream);
	else if (stream.GetVersion().Stream() > 28)
		animNotesRefs.Get(stream);
}

void NiControllerSequence::Put(NiStream& stream) {
	NiSequence::Put(stream);

	stream << weight;
	textKeyRef.Put(stream);
	stream << cycleType;
	stream << frequency;
	stream << startTime;
	stream << stopTime;
	managerRef.Put(stream);
	accumRootName.Put(stream);

	if (stream.GetVersion().Stream() >= 24 && stream.GetVersion().Stream() <= 28)
		animNotesRef.Put(stream);
	else if (stream.GetVersion().Stream() > 28)
		animNotesRefs.Put(stream);
}

void NiControllerSequence::GetStringRefs(std::vector<StringRef*>& refs) {
	NiSequence::GetStringRefs(refs);

	refs.emplace_back(&accumRootName);
}

void NiControllerSequence::GetChildRefs(std::set<Ref*>& refs) {
	NiSequence::GetChildRefs(refs);

	refs.insert(&textKeyRef);
	refs.insert(&animNotesRef);
	animNotesRefs.GetIndexPtrs(refs);
}

void NiControllerSequence::GetChildIndices(std::vector<int>& indices) {
	NiSequence::GetChildIndices(indices);

	indices.push_back(textKeyRef.GetIndex());
	indices.push_back(animNotesRef.GetIndex());
	animNotesRefs.GetIndices(indices);
}

void NiControllerSequence::GetPtrs(std::set<Ref*>& ptrs) {
	NiSequence::GetPtrs(ptrs);

	ptrs.insert(&managerRef);
}

int NiControllerSequence::GetAnimNotesRef() {
	return animNotesRef.GetIndex();
}

void NiControllerSequence::SetAnimNotesRef(int notesRef) {
	animNotesRef.SetIndex(notesRef);
}

BlockRefShortArray<BSAnimNotes>& NiControllerSequence::GetAnimNotes() {
	return animNotesRefs;
}


void NiControllerManager::Get(NiStream& stream) {
	NiTimeController::Get(stream);

	stream >> cumulative;

	controllerSequenceRefs.Get(stream);
	objectPaletteRef.Get(stream);
}

void NiControllerManager::Put(NiStream& stream) {
	NiTimeController::Put(stream);

	stream << cumulative;

	controllerSequenceRefs.Put(stream);
	objectPaletteRef.Put(stream);
}

void NiControllerManager::GetChildRefs(std::set<Ref*>& refs) {
	NiTimeController::GetChildRefs(refs);

	controllerSequenceRefs.GetIndexPtrs(refs);
	refs.insert(&objectPaletteRef);
}

void NiControllerManager::GetChildIndices(std::vector<int>& indices) {
	NiTimeController::GetChildIndices(indices);

	controllerSequenceRefs.GetIndices(indices);
	indices.push_back(objectPaletteRef.GetIndex());
}

BlockRefArray<NiControllerSequence>& NiControllerManager::GetControllerSequences() {
	return controllerSequenceRefs;
}

int NiControllerManager::GetObjectPaletteRef() {
	return objectPaletteRef.GetIndex();
}

void NiControllerManager::SetObjectPaletteRef(int paletteRef) {
	objectPaletteRef.SetIndex(paletteRef);
}
