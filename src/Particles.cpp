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
	if (hasRadii && stream.GetVersion().File() != V20_2_0_7) {
		radii.resize(numVertices);
		for (float& r : radii)
			stream.Sync(r);
	}

	stream.Sync(numActive);

	stream.Sync(hasSizes);
	if (hasSizes && stream.GetVersion().File() != V20_2_0_7) {
		sizes.resize(numVertices);
		for (float& s : sizes)
			stream.Sync(s);
	}

	stream.Sync(hasRotations);
	if (hasRotations && stream.GetVersion().File() != V20_2_0_7) {
		rotations.resize(numVertices);
		for (Quaternion& q : rotations)
			stream.Sync(q);
	}

	stream.Sync(hasRotationAngles);
	if (hasRotationAngles && stream.GetVersion().File() != V20_2_0_7) {
		rotationAngles.resize(numVertices);
		for (float& a : rotationAngles)
			stream.Sync(a);
	}

	stream.Sync(hasRotationAxes);
	if (hasRotationAxes && stream.GetVersion().File() != V20_2_0_7) {
		rotationAxes.resize(numVertices);
		for (Vector3& a : rotationAxes)
			stream.Sync(a);
	}

	if (stream.GetVersion().File() == V20_2_0_7) {
		stream.Sync(hasTextureIndices);

		uint32_t sz = 0;

		if (stream.GetVersion().User() >= 12) {
			sz = subtexOffsets.SyncSize(stream);
		}
		else {
			uint8_t numOffsets = subtexOffsets.size() > 255 ? 255 : static_cast<uint8_t>(subtexOffsets.size());
			stream.Sync(numOffsets);
			sz = numOffsets;
		}

		subtexOffsets.SyncData(stream, sz);

		if (stream.GetVersion().User() >= 12) {
			stream.Sync(aspectRatio);
			stream.Sync(aspectFlags);
			stream.Sync(speedToAspectAspect2);
			stream.Sync(speedToAspectSpeed1);
			stream.Sync(speedToAspectSpeed2);
		}
	}
}


void NiParticleMeshesData::Sync(NiStreamReversible& stream) {
	dataRef.Sync(stream);
}

