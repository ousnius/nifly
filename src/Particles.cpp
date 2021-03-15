/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#include "Particles.hpp"

using namespace nifly;

NiParticlesData::NiParticlesData() {
	NiGeometryData::isPSys = true;
}

void NiParticlesData::Sync(NiStreamReversible& stream) {
	stream.Sync(hasRadii);
	stream.Sync(numActive);
	stream.Sync(hasSizes);
	stream.Sync(hasRotations);
	stream.Sync(hasRotationAngles);
	stream.Sync(hasRotationAxes);
	stream.Sync(hasTextureIndices);

	if (stream.GetVersion().User() >= 12) {
		stream.Sync(numSubtexOffsets);
	}
	else {
		uint8_t numOffsets = numSubtexOffsets > 255 ? 255 : numSubtexOffsets;
		stream.Sync(numOffsets);
		numSubtexOffsets = numOffsets;
	}

	subtexOffsets.resize(numSubtexOffsets);
	for (uint32_t i = 0; i < numSubtexOffsets; i++)
		stream.Sync(subtexOffsets[i]);

	if (stream.GetVersion().User() >= 12) {
		stream.Sync(aspectRatio);
		stream.Sync(aspectFlags);
		stream.Sync(speedToAspectAspect2);
		stream.Sync(speedToAspectSpeed1);
		stream.Sync(speedToAspectSpeed2);
	}
}


void NiParticleMeshesData::Sync(NiStreamReversible& stream) {
	dataRef.Sync(stream);
}

