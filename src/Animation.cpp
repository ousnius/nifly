/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#include "Animation.hpp"

using namespace nifly;

void NiKeyframeData::Sync(NiStreamReversible& stream) {
	stream.Sync(numRotationKeys);

	if (numRotationKeys > 0) {
		stream.Sync(rotationType);

		if (rotationType != XYZ_ROTATION_KEY) {
			quaternionKeys.resize(numRotationKeys);

			for (uint32_t i = 0; i < numRotationKeys; i++) {
				stream.Sync(quaternionKeys[i].time);
				stream.Sync(quaternionKeys[i].value);

				if (rotationType == TBC_KEY)
					stream.Sync(quaternionKeys[i].tbc);
			}
		}
		else {
			xRotations.Sync(stream);
			yRotations.Sync(stream);
			zRotations.Sync(stream);
		}
	}

	translations.Sync(stream);
	scales.Sync(stream);
}

std::vector<Key<Quaternion>> NiKeyframeData::GetQuaternionKeys() const {
	return quaternionKeys;
}

void NiKeyframeData::SetQuaternionKeys(const std::vector<Key<Quaternion>>& qk) {
	numRotationKeys = qk.size();
	quaternionKeys = qk;
}


void NiPosData::Sync(NiStreamReversible& stream) {
	data.Sync(stream);
}


void NiBoolData::Sync(NiStreamReversible& stream) {
	data.Sync(stream);
}


void NiFloatData::Sync(NiStreamReversible& stream) {
	data.Sync(stream);
}


void NiBSplineData::Sync(NiStreamReversible& stream) {
	floatControlPoints.Sync(stream);
	shortControlPoints.Sync(stream);
}


void NiBSplineBasisData::Sync(NiStreamReversible& stream) {
	stream.Sync(numControlPoints);
}


void NiTimeController::Sync(NiStreamReversible& stream) {
	nextControllerRef.Sync(stream);
	stream.Sync(flags);
	stream.Sync(frequency);
	stream.Sync(phase);
	stream.Sync(startTime);
	stream.Sync(stopTime);
	targetRef.Sync(stream);
}

void NiTimeController::GetChildRefs(std::set<NiRef*>& refs) {
	NiObject::GetChildRefs(refs);

	refs.insert(&nextControllerRef);
}

void NiTimeController::GetChildIndices(std::vector<int>& indices) {
	NiObject::GetChildIndices(indices);

	indices.push_back(nextControllerRef.index);
}

void NiTimeController::GetPtrs(std::set<NiPtr*>& ptrs) {
	NiObject::GetPtrs(ptrs);

	ptrs.insert(&targetRef);
}


void NiLookAtController::Sync(NiStreamReversible& stream) {
	stream.Sync(lookAtFlags);
	lookAtNodePtr.Sync(stream);
}

void NiLookAtController::GetPtrs(std::set<NiPtr*>& ptrs) {
	NiTimeController::GetPtrs(ptrs);

	ptrs.insert(&lookAtNodePtr);
}


void NiPathController::Sync(NiStreamReversible& stream) {
	stream.Sync(pathFlags);
	stream.Sync(bankDir);
	stream.Sync(maxBankAngle);
	stream.Sync(smoothing);
	stream.Sync(followAxis);
	pathDataRef.Sync(stream);
	percentDataRef.Sync(stream);
}

void NiPathController::GetChildRefs(std::set<NiRef*>& refs) {
	NiTimeController::GetChildRefs(refs);

	refs.insert(&pathDataRef);
	refs.insert(&percentDataRef);
}

void NiPathController::GetChildIndices(std::vector<int>& indices) {
	NiTimeController::GetChildIndices(indices);

	indices.push_back(pathDataRef.index);
	indices.push_back(percentDataRef.index);
}


void NiUVData::Sync(NiStreamReversible& stream) {
	uTrans.Sync(stream);
	vTrans.Sync(stream);
	uScale.Sync(stream);
	vScale.Sync(stream);
}


void NiUVController::Sync(NiStreamReversible& stream) {
	stream.Sync(textureSet);
	dataRef.Sync(stream);
}