void NiParticleMeshesData::GetChildRefs(std::set<NiRef*>& refs) {
	NiRotatingParticlesData::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void NiParticleMeshesData::GetChildIndices(std::vector<uint32_t>& indices) {
	NiRotatingParticlesData::GetChildIndices(indices);

	indices.push_back(dataRef.index);
}


void NiPSysData::Sync(NiStreamReversible& stream) {
	if (stream.GetVersion().File() != V20_2_0_7) {
		particleInfo.resize(numVertices);
		for (auto& pi : particleInfo)
			pi.Sync(stream);
	}

	if (stream.GetVersion().Stream() > 130)
		stream.Sync(unknownVector);

	if (stream.GetVersion().File() == V20_2_4_7)
		stream.Sync(unknownQQSpeedByte1);

	if (stream.GetVersion().File() >= V20_0_0_2) {
		stream.Sync(hasRotationSpeeds);

		if (hasRotationSpeeds && stream.GetVersion().File() != V20_2_0_7) {
			rotationSpeeds.resize(numVertices);
			for (auto& rs : rotationSpeeds)
				stream.Sync(rs);
		}
	}

	if (stream.GetVersion().File() != V20_2_0_7) {
		stream.Sync(numAddedParticles);
		stream.Sync(addedParticlesBase);
	}

	if (stream.GetVersion().File() == V20_2_4_7)
		stream.Sync(unknownQQSpeedByte2);
}


void NiMeshPSysData::Sync(NiStreamReversible& stream) {
	stream.Sync(defaultPoolSize);
	stream.Sync(fillPoolsOnLoad);

	generationPoolSize.Sync(stream);
	nodeRef.Sync(stream);
}

void NiMeshPSysData::GetChildRefs(std::set<NiRef*>& refs) {
	NiPSysData::GetChildRefs(refs);

	refs.insert(&nodeRef);
}

void NiMeshPSysData::GetChildIndices(std::vector<uint32_t>& indices) {
	NiPSysData::GetChildIndices(indices);

	indices.push_back(nodeRef.index);
}


void BSStripPSysData::Sync(NiStreamReversible& stream) {
	stream.Sync(maxPointCount);
	stream.Sync(startCapSize);
	stream.Sync(endCapSize);
	stream.Sync(doZPrepass);
}


void NiPSysEmitterCtlrData::Sync(NiStreamReversible& stream) {
	floatKeys.Sync(stream);
	visibilityKeys.Sync(stream);
}


void NiPSysEmitterCtlr::Sync(NiStreamReversible& stream) {
	if (stream.GetVersion().File() < V10_1_0_104)
		dataRef.Sync(stream);
	else
		visInterpolatorRef.Sync(stream);
}

void NiPSysEmitterCtlr::GetChildRefs(std::set<NiRef*>& refs) {
	NiPSysModifierCtlr::GetChildRefs(refs);

	refs.insert(&dataRef);
	refs.insert(&visInterpolatorRef);
}

void NiPSysEmitterCtlr::GetChildIndices(std::vector<uint32_t>& indices) {
	NiPSysModifierCtlr::GetChildIndices(indices);

	indices.push_back(dataRef.index);
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


void NiPSysModifier::Sync(NiStreamReversible& stream) {
	name.Sync(stream);

	stream.Sync(order);
	targetRef.Sync(stream);
	stream.Sync(isActive);
}

void NiPSysModifier::GetStringRefs(std::vector<NiStringRef*>& refs) {
	NiObject::GetStringRefs(refs);

	refs.emplace_back(&name);
}

void NiPSysModifier::GetPtrs(std::set<NiPtr*>& ptrs) {
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

void NiPSysAgeDeathModifier::GetChildRefs(std::set<NiRef*>& refs) {
	NiPSysModifier::GetChildRefs(refs);

	refs.insert(&spawnModifierRef);
}

void NiPSysAgeDeathModifier::GetChildIndices(std::vector<uint32_t>& indices) {
	NiPSysModifier::GetChildIndices(indices);

	indices.push_back(spawnModifierRef.index);
}


void BSPSysLODModifier::Sync(NiStreamReversible& stream) {
	stream.Sync(lodBeginDistance);
	stream.Sync(lodEndDistance);
	stream.Sync(endEmitScale);
	stream.Sync(endSize);
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

	if (stream.GetVersion().Stream() > 130) {
		for (uint16_t& unknownShort : unknownShorts)
			stream.Sync(unknownShort);
	}
}


void NiPSysRotationModifier::Sync(NiStreamReversible& stream) {
	stream.Sync(initialSpeed);
	stream.Sync(initialSpeedVariation);

	if (stream.GetVersion().Stream() > 130) {
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
	floats.Sync(stream);
}


void NiPSysGravityModifier::Sync(NiStreamReversible& stream) {
	gravityObjRef.Sync(stream);
	stream.Sync(gravityAxis);
	stream.Sync(decay);
	stream.Sync(strength);
	stream.Sync(forceType);
	stream.Sync(turbulence);
	stream.Sync(turbulenceScale);

	if (stream.GetVersion().Stream() > 16)
		stream.Sync(worldAligned);
}

void NiPSysGravityModifier::GetPtrs(std::set<NiPtr*>& ptrs) {
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

void NiPSysDragModifier::GetPtrs(std::set<NiPtr*>& ptrs) {
	NiPSysModifier::GetPtrs(ptrs);

	ptrs.insert(&parentRef);
}


void BSPSysInheritVelocityModifier::Sync(NiStreamReversible& stream) {
	targetNodeRef.Sync(stream);
	stream.Sync(changeToInherit);
	stream.Sync(velocityMult);
	stream.Sync(velocityVar);
}

void BSPSysInheritVelocityModifier::GetPtrs(std::set<NiPtr*>& ptrs) {
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

void NiPSysBombModifier::GetPtrs(std::set<NiPtr*>& ptrs) {
	NiPSysModifier::GetPtrs(ptrs);

	ptrs.insert(&bombNodeRef);
}


void NiColorData::Sync(NiStreamReversible& stream) {
	data.Sync(stream);
}


void NiPSysColorModifier::Sync(NiStreamReversible& stream) {
	dataRef.Sync(stream);
}

void NiPSysColorModifier::GetChildRefs(std::set<NiRef*>& refs) {
	NiPSysModifier::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void NiPSysColorModifier::GetChildIndices(std::vector<uint32_t>& indices) {
	NiPSysModifier::GetChildIndices(indices);

	indices.push_back(dataRef.index);
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

void NiPSysMeshUpdateModifier::GetChildRefs(std::set<NiRef*>& refs) {
	NiPSysModifier::GetChildRefs(refs);

	meshRefs.GetIndexPtrs(refs);
}

void NiPSysMeshUpdateModifier::GetChildIndices(std::vector<uint32_t>& indices) {
	NiPSysModifier::GetChildIndices(indices);

	meshRefs.GetIndices(indices);
}


void NiPSysFieldModifier::Sync(NiStreamReversible& stream) {
	fieldObjectRef.Sync(stream);
	stream.Sync(magnitude);
	stream.Sync(attenuation);
	stream.Sync(useMaxDistance);
	stream.Sync(maxDistance);
}

void NiPSysFieldModifier::GetChildRefs(std::set<NiRef*>& refs) {
	NiPSysModifier::GetChildRefs(refs);

	refs.insert(&fieldObjectRef);
}

void NiPSysFieldModifier::GetChildIndices(std::vector<uint32_t>& indices) {
	NiPSysModifier::GetChildIndices(indices);

	indices.push_back(fieldObjectRef.index);
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
	stream.Sync(airFriction);
	stream.Sync(inheritVelocity);
	stream.Sync(inheritRotation);
	stream.Sync(componentOnly);
	stream.Sync(enableSpread);
	stream.Sync(spread);
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

void BSPSysRecycleBoundModifier::GetPtrs(std::set<NiPtr*>& ptrs) {
	NiPSysModifier::GetPtrs(ptrs);

	ptrs.insert(&targetNodeRef);
}


void BSPSysHavokUpdateModifier::Sync(NiStreamReversible& stream) {
	nodeRefs.Sync(stream);
	modifierRef.Sync(stream);
}

void BSPSysHavokUpdateModifier::GetChildRefs(std::set<NiRef*>& refs) {
	NiPSysModifier::GetChildRefs(refs);

	nodeRefs.GetIndexPtrs(refs);
	refs.insert(&modifierRef);
}

void BSPSysHavokUpdateModifier::GetChildIndices(std::vector<uint32_t>& indices) {
	NiPSysModifier::GetChildIndices(indices);

	nodeRefs.GetIndices(indices);
	indices.push_back(modifierRef.index);
}


void BSParentVelocityModifier::Sync(NiStreamReversible& stream) {
	stream.Sync(damping);
}


void BSMasterParticleSystem::Sync(NiStreamReversible& stream) {
	stream.Sync(maxEmitterObjs);
	particleSysRefs.Sync(stream);
}

void BSMasterParticleSystem::GetChildRefs(std::set<NiRef*>& refs) {
	NiNode::GetChildRefs(refs);

	particleSysRefs.GetIndexPtrs(refs);
}

void BSMasterParticleSystem::GetChildIndices(std::vector<uint32_t>& indices) {
	NiNode::GetChildIndices(indices);

	particleSysRefs.GetIndices(indices);
}


void NiParticleSystem::Sync(NiStreamReversible& stream) {
	if (stream.GetVersion().Stream() >= 100) {
		stream.Sync(bounds);

		if (stream.GetVersion().Stream() > 139)
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
		psysDataRef.index = dataRef.index;
		skinInstanceRef.Sync(stream);

		if (stream.GetVersion().File() >= V10_0_1_0 && stream.GetVersion().File() <= V20_1_0_3) {
			stream.Sync(hasShader);
			if (hasShader) {
				shaderName.Sync(stream);
				stream.Sync(shaderExtraData);
			}
		}

		if (stream.GetVersion().File() >= V20_2_0_5) {
			uint32_t numMaterials = materialNames.Sync(stream);
			materialExtraData.SyncData(stream, numMaterials);

			stream.Sync(activeMaterial);
		}

		if (stream.GetVersion().File() >= V20_2_0_7)
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
			dataRef.index = psysDataRef.index;
		}
	}

	stream.Sync(isWorldSpace);
	modifierRefs.Sync(stream);
}

void NiParticleSystem::GetStringRefs(std::vector<NiStringRef*>& refs) {
	NiAVObject::GetStringRefs(refs);

	refs.emplace_back(&shaderName);

	for (auto& mn : materialNames)
		refs.emplace_back(&mn);
}

void NiParticleSystem::GetChildRefs(std::set<NiRef*>& refs) {
	NiAVObject::GetChildRefs(refs);

	refs.insert(&dataRef);
	refs.insert(&skinInstanceRef);
	refs.insert(&shaderPropertyRef);
	refs.insert(&alphaPropertyRef);
	refs.insert(&psysDataRef);
	modifierRefs.GetIndexPtrs(refs);
}

void NiParticleSystem::GetChildIndices(std::vector<uint32_t>& indices) {
	NiAVObject::GetChildIndices(indices);

	indices.push_back(dataRef.index);
	indices.push_back(skinInstanceRef.index);
	indices.push_back(shaderPropertyRef.index);
	indices.push_back(alphaPropertyRef.index);
	indices.push_back(psysDataRef.index);
	modifierRefs.GetIndices(indices);
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

void NiPSysCollider::GetChildRefs(std::set<NiRef*>& refs) {
	NiObject::GetChildRefs(refs);

	refs.insert(&spawnModifierRef);
	refs.insert(&nextColliderRef);
}

void NiPSysCollider::GetChildIndices(std::vector<uint32_t>& indices) {
	NiObject::GetChildIndices(indices);

	indices.push_back(spawnModifierRef.index);
	indices.push_back(nextColliderRef.index);
}

void NiPSysCollider::GetPtrs(std::set<NiPtr*>& ptrs) {
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

void NiPSysColliderManager::GetChildRefs(std::set<NiRef*>& refs) {
	NiPSysModifier::GetChildRefs(refs);

	refs.insert(&colliderRef);
}

void NiPSysColliderManager::GetChildIndices(std::vector<uint32_t>& indices) {
	NiPSysModifier::GetChildIndices(indices);

	indices.push_back(colliderRef.index);
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

void NiPSysVolumeEmitter::GetPtrs(std::set<NiPtr*>& ptrs) {
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

void NiPSysMeshEmitter::GetPtrs(std::set<NiPtr*>& ptrs) {
	NiPSysEmitter::GetPtrs(ptrs);

	meshRefs.GetIndexPtrs(ptrs);
}