void NiParticleMeshesData::GetChildRefs(std::set<Ref*>& refs) {
	NiRotatingParticlesData::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void NiParticleMeshesData::GetChildIndices(std::vector<int>& indices) {
	NiRotatingParticlesData::GetChildIndices(indices);

	indices.push_back(dataRef.GetIndex());
}


void NiPSysData::Sync(NiStreamReversible& stream) {
	if (stream.GetVersion().Stream() == 155)
		stream.Sync(unknownVector);

	stream.Sync(hasRotationSpeeds);
}


void NiMeshPSysData::Sync(NiStreamReversible& stream) {
	stream.Sync(defaultPoolSize);
	stream.Sync(fillPoolsOnLoad);

	stream.Sync(numGenerations);
	generationPoolSize.resize(numGenerations);
	for (uint32_t i = 0; i < numGenerations; i++)
		stream.Sync(generationPoolSize[i]);

	nodeRef.Sync(stream);
}

void NiMeshPSysData::GetChildRefs(std::set<Ref*>& refs) {
	NiPSysData::GetChildRefs(refs);

	refs.insert(&nodeRef);
}

void NiMeshPSysData::GetChildIndices(std::vector<int>& indices) {
	NiPSysData::GetChildIndices(indices);

	indices.push_back(nodeRef.GetIndex());
}


void BSStripPSysData::Sync(NiStreamReversible& stream) {
	stream.Sync(maxPointCount);
	stream.Sync(startCapSize);
	stream.Sync(endCapSize);
	stream.Sync(doZPrepass);
}


void NiPSysEmitterCtlrData::Sync(NiStreamReversible& stream) {
	floatKeys.Sync(stream);

	stream.Sync(numVisibilityKeys);
	visibilityKeys.resize(numVisibilityKeys);
	for (uint32_t i = 0; i < numVisibilityKeys; i++) {
		stream.Sync(visibilityKeys[i].time);
		stream.Sync(visibilityKeys[i].value);
	}
}


void NiPSysModifier::Sync(NiStreamReversible& stream) {
	name.Sync(stream);

	stream.Sync(order);
	targetRef.Sync(stream);
	stream.Sync(isActive);
}

void NiPSysModifier::GetStringRefs(std::vector<StringRef*>& refs) {
	NiObject::GetStringRefs(refs);

	refs.emplace_back(&name);
}

void NiPSysModifier::GetPtrs(std::set<Ref*>& ptrs) {
	NiObject::GetPtrs(ptrs);

	ptrs.insert(&targetRef);
}


void BSPSysStripUpdateModifier::Sync(NiStreamReversible& stream) {
	stream.Sync(updateDeltaTime);
}


void NiPSysSpawnModifier::Sync(NiStreamReversible& stream) {
	stream.Sync(numSpawnGenerations);
	stream.Sync(percentSpawned);
	stream.Sync(minSpawned);
	stream.Sync(maxSpawned);
	stream.Sync(spawnSpeedVariation);
	stream.Sync(spawnDirVariation);
	stream.Sync(lifeSpan);
	stream.Sync(lifeSpanVariation);
}


void NiPSysAgeDeathModifier::Sync(NiStreamReversible& stream) {
	stream.Sync(spawnOnDeath);
	spawnModifierRef.Sync(stream);
}

void NiPSysAgeDeathModifier::GetChildRefs(std::set<Ref*>& refs) {
	NiPSysModifier::GetChildRefs(refs);

	refs.insert(&spawnModifierRef);
}

void NiPSysAgeDeathModifier::GetChildIndices(std::vector<int>& indices) {
	NiPSysModifier::GetChildIndices(indices);

	indices.push_back(spawnModifierRef.GetIndex());
}


void BSPSysLODModifier::Sync(NiStreamReversible& stream) {
	stream.Sync(lodBeginDistance);
	stream.Sync(lodEndDistance);
	stream.Sync(unknownFadeFactor1);
	stream.Sync(unknownFadeFactor2);
}


void BSPSysSimpleColorModifier::Sync(NiStreamReversible& stream) {
	stream.Sync(fadeInPercent);
	stream.Sync(fadeOutPercent);
	stream.Sync(color1EndPercent);
	stream.Sync(color2StartPercent);
	stream.Sync(color2EndPercent);
	stream.Sync(color3StartPercent);
	stream.Sync(color1);
	stream.Sync(color2);
	stream.Sync(color3);

	if (stream.GetVersion().Stream() == 155) {
		for (uint16_t& unknownShort : unknownShorts)
			stream.Sync(unknownShort);
	}
}


void NiPSysRotationModifier::Sync(NiStreamReversible& stream) {
	stream.Sync(initialSpeed);
	stream.Sync(initialSpeedVariation);

	if (stream.GetVersion().Stream() == 155) {
		stream.Sync(unknownVector);
		stream.Sync(unknownByte);
	}

	stream.Sync(initialAngle);
	stream.Sync(initialAngleVariation);
	stream.Sync(randomSpeedSign);
	stream.Sync(randomInitialAxis);
	stream.Sync(initialAxis);
}


void BSPSysScaleModifier::Sync(NiStreamReversible& stream) {
	stream.Sync(numFloats);
	floats.resize(numFloats);
	for (uint32_t i = 0; i < numFloats; i++)
		stream.Sync(floats[i]);
}


void NiPSysGravityModifier::Sync(NiStreamReversible& stream) {
	gravityObjRef.Sync(stream);
	stream.Sync(gravityAxis);
	stream.Sync(decay);
	stream.Sync(strength);
	stream.Sync(forceType);
	stream.Sync(turbulence);
	stream.Sync(turbulenceScale);
	stream.Sync(worldAligned);
}

void NiPSysGravityModifier::GetPtrs(std::set<Ref*>& ptrs) {
	NiPSysModifier::GetPtrs(ptrs);

	ptrs.insert(&gravityObjRef);
}


void NiPSysBoundUpdateModifier::Sync(NiStreamReversible& stream) {
	stream.Sync(updateSkip);
}


void NiPSysDragModifier::Sync(NiStreamReversible& stream) {
	parentRef.Sync(stream);
	stream.Sync(dragAxis);
	stream.Sync(percentage);
	stream.Sync(range);
	stream.Sync(rangeFalloff);
}

void NiPSysDragModifier::GetPtrs(std::set<Ref*>& ptrs) {
	NiPSysModifier::GetPtrs(ptrs);

	ptrs.insert(&parentRef);
}


void BSPSysInheritVelocityModifier::Sync(NiStreamReversible& stream) {
	targetNodeRef.Sync(stream);
	stream.Sync(changeToInherit);
	stream.Sync(velocityMult);
	stream.Sync(velocityVar);
}

void BSPSysInheritVelocityModifier::GetPtrs(std::set<Ref*>& ptrs) {
	NiPSysModifier::GetPtrs(ptrs);

	ptrs.insert(&targetNodeRef);
}


void BSPSysSubTexModifier::Sync(NiStreamReversible& stream) {
	stream.Sync(startFrame);
	stream.Sync(startFrameVariation);
	stream.Sync(endFrame);
	stream.Sync(loopStartFrame);
	stream.Sync(loopStartFrameVariation);
	stream.Sync(frameCount);
	stream.Sync(frameCountVariation);
}


void NiPSysBombModifier::Sync(NiStreamReversible& stream) {
	bombNodeRef.Sync(stream);
	stream.Sync(bombAxis);
	stream.Sync(decay);
	stream.Sync(deltaV);
	stream.Sync(decayType);
	stream.Sync(symmetryType);
}

void NiPSysBombModifier::GetPtrs(std::set<Ref*>& ptrs) {
	NiPSysModifier::GetPtrs(ptrs);

	ptrs.insert(&bombNodeRef);
}


void NiColorData::Sync(NiStreamReversible& stream) {
	data.Sync(stream);
}


void NiPSysColorModifier::Sync(NiStreamReversible& stream) {
	dataRef.Sync(stream);
}

void NiPSysColorModifier::GetChildRefs(std::set<Ref*>& refs) {
	NiPSysModifier::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void NiPSysColorModifier::GetChildIndices(std::vector<int>& indices) {
	NiPSysModifier::GetChildIndices(indices);

	indices.push_back(dataRef.GetIndex());
}


void NiPSysGrowFadeModifier::Sync(NiStreamReversible& stream) {
	stream.Sync(growTime);
	stream.Sync(growGeneration);
	stream.Sync(fadeTime);
	stream.Sync(fadeGeneration);

	if (stream.GetVersion().Stream() >= 34)
		stream.Sync(baseScale);
}


void NiPSysMeshUpdateModifier::Sync(NiStreamReversible& stream) {
	meshRefs.Sync(stream);
}

void NiPSysMeshUpdateModifier::GetChildRefs(std::set<Ref*>& refs) {
	NiPSysModifier::GetChildRefs(refs);

	meshRefs.GetIndexPtrs(refs);
}

void NiPSysMeshUpdateModifier::GetChildIndices(std::vector<int>& indices) {
	NiPSysModifier::GetChildIndices(indices);

	meshRefs.GetIndices(indices);
}

BlockRefArray<NiAVObject>& NiPSysMeshUpdateModifier::GetMeshes() {
	return meshRefs;
}


void NiPSysFieldModifier::Sync(NiStreamReversible& stream) {
	fieldObjectRef.Sync(stream);
	stream.Sync(magnitude);
	stream.Sync(attenuation);
	stream.Sync(useMaxDistance);
	stream.Sync(maxDistance);
}

void NiPSysFieldModifier::GetChildRefs(std::set<Ref*>& refs) {
	NiPSysModifier::GetChildRefs(refs);

	refs.insert(&fieldObjectRef);
}

void NiPSysFieldModifier::GetChildIndices(std::vector<int>& indices) {
	NiPSysModifier::GetChildIndices(indices);

	indices.push_back(fieldObjectRef.GetIndex());
}


void NiPSysVortexFieldModifier::Sync(NiStreamReversible& stream) {
	stream.Sync(direction);
}


void NiPSysGravityFieldModifier::Sync(NiStreamReversible& stream) {
	stream.Sync(direction);
}


void NiPSysDragFieldModifier::Sync(NiStreamReversible& stream) {
	stream.Sync(useDirection);
	stream.Sync(direction);
}


void NiPSysTurbulenceFieldModifier::Sync(NiStreamReversible& stream) {
	stream.Sync(frequency);
}


void NiPSysAirFieldModifier::Sync(NiStreamReversible& stream) {
	stream.Sync(direction);
	stream.Sync(unkFloat1);
	stream.Sync(unkFloat2);
	stream.Sync(unkBool1);
	stream.Sync(unkBool2);
	stream.Sync(unkBool3);
	stream.Sync(unkFloat3);
}


void NiPSysRadialFieldModifier::Sync(NiStreamReversible& stream) {
	stream.Sync(radialType);
}


void BSWindModifier::Sync(NiStreamReversible& stream) {
	stream.Sync(strength);
}


void BSPSysRecycleBoundModifier::Sync(NiStreamReversible& stream) {
	stream.Sync(boundOffset);
	stream.Sync(boundExtent);
	targetNodeRef.Sync(stream);
}

void BSPSysRecycleBoundModifier::GetPtrs(std::set<Ref*>& ptrs) {
	NiPSysModifier::GetPtrs(ptrs);

	ptrs.insert(&targetNodeRef);
}


void BSPSysHavokUpdateModifier::Sync(NiStreamReversible& stream) {
	nodeRefs.Sync(stream);
	modifierRef.Sync(stream);
}

void BSPSysHavokUpdateModifier::GetChildRefs(std::set<Ref*>& refs) {
	NiPSysModifier::GetChildRefs(refs);

	nodeRefs.GetIndexPtrs(refs);
	refs.insert(&modifierRef);
}

void BSPSysHavokUpdateModifier::GetChildIndices(std::vector<int>& indices) {
	NiPSysModifier::GetChildIndices(indices);

	nodeRefs.GetIndices(indices);
	indices.push_back(modifierRef.GetIndex());
}


BlockRefArray<NiNode>& BSPSysHavokUpdateModifier::GetNodes() {
	return nodeRefs;
}

int BSPSysHavokUpdateModifier::GetModifierRef() {
	return modifierRef.GetIndex();
}

void BSPSysHavokUpdateModifier::SetModifierRef(int modRef) {
	modifierRef.SetIndex(modRef);
}


void BSParentVelocityModifier::Sync(NiStreamReversible& stream) {
	stream.Sync(damping);
}


void BSMasterParticleSystem::Sync(NiStreamReversible& stream) {
	stream.Sync(maxEmitterObjs);
	particleSysRefs.Sync(stream);
}

void BSMasterParticleSystem::GetChildRefs(std::set<Ref*>& refs) {
	NiNode::GetChildRefs(refs);

	particleSysRefs.GetIndexPtrs(refs);
}

void BSMasterParticleSystem::GetChildIndices(std::vector<int>& indices) {
	NiNode::GetChildIndices(indices);

	particleSysRefs.GetIndices(indices);
}

BlockRefArray<NiAVObject>& BSMasterParticleSystem::GetParticleSystems() {
	return particleSysRefs;
}


void NiParticleSystem::Sync(NiStreamReversible& stream) {
	if (stream.GetVersion().Stream() >= 100) {
		stream.Sync(bounds);

		if (stream.GetVersion().Stream() == 155)
			for (float& i : boundMinMax)
				stream.Sync(i);

		skinInstanceRef.Sync(stream);
		shaderPropertyRef.Sync(stream);
		alphaPropertyRef.Sync(stream);
		stream.Sync(vertFlags1);
		stream.Sync(vertFlags2);
		stream.Sync(vertFlags3);
		stream.Sync(vertFlags4);
		stream.Sync(vertFlags5);
		stream.Sync(vertFlags6);
		stream.Sync(vertFlags7);
		stream.Sync(vertFlags8);
	}
	else {
		dataRef.Sync(stream);
		psysDataRef.SetIndex(dataRef.GetIndex());
		skinInstanceRef.Sync(stream);

		stream.Sync(numMaterials);
		materialNameRefs.resize(numMaterials);
		for (uint32_t i = 0; i < numMaterials; i++)
			materialNameRefs[i].Sync(stream);

		materials.resize(numMaterials);
		for (uint32_t i = 0; i < numMaterials; i++)
			stream.Sync(materials[i]);

		stream.Sync(activeMaterial);
		stream.Sync(defaultMatNeedsUpdate);

		if (stream.GetVersion().User() >= 12) {
			shaderPropertyRef.Sync(stream);
			alphaPropertyRef.Sync(stream);
		}
	}

	if (stream.GetVersion().User() >= 12) {
		stream.Sync(farBegin);
		stream.Sync(farEnd);
		stream.Sync(nearBegin);
		stream.Sync(nearEnd);

		if (stream.GetVersion().Stream() >= 100) {
			psysDataRef.Sync(stream);
			dataRef.SetIndex(psysDataRef.GetIndex());
		}
	}

	stream.Sync(isWorldSpace);
	modifierRefs.Sync(stream);
}

void NiParticleSystem::GetStringRefs(std::vector<StringRef*>& refs) {
	NiAVObject::GetStringRefs(refs);

	for (auto& m : materialNameRefs)
		refs.emplace_back(&m);
}

void NiParticleSystem::GetChildRefs(std::set<Ref*>& refs) {
	NiAVObject::GetChildRefs(refs);

	refs.insert(&dataRef);
	refs.insert(&skinInstanceRef);
	refs.insert(&shaderPropertyRef);
	refs.insert(&alphaPropertyRef);
	refs.insert(&psysDataRef);
	modifierRefs.GetIndexPtrs(refs);
}

void NiParticleSystem::GetChildIndices(std::vector<int>& indices) {
	NiAVObject::GetChildIndices(indices);

	indices.push_back(dataRef.GetIndex());
	indices.push_back(skinInstanceRef.GetIndex());
	indices.push_back(shaderPropertyRef.GetIndex());
	indices.push_back(alphaPropertyRef.GetIndex());
	indices.push_back(psysDataRef.GetIndex());
	modifierRefs.GetIndices(indices);
}

int NiParticleSystem::GetDataRef() {
	return dataRef.GetIndex();
}

void NiParticleSystem::SetDataRef(int datRef) {
	dataRef.SetIndex(datRef);
}

int NiParticleSystem::GetSkinInstanceRef() {
	return skinInstanceRef.GetIndex();
}

void NiParticleSystem::SetSkinInstanceRef(int skinRef) {
	skinInstanceRef.SetIndex(skinRef);
}

int NiParticleSystem::GetShaderPropertyRef() {
	return shaderPropertyRef.GetIndex();
}

void NiParticleSystem::SetShaderPropertyRef(int shaderRef) {
	shaderPropertyRef.SetIndex(shaderRef);
}

int NiParticleSystem::GetAlphaPropertyRef() {
	return alphaPropertyRef.GetIndex();
}

void NiParticleSystem::SetAlphaPropertyRef(int alphaRef) {
	alphaPropertyRef.SetIndex(alphaRef);
}

int NiParticleSystem::GetPSysDataRef() {
	return psysDataRef.GetIndex();
}

void NiParticleSystem::SetPSysDataRef(int psysDatRef) {
	psysDataRef.SetIndex(psysDatRef);
}

BlockRefArray<NiPSysModifier>& NiParticleSystem::GetModifiers() {
	return modifierRefs;
}


void NiPSysCollider::Sync(NiStreamReversible& stream) {
	stream.Sync(bounce);
	stream.Sync(spawnOnCollide);
	stream.Sync(dieOnCollide);
	spawnModifierRef.Sync(stream);
	managerRef.Sync(stream);
	nextColliderRef.Sync(stream);
	colliderNodeRef.Sync(stream);
}

void NiPSysCollider::GetChildRefs(std::set<Ref*>& refs) {
	NiObject::GetChildRefs(refs);

	refs.insert(&spawnModifierRef);
	refs.insert(&nextColliderRef);
}

void NiPSysCollider::GetChildIndices(std::vector<int>& indices) {
	NiObject::GetChildIndices(indices);

	indices.push_back(spawnModifierRef.GetIndex());
	indices.push_back(nextColliderRef.GetIndex());
}

void NiPSysCollider::GetPtrs(std::set<Ref*>& ptrs) {
	NiObject::GetPtrs(ptrs);

	ptrs.insert(&managerRef);
	ptrs.insert(&colliderNodeRef);
}


void NiPSysSphericalCollider::Sync(NiStreamReversible& stream) {
	stream.Sync(radius);
}


void NiPSysPlanarCollider::Sync(NiStreamReversible& stream) {
	stream.Sync(width);
	stream.Sync(height);
	stream.Sync(xAxis);
	stream.Sync(yAxis);
}


void NiPSysColliderManager::Sync(NiStreamReversible& stream) {
	colliderRef.Sync(stream);
}

void NiPSysColliderManager::GetChildRefs(std::set<Ref*>& refs) {
	NiPSysModifier::GetChildRefs(refs);

	refs.insert(&colliderRef);
}

void NiPSysColliderManager::GetChildIndices(std::vector<int>& indices) {
	NiPSysModifier::GetChildIndices(indices);

	indices.push_back(colliderRef.GetIndex());
}


void NiPSysEmitter::Sync(NiStreamReversible& stream) {
	stream.Sync(speed);
	stream.Sync(speedVariation);
	stream.Sync(declination);
	stream.Sync(declinationVariation);
	stream.Sync(planarAngle);
	stream.Sync(planarAngleVariation);
	stream.Sync(color);
	stream.Sync(radius);
	stream.Sync(radiusVariation);
	stream.Sync(lifeSpan);
	stream.Sync(lifeSpanVariation);
}


void NiPSysVolumeEmitter::Sync(NiStreamReversible& stream) {
	emitterNodeRef.Sync(stream);
}

void NiPSysVolumeEmitter::GetPtrs(std::set<Ref*>& ptrs) {
	NiPSysEmitter::GetPtrs(ptrs);

	ptrs.insert(&emitterNodeRef);
}


void NiPSysSphereEmitter::Sync(NiStreamReversible& stream) {
	stream.Sync(radius);
}


void NiPSysCylinderEmitter::Sync(NiStreamReversible& stream) {
	stream.Sync(radius);
	stream.Sync(height);
}


void NiPSysBoxEmitter::Sync(NiStreamReversible& stream) {
	stream.Sync(width);
	stream.Sync(height);
	stream.Sync(depth);
}


void NiPSysMeshEmitter::Sync(NiStreamReversible& stream) {
	meshRefs.Sync(stream);

	stream.Sync(velocityType);
	stream.Sync(emissionType);
	stream.Sync(emissionAxis);
}

void NiPSysMeshEmitter::GetChildRefs(std::set<Ref*>& refs) {
	NiPSysEmitter::GetChildRefs(refs);

	meshRefs.GetIndexPtrs(refs);
}

void NiPSysMeshEmitter::GetChildIndices(std::vector<int>& indices) {
	NiPSysEmitter::GetChildIndices(indices);

	meshRefs.GetIndices(indices);
}

BlockRefArray<NiAVObject>& NiPSysMeshEmitter::GetMeshes() {
	return meshRefs;
}