void NiUVController::GetChildRefs(std::set<NiRef*>& refs) {
	NiTimeController::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void NiUVController::GetChildIndices(std::vector<int>& indices) {
	NiTimeController::GetChildIndices(indices);

	indices.push_back(dataRef.index);
}


void BSFrustumFOVController::Sync(NiStreamReversible& stream) {
	interpolatorRef.Sync(stream);
}

void BSFrustumFOVController::GetChildRefs(std::set<NiRef*>& refs) {
	NiTimeController::GetChildRefs(refs);

	refs.insert(&interpolatorRef);
}

void BSFrustumFOVController::GetChildIndices(std::vector<int>& indices) {
	NiTimeController::GetChildIndices(indices);

	indices.push_back(interpolatorRef.index);
}


void BSLagBoneController::Sync(NiStreamReversible& stream) {
	stream.Sync(linearVelocity);
	stream.Sync(linearRotation);
	stream.Sync(maxDistance);
}


void BSProceduralLightningController::Sync(NiStreamReversible& stream) {
	generationInterpRef.Sync(stream);
	mutationInterpRef.Sync(stream);
	subdivisionInterpRef.Sync(stream);
	numBranchesInterpRef.Sync(stream);
	numBranchesVarInterpRef.Sync(stream);
	lengthInterpRef.Sync(stream);
	lengthVarInterpRef.Sync(stream);
	widthInterpRef.Sync(stream);
	arcOffsetInterpRef.Sync(stream);

	stream.Sync(subdivisions);
	stream.Sync(numBranches);
	stream.Sync(numBranchesPerVariation);

	stream.Sync(length);
	stream.Sync(lengthVariation);
	stream.Sync(width);
	stream.Sync(childWidthMult);
	stream.Sync(arcOffset);

	stream.Sync(fadeMainBolt);
	stream.Sync(fadeChildBolts);
	stream.Sync(animateArcOffset);

	shaderPropertyRef.Sync(stream);
}

void BSProceduralLightningController::GetChildRefs(std::set<NiRef*>& refs) {
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

	indices.push_back(generationInterpRef.index);
	indices.push_back(mutationInterpRef.index);
	indices.push_back(subdivisionInterpRef.index);
	indices.push_back(numBranchesInterpRef.index);
	indices.push_back(numBranchesVarInterpRef.index);
	indices.push_back(lengthInterpRef.index);
	indices.push_back(lengthVarInterpRef.index);
	indices.push_back(widthInterpRef.index);
	indices.push_back(arcOffsetInterpRef.index);
	indices.push_back(shaderPropertyRef.index);
}


void NiBoneLODController::Sync(NiStreamReversible& stream) {
	stream.Sync(lod);
	stream.Sync(numLODs);

	stream.Sync(boneArraysSize);
	boneArrays.resize(boneArraysSize);
	for (uint32_t i = 0; i < boneArraysSize; i++)
		boneArrays[i].Sync(stream);
}

void NiBoneLODController::GetPtrs(std::set<NiPtr*>& ptrs) {
	NiTimeController::GetPtrs(ptrs);

	for (uint32_t i = 0; i < boneArraysSize; i++)
		boneArrays[i].GetIndexPtrs(ptrs);
}

std::vector<NiBlockPtrArray<NiNode>> NiBoneLODController::GetBoneArrays() const {
	return boneArrays;
}

void NiBoneLODController::SetBoneArrays(const std::vector<NiBlockPtrArray<NiNode>>& ba) {
	boneArraysSize = ba.size();
	boneArrays = ba;
}


void NiMorphData::Sync(NiStreamReversible& stream) {
	stream.Sync(numMorphs);
	stream.Sync(numVertices);
	stream.Sync(relativeTargets);

	morphs.resize(numMorphs);
	for (uint32_t i = 0; i < numMorphs; i++)
		morphs[i].Sync(stream, numVertices);
}

void NiMorphData::GetStringRefs(std::vector<NiStringRef*>& refs) {
	NiObject::GetStringRefs(refs);

	for (auto& m : morphs)
		m.GetStringRefs(refs);
}

std::vector<Morph> NiMorphData::GetMorphs() const {
	return morphs;
}

void NiMorphData::SetMorphs(const std::vector<Morph>& m) {
	numMorphs = m.size();
	morphs = m;
}


void NiGeomMorpherController::Sync(NiStreamReversible& stream) {
	stream.Sync(morpherFlags);
	dataRef.Sync(stream);
	stream.Sync(alwaysUpdate);

	stream.Sync(numTargets);
	interpWeights.resize(numTargets);
	for (uint32_t i = 0; i < numTargets; i++)
		interpWeights[i].Sync(stream);
}

void NiGeomMorpherController::GetChildRefs(std::set<NiRef*>& refs) {
	NiInterpController::GetChildRefs(refs);

	refs.insert(&dataRef);

	for (auto& m : interpWeights)
		m.GetChildRefs(refs);
}

void NiGeomMorpherController::GetChildIndices(std::vector<int>& indices) {
	NiInterpController::GetChildIndices(indices);

	indices.push_back(dataRef.index);

	for (auto& m : interpWeights)
		m.GetChildIndices(indices);
}

std::vector<MorphWeight> NiGeomMorpherController::GetInterpWeights() const {
	return interpWeights;
}

void NiGeomMorpherController::SetInterpWeights(const std::vector<MorphWeight>& m) {
	numTargets = m.size();
	interpWeights = m;
}


void NiSingleInterpController::Sync(NiStreamReversible& stream) {
	if (stream.GetVersion().File() >= V10_1_0_104)
		interpolatorRef.Sync(stream);
}

void NiSingleInterpController::GetChildRefs(std::set<NiRef*>& refs) {
	NiInterpController::GetChildRefs(refs);

	refs.insert(&interpolatorRef);
}

void NiSingleInterpController::GetChildIndices(std::vector<int>& indices) {
	NiInterpController::GetChildIndices(indices);

	indices.push_back(interpolatorRef.index);
}


void NiRollController::Sync(NiStreamReversible& stream) {
	dataRef.Sync(stream);
}

void NiRollController::GetChildRefs(std::set<NiRef*>& refs) {
	NiSingleInterpController::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void NiRollController::GetChildIndices(std::vector<int>& indices) {
	NiSingleInterpController::GetChildIndices(indices);

	indices.push_back(dataRef.index);
}


void NiPoint3InterpController::Sync(NiStreamReversible& stream) {
	stream.Sync(targetColor);
}


void NiFloatExtraDataController::Sync(NiStreamReversible& stream) {
	extraData.Sync(stream);
}

void NiFloatExtraDataController::GetStringRefs(std::vector<NiStringRef*>& refs) {
	NiExtraDataController::GetStringRefs(refs);

	refs.emplace_back(&extraData);
}


void NiVisData::Sync(NiStreamReversible& stream) {
	stream.Sync(numKeys);
	keys.resize(numKeys);
	for (uint32_t i = 0; i < numKeys; i++) {
		stream.Sync(keys[i].time);
		stream.Sync(keys[i].value);
	}
}

std::vector<Key<uint8_t>> NiVisData::GetKeys() const {
	return keys;
}

void NiVisData::SetKeys(const std::vector<Key<uint8_t>>& k) {
	numKeys = k.size();
	keys = k;
}


void NiFlipController::Sync(NiStreamReversible& stream) {
	stream.Sync(textureSlot);
	sourceRefs.Sync(stream);
}

void NiFlipController::GetChildRefs(std::set<NiRef*>& refs) {
	NiFloatInterpController::GetChildRefs(refs);

	sourceRefs.GetIndexPtrs(refs);
}

void NiFlipController::GetChildIndices(std::vector<int>& indices) {
	NiFloatInterpController::GetChildIndices(indices);

	sourceRefs.GetIndices(indices);
}


void NiTextureTransformController::Sync(NiStreamReversible& stream) {
	stream.Sync(shaderMap);
	stream.Sync(textureSlot);
	stream.Sync(operation);
}


void BSLightingShaderPropertyColorController::Sync(NiStreamReversible& stream) {
	stream.Sync(typeOfControlledColor);
}


void BSLightingShaderPropertyFloatController::Sync(NiStreamReversible& stream) {
	stream.Sync(typeOfControlledVariable);
}


void BSLightingShaderPropertyUShortController::Sync(NiStreamReversible& stream) {
	stream.Sync(typeOfControlledVariable);
}


void BSEffectShaderPropertyColorController::Sync(NiStreamReversible& stream) {
	stream.Sync(typeOfControlledColor);
}


void BSEffectShaderPropertyFloatController::Sync(NiStreamReversible& stream) {
	stream.Sync(typeOfControlledVariable);
}


void NiMultiTargetTransformController::Sync(NiStreamReversible& stream) {
	targetRefs.SetKeepEmptyRefs();
	targetRefs.Sync(stream);
}

void NiMultiTargetTransformController::GetPtrs(std::set<NiPtr*>& ptrs) {
	NiInterpController::GetPtrs(ptrs);

	targetRefs.GetIndexPtrs(ptrs);
}


void NiPSysModifierCtlr::Sync(NiStreamReversible& stream) {
	modifierName.Sync(stream);
}

void NiPSysModifierCtlr::GetStringRefs(std::vector<NiStringRef*>& refs) {
	NiSingleInterpController::GetStringRefs(refs);

	refs.emplace_back(&modifierName);
}


void NiPSysEmitterCtlr::Sync(NiStreamReversible& stream) {
	visInterpolatorRef.Sync(stream);
}

void NiPSysEmitterCtlr::GetChildRefs(std::set<NiRef*>& refs) {
	NiPSysModifierCtlr::GetChildRefs(refs);

	refs.insert(&visInterpolatorRef);
}

void NiPSysEmitterCtlr::GetChildIndices(std::vector<int>& indices) {
	NiPSysModifierCtlr::GetChildIndices(indices);

	indices.push_back(visInterpolatorRef.index);
}


void BSPSysMultiTargetEmitterCtlr::Sync(NiStreamReversible& stream) {
	stream.Sync(maxEmitters);
	masterParticleSystemRef.Sync(stream);
}

void BSPSysMultiTargetEmitterCtlr::GetPtrs(std::set<NiPtr*>& ptrs) {
	NiPSysEmitterCtlr::GetPtrs(ptrs);

	ptrs.insert(&masterParticleSystemRef);
}


void NiBSplineInterpolator::Sync(NiStreamReversible& stream) {
	stream.Sync(startTime);
	stream.Sync(stopTime);
	splineDataRef.Sync(stream);
	basisDataRef.Sync(stream);
}

void NiBSplineInterpolator::GetChildRefs(std::set<NiRef*>& refs) {
	NiInterpolator::GetChildRefs(refs);

	refs.insert(&splineDataRef);
	refs.insert(&basisDataRef);
}

void NiBSplineInterpolator::GetChildIndices(std::vector<int>& indices) {
	NiInterpolator::GetChildIndices(indices);

	indices.push_back(splineDataRef.index);
	indices.push_back(basisDataRef.index);
}


void NiBSplineCompFloatInterpolator::Sync(NiStreamReversible& stream) {
	stream.Sync(base);
	stream.Sync(offset);
	stream.Sync(bias);
	stream.Sync(multiplier);
}


void NiBSplinePoint3Interpolator::Sync(NiStreamReversible& stream) {
	stream.Sync(value);
	stream.Sync(handle);
}


void NiBSplineCompPoint3Interpolator::Sync(NiStreamReversible& stream) {
	stream.Sync(positionOffset);
	stream.Sync(positionHalfRange);
}


void NiBSplineTransformInterpolator::Sync(NiStreamReversible& stream) {
	stream.Sync(translation);
	stream.Sync(rotation);
	stream.Sync(scale);

	stream.Sync(translationOffset);
	stream.Sync(rotationOffset);
	stream.Sync(scaleOffset);
}


void NiBSplineCompTransformInterpolator::Sync(NiStreamReversible& stream) {
	stream.Sync(translationBias);
	stream.Sync(translationMultiplier);
	stream.Sync(rotationBias);
	stream.Sync(rotationMultiplier);
	stream.Sync(scaleBias);
	stream.Sync(scaleMultiplier);
}


void InterpBlendItem::Sync(NiStreamReversible& stream) {
	interpolatorRef.Sync(stream);
	stream.Sync(weight);
	stream.Sync(normalizedWeight);
	stream.Sync(priority);
	stream.Sync(easeSpinner);
}


void NiBlendInterpolator::Sync(NiStreamReversible& stream) {
	stream.Sync(flags);
	stream.Sync(arraySize);
	stream.Sync(weightThreshold);

	if ((flags & INTERP_BLEND_MANAGER_CONTROLLED) == 0) {
		stream.Sync(interpCount);
		stream.Sync(singleIndex);
		stream.Sync(highPriority);
		stream.Sync(nextHighPriority);
		stream.Sync(singleTime);
		stream.Sync(highWeightsSum);
		stream.Sync(nextHighWeightsSum);
		stream.Sync(highEaseSpinner);

		interpItems.resize(arraySize);
		for (auto& item : interpItems)
			item.Sync(stream);
	}
}


void NiBlendBoolInterpolator::Sync(NiStreamReversible& stream) {
	stream.Sync(value);
}


void NiBlendFloatInterpolator::Sync(NiStreamReversible& stream) {
	stream.Sync(value);
}


void NiBlendPoint3Interpolator::Sync(NiStreamReversible& stream) {
	stream.Sync(point);
}


void NiBoolInterpolator::Sync(NiStreamReversible& stream) {
	stream.Sync(boolValue);
	dataRef.Sync(stream);
}

void NiBoolInterpolator::GetChildRefs(std::set<NiRef*>& refs) {
	NiKeyBasedInterpolator::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void NiBoolInterpolator::GetChildIndices(std::vector<int>& indices) {
	NiKeyBasedInterpolator::GetChildIndices(indices);

	indices.push_back(dataRef.index);
}


void NiFloatInterpolator::Sync(NiStreamReversible& stream) {
	stream.Sync(floatValue);
	dataRef.Sync(stream);
}

void NiFloatInterpolator::GetChildRefs(std::set<NiRef*>& refs) {
	NiKeyBasedInterpolator::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void NiFloatInterpolator::GetChildIndices(std::vector<int>& indices) {
	NiKeyBasedInterpolator::GetChildIndices(indices);

	indices.push_back(dataRef.index);
}


void NiTransformInterpolator::Sync(NiStreamReversible& stream) {
	stream.Sync(translation);
	stream.Sync(rotation);
	stream.Sync(scale);
	dataRef.Sync(stream);
}

void NiTransformInterpolator::GetChildRefs(std::set<NiRef*>& refs) {
	NiKeyBasedInterpolator::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void NiTransformInterpolator::GetChildIndices(std::vector<int>& indices) {
	NiKeyBasedInterpolator::GetChildIndices(indices);

	indices.push_back(dataRef.index);
}


void NiPoint3Interpolator::Sync(NiStreamReversible& stream) {
	stream.Sync(point3Value);
	dataRef.Sync(stream);
}

void NiPoint3Interpolator::GetChildRefs(std::set<NiRef*>& refs) {
	NiKeyBasedInterpolator::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void NiPoint3Interpolator::GetChildIndices(std::vector<int>& indices) {
	NiKeyBasedInterpolator::GetChildIndices(indices);

	indices.push_back(dataRef.index);
}


void NiPathInterpolator::Sync(NiStreamReversible& stream) {
	stream.Sync(pathFlags);
	stream.Sync(bankDir);
	stream.Sync(maxBankAngle);
	stream.Sync(smoothing);
	stream.Sync(followAxis);
	pathDataRef.Sync(stream);
	percentDataRef.Sync(stream);
}

void NiPathInterpolator::GetChildRefs(std::set<NiRef*>& refs) {
	NiKeyBasedInterpolator::GetChildRefs(refs);

	refs.insert(&pathDataRef);
	refs.insert(&percentDataRef);
}

void NiPathInterpolator::GetChildIndices(std::vector<int>& indices) {
	NiKeyBasedInterpolator::GetChildIndices(indices);

	indices.push_back(pathDataRef.index);
	indices.push_back(percentDataRef.index);
}


void NiLookAtInterpolator::Sync(NiStreamReversible& stream) {
	stream.Sync(flags);
	lookAtRef.Sync(stream);
	lookAtName.Sync(stream);
	stream.Sync(transform);
	translateInterpRef.Sync(stream);
	rollInterpRef.Sync(stream);
	scaleInterpRef.Sync(stream);
}

void NiLookAtInterpolator::GetStringRefs(std::vector<NiStringRef*>& refs) {
	NiInterpolator::GetStringRefs(refs);

	refs.emplace_back(&lookAtName);
}

void NiLookAtInterpolator::GetChildRefs(std::set<NiRef*>& refs) {
	NiInterpolator::GetChildRefs(refs);

	refs.insert(&translateInterpRef);
	refs.insert(&rollInterpRef);
	refs.insert(&scaleInterpRef);
}

void NiLookAtInterpolator::GetChildIndices(std::vector<int>& indices) {
	NiInterpolator::GetChildIndices(indices);

	indices.push_back(translateInterpRef.index);
	indices.push_back(rollInterpRef.index);
	indices.push_back(scaleInterpRef.index);
}

void NiLookAtInterpolator::GetPtrs(std::set<NiPtr*>& ptrs) {
	NiInterpolator::GetPtrs(ptrs);

	ptrs.insert(&lookAtRef);
}


void BSTreadTransfInterpolator::Sync(NiStreamReversible& stream) {
	stream.Sync(numTreadTransforms);
	treadTransforms.resize(numTreadTransforms);
	for (uint32_t i = 0; i < numTreadTransforms; i++)
		treadTransforms[i].Sync(stream);

	dataRef.Sync(stream);
}

void BSTreadTransfInterpolator::GetStringRefs(std::vector<NiStringRef*>& refs) {
	NiInterpolator::GetStringRefs(refs);

	for (uint32_t i = 0; i < numTreadTransforms; i++)
		treadTransforms[i].GetStringRefs(refs);
}

void BSTreadTransfInterpolator::GetChildRefs(std::set<NiRef*>& refs) {
	NiInterpolator::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void BSTreadTransfInterpolator::GetChildIndices(std::vector<int>& indices) {
	NiInterpolator::GetChildIndices(indices);

	indices.push_back(dataRef.index);
}

std::vector<BSTreadTransform> BSTreadTransfInterpolator::GetTreadTransforms() const {
	return treadTransforms;
}

void BSTreadTransfInterpolator::SetTreadTransforms(const std::vector<BSTreadTransform>& tt) {
	numTreadTransforms = tt.size();
	treadTransforms = tt;
}


void NiStringPalette::Sync(NiStreamReversible& stream) {
	palette.Sync(stream, 4);
	length = palette.length();
	stream.Sync(length);
}


void NiSequence::Sync(NiStreamReversible& stream) {
	name.Sync(stream);

	stream.Sync(numControlledBlocks);
	stream.Sync(arrayGrowBy);

	controlledBlocks.resize(numControlledBlocks);
	for (uint32_t i = 0; i < numControlledBlocks; i++) {
		controlledBlocks[i].interpolatorRef.Sync(stream);
		controlledBlocks[i].controllerRef.Sync(stream);
		stream.Sync(controlledBlocks[i].priority);

		controlledBlocks[i].nodeName.Sync(stream);
		controlledBlocks[i].propType.Sync(stream);
		controlledBlocks[i].ctrlType.Sync(stream);
		controlledBlocks[i].ctrlID.Sync(stream);
		controlledBlocks[i].interpID.Sync(stream);
	}
}

void NiSequence::GetStringRefs(std::vector<NiStringRef*>& refs) {
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

void NiSequence::GetChildRefs(std::set<NiRef*>& refs) {
	NiObject::GetChildRefs(refs);

	for (uint32_t i = 0; i < numControlledBlocks; i++) {
		refs.insert(&controlledBlocks[i].interpolatorRef);
		refs.insert(&controlledBlocks[i].controllerRef);
	}
}

void NiSequence::GetChildIndices(std::vector<int>& indices) {
	NiObject::GetChildIndices(indices);

	for (uint32_t i = 0; i < numControlledBlocks; i++) {
		indices.push_back(controlledBlocks[i].interpolatorRef.index);
		indices.push_back(controlledBlocks[i].controllerRef.index);
	}
}

std::vector<ControllerLink> NiSequence::GetControlledBlocks() const {
	return controlledBlocks;
}

void NiSequence::SetControlledBlocks(const std::vector<ControllerLink>& cl) {
	numControlledBlocks = cl.size();
	controlledBlocks = cl;
}


void BSAnimNote::Sync(NiStreamReversible& stream) {
	stream.Sync(type);
	stream.Sync(time);

	if (type == ANT_GRABIK)
		stream.Sync(arm);

	if (type != ANT_INVALID) {
		stream.Sync(gain);
		stream.Sync(state);
	}
}


void BSAnimNotes::Sync(NiStreamReversible& stream) {
	animNoteRefs.Sync(stream);
}

void BSAnimNotes::GetChildRefs(std::set<NiRef*>& refs) {
	NiObject::GetChildRefs(refs);

	animNoteRefs.GetIndexPtrs(refs);
}

void BSAnimNotes::GetChildIndices(std::vector<int>& indices) {
	NiObject::GetChildIndices(indices);

	animNoteRefs.GetIndices(indices);
}


void NiControllerSequence::Sync(NiStreamReversible& stream) {
	stream.Sync(weight);
	textKeyRef.Sync(stream);
	stream.Sync(cycleType);
	stream.Sync(frequency);
	stream.Sync(startTime);
	stream.Sync(stopTime);
	managerRef.Sync(stream);
	accumRootName.Sync(stream);

	if (stream.GetVersion().Stream() >= 24 && stream.GetVersion().Stream() <= 28)
		animNotesRef.Sync(stream);
	else if (stream.GetVersion().Stream() > 28)
		animNotesRefs.Sync(stream);
}

void NiControllerSequence::GetStringRefs(std::vector<NiStringRef*>& refs) {
	NiSequence::GetStringRefs(refs);

	refs.emplace_back(&accumRootName);
}

void NiControllerSequence::GetChildRefs(std::set<NiRef*>& refs) {
	NiSequence::GetChildRefs(refs);

	refs.insert(&textKeyRef);
	refs.insert(&animNotesRef);
	animNotesRefs.GetIndexPtrs(refs);
}

void NiControllerSequence::GetChildIndices(std::vector<int>& indices) {
	NiSequence::GetChildIndices(indices);

	indices.push_back(textKeyRef.index);
	indices.push_back(animNotesRef.index);
	animNotesRefs.GetIndices(indices);
}

void NiControllerSequence::GetPtrs(std::set<NiPtr*>& ptrs) {
	NiSequence::GetPtrs(ptrs);

	ptrs.insert(&managerRef);
}


void NiControllerManager::Sync(NiStreamReversible& stream) {
	stream.Sync(cumulative);

	controllerSequenceRefs.Sync(stream);
	objectPaletteRef.Sync(stream);
}

void NiControllerManager::GetChildRefs(std::set<NiRef*>& refs) {
	NiTimeController::GetChildRefs(refs);

	controllerSequenceRefs.GetIndexPtrs(refs);
	refs.insert(&objectPaletteRef);
}

void NiControllerManager::GetChildIndices(std::vector<int>& indices) {
	NiTimeController::GetChildIndices(indices);

	controllerSequenceRefs.GetIndices(indices);
	indices.push_back(objectPaletteRef.index);
}
