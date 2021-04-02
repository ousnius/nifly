/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#include "bhk.hpp"

using namespace nifly;

void NiCollisionObject::Sync(NiStreamReversible& stream) {
	targetRef.Sync(stream);
}

void NiCollisionObject::GetPtrs(std::set<NiPtr*>& ptrs) {
	NiObject::GetPtrs(ptrs);

	ptrs.insert(&targetRef);
}


void BoundingVolume::Sync(NiStreamReversible& stream) {
	stream.Sync(collisionType);

	switch (collisionType) {
		case SPHERE_BV: stream.Sync(bvSphere); break;
		case BOX_BV: stream.Sync(bvBox); break;
		case CAPSULE_BV: stream.Sync(bvCapsule); break;
		case UNION_BV: bvUnion->Sync(stream); break;
		case HALFSPACE_BV: stream.Sync(bvHalfSpace); break;
		default: break;
	}
}


void NiCollisionData::Sync(NiStreamReversible& stream) {
	stream.Sync(propagationMode);
	stream.Sync(collisionMode);
	stream.Sync(useABV);

	if (useABV)
		boundingVolume.Sync(stream);
}


void bhkNiCollisionObject::Sync(NiStreamReversible& stream) {
	stream.Sync(flags);
	bodyRef.Sync(stream);
}

void bhkNiCollisionObject::GetChildRefs(std::set<NiRef*>& refs) {
	NiCollisionObject::GetChildRefs(refs);

	refs.insert(&bodyRef);
}

void bhkNiCollisionObject::GetChildIndices(std::vector<int>& indices) {
	NiCollisionObject::GetChildIndices(indices);

	indices.push_back(bodyRef.index);
}


void bhkNPCollisionObject::Sync(NiStreamReversible& stream) {
	stream.Sync(bodyID);
}


void bhkBlendCollisionObject::Sync(NiStreamReversible& stream) {
	stream.Sync(heirGain);
	stream.Sync(velGain);
}


bhkPhysicsSystem::bhkPhysicsSystem(const uint32_t size) {
	numBytes = size;
	data.resize(size);
}

void bhkPhysicsSystem::Sync(NiStreamReversible& stream) {
	stream.Sync(numBytes);
	data.resize(numBytes);
	if (data.empty())
		return;

	stream.Sync(&data[0], numBytes);
}

std::vector<char> bhkPhysicsSystem::GetData() const {
	return data;
}

void bhkPhysicsSystem::SetData(const std::vector<char>& dat) {
	numBytes = dat.size();
	data = dat;
}


bhkRagdollSystem::bhkRagdollSystem(const uint32_t size) {
	numBytes = size;
	data.resize(size);
}

void bhkRagdollSystem::Sync(NiStreamReversible& stream) {
	stream.Sync(numBytes);
	data.resize(numBytes);
	if (data.empty())
		return;

	stream.Sync(&data[0], numBytes);
}

std::vector<char> bhkRagdollSystem::GetData() const {
	return data;
}

void bhkRagdollSystem::SetData(const std::vector<char>& dat) {
	numBytes = dat.size();
	data = dat;
}


void bhkBlendController::Sync(NiStreamReversible& stream) {
	stream.Sync(keys);
}


void bhkHeightFieldShape::Sync(NiStreamReversible& stream) {
	stream.Sync(material);
}


void bhkPlaneShape::Sync(NiStreamReversible& stream) {
	stream.Sync(unkVec);
	stream.Sync(plane);
	stream.Sync(halfExtents);
	stream.Sync(center);
}


void bhkSphereRepShape::Sync(NiStreamReversible& stream) {
	stream.Sync(material);
}


void bhkConvexShape::Sync(NiStreamReversible& stream) {
	stream.Sync(radius);
}


void bhkMultiSphereShape::Sync(NiStreamReversible& stream) {
	stream.Sync(shapeProperty);
	spheres.Sync(stream);
}


void bhkConvexListShape::Sync(NiStreamReversible& stream) {
	shapeRefs.Sync(stream);
	stream.Sync(material);
	stream.Sync(radius);
	stream.Sync(unkInt1);
	stream.Sync(unkFloat1);
	stream.Sync(childShapeProp);
	stream.Sync(useCachedAABB);
	stream.Sync(closestPointMinDistance);
}

void bhkConvexListShape::GetChildRefs(std::set<NiRef*>& refs) {
	bhkShape::GetChildRefs(refs);

	shapeRefs.GetIndexPtrs(refs);
}

void bhkConvexListShape::GetChildIndices(std::vector<int>& indices) {
	bhkShape::GetChildIndices(indices);

	shapeRefs.GetIndices(indices);
}


void bhkConvexVerticesShape::Sync(NiStreamReversible& stream) {
	stream.Sync(vertsProp);
	stream.Sync(normalsProp);
	verts.Sync(stream);
	normals.Sync(stream);
}


void bhkBoxShape::Sync(NiStreamReversible& stream) {
	stream.Sync(padding);
	stream.Sync(dimensions);
	stream.Sync(radius2);
}


void bhkTransformShape::Sync(NiStreamReversible& stream) {
	shapeRef.Sync(stream);
	stream.Sync(material);
	stream.Sync(radius);
	stream.Sync(padding);
	stream.Sync(xform);
}

void bhkTransformShape::GetChildRefs(std::set<NiRef*>& refs) {
	bhkShape::GetChildRefs(refs);

	refs.insert(&shapeRef);
}

void bhkTransformShape::GetChildIndices(std::vector<int>& indices) {
	bhkShape::GetChildIndices(indices);

	indices.push_back(shapeRef.index);
}


void bhkCapsuleShape::Sync(NiStreamReversible& stream) {
	stream.Sync(padding);
	stream.Sync(point1);
	stream.Sync(radius1);
	stream.Sync(point2);
	stream.Sync(radius2);
}


void bhkMoppBvTreeShape::Sync(NiStreamReversible& stream) {
	shapeRef.Sync(stream);
	stream.Sync(userData);
	stream.Sync(shapeCollection);
	stream.Sync(code);
	stream.Sync(scale);
	stream.Sync(dataSize);
	stream.Sync(offset);

	if (stream.GetVersion().User() >= 12)
		stream.Sync(buildType);

	data.resize(dataSize);
	for (uint32_t i = 0; i < dataSize; i++)
		stream.Sync(data[i]);
}

void bhkMoppBvTreeShape::GetChildRefs(std::set<NiRef*>& refs) {
	bhkBvTreeShape::GetChildRefs(refs);

	refs.insert(&shapeRef);
}

void bhkMoppBvTreeShape::GetChildIndices(std::vector<int>& indices) {
	bhkBvTreeShape::GetChildIndices(indices);

	indices.push_back(shapeRef.index);
}

std::vector<uint8_t> bhkMoppBvTreeShape::GetData() const {
	return data;
}

void bhkMoppBvTreeShape::SetData(const std::vector<uint8_t>& dat) {
	dataSize = dat.size();
	data = dat;
}


void bhkNiTriStripsShape::Sync(NiStreamReversible& stream) {
	stream.Sync(material);
	stream.Sync(radius);
	stream.Sync(unused1);
	stream.Sync(unused2);
	stream.Sync(unused3);
	stream.Sync(unused4);
	stream.Sync(unused5);
	stream.Sync(growBy);
	stream.Sync(scale);

	partRefs.Sync(stream);
	filters.Sync(stream);
}

void bhkNiTriStripsShape::GetChildRefs(std::set<NiRef*>& refs) {
	bhkShape::GetChildRefs(refs);

	partRefs.GetIndexPtrs(refs);
}

void bhkNiTriStripsShape::GetChildIndices(std::vector<int>& indices) {
	bhkShape::GetChildIndices(indices);

	partRefs.GetIndices(indices);
}


void bhkListShape::Sync(NiStreamReversible& stream) {
	subShapeRefs.Sync(stream);

	stream.Sync(material);
	stream.Sync(childShapeProp);
	stream.Sync(childFilterProp);

	filters.Sync(stream);
}

void bhkListShape::GetChildRefs(std::set<NiRef*>& refs) {
	bhkShapeCollection::GetChildRefs(refs);

	subShapeRefs.GetIndexPtrs(refs);
}

void bhkListShape::GetChildIndices(std::vector<int>& indices) {
	bhkShapeCollection::GetChildIndices(indices);

	subShapeRefs.GetIndices(indices);
}


void hkPackedNiTriStripsData::Sync(NiStreamReversible& stream) {
	stream.Sync(keyCount);

	if (stream.GetVersion().Stream() > 11) {
		triData.resize(keyCount);
		for (uint32_t i = 0; i < keyCount; i++)
			stream.Sync(triData[i]);
	}
	else {
		triNormData.resize(keyCount);
		for (uint32_t i = 0; i < keyCount; i++)
			stream.Sync(triNormData[i]);
	}

	stream.Sync(numVerts);

	if (stream.GetVersion().Stream() > 11)
		stream.Sync(compressed);

	compressedVertData.resize(numVerts);
	for (uint32_t i = 0; i < numVerts; i++)
		stream.Sync(compressedVertData[i]);

	if (stream.GetVersion().Stream() > 11)
		subPartData.Sync(stream);
}


void bhkPackedNiTriStripsShape::Sync(NiStreamReversible& stream) {
	if (stream.GetVersion().Stream() <= 11)
		subPartData.Sync(stream);

	stream.Sync(userData);
	stream.Sync(unused1);
	stream.Sync(radius);
	stream.Sync(unused2);
	stream.Sync(scaling);
	stream.Sync(radius2);
	stream.Sync(scaling2);
	dataRef.Sync(stream);
}

void bhkPackedNiTriStripsShape::GetChildRefs(std::set<NiRef*>& refs) {
	bhkShapeCollection::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void bhkPackedNiTriStripsShape::GetChildIndices(std::vector<int>& indices) {
	bhkShapeCollection::GetChildIndices(indices);

	indices.push_back(dataRef.index);
}


void bhkLiquidAction::Sync(NiStreamReversible& stream) {
	stream.Sync(userData);
	stream.Sync(unkInt1);
	stream.Sync(unkInt2);
	stream.Sync(initialStickForce);
	stream.Sync(stickStrength);
	stream.Sync(neighborDistance);
	stream.Sync(neighborStrength);
}


void bhkOrientHingedBodyAction::Sync(NiStreamReversible& stream) {
	bodyRef.Sync(stream);
	stream.Sync(unkInt1);
	stream.Sync(unkInt2);
	stream.Sync(padding);
	stream.Sync(hingeAxisLS);
	stream.Sync(forwardLS);
	stream.Sync(strength);
	stream.Sync(damping);
	stream.Sync(padding2);
}

void bhkOrientHingedBodyAction::GetPtrs(std::set<NiPtr*>& ptrs) {
	bhkSerializable::GetPtrs(ptrs);

	ptrs.insert(&bodyRef);
}


void bhkWorldObject::Sync(NiStreamReversible& stream) {
	shapeRef.Sync(stream);
	stream.Sync(collisionFilter);
	stream.Sync(unkInt1);
	stream.Sync(broadPhaseType);
	stream.Sync(reinterpret_cast<char*>(unkBytes), 3);
	stream.Sync(prop);
}

void bhkWorldObject::GetChildRefs(std::set<NiRef*>& refs) {
	bhkSerializable::GetChildRefs(refs);

	refs.insert(&shapeRef);
}

void bhkWorldObject::GetChildIndices(std::vector<int>& indices) {
	bhkSerializable::GetChildIndices(indices);

	indices.push_back(shapeRef.index);
}


void bhkSimpleShapePhantom::Sync(NiStreamReversible& stream) {
	stream.Sync(padding);
	stream.Sync(transform);
}


void bhkAabbPhantom::Sync(NiStreamReversible& stream) {
	stream.Sync(padding);
	stream.Sync(aabbMin);
	stream.Sync(aabbMax);
}


void bhkRigidBody::Sync(NiStreamReversible& stream) {
	stream.Sync(collisionResponse);
	stream.Sync(unusedByte1);
	stream.Sync(processContactCallbackDelay);
	stream.Sync(unkInt1);
	stream.Sync(collisionFilterCopy);
	stream.Sync(reinterpret_cast<char*>(unkShorts2), 12);
	stream.Sync(translation);
	stream.Sync(rotation);
	stream.Sync(linearVelocity);
	stream.Sync(angularVelocity);
	stream.Sync(reinterpret_cast<char*>(inertiaMatrix), 48);
	stream.Sync(center);
	stream.Sync(mass);
	stream.Sync(linearDamping);
	stream.Sync(angularDamping);

	if (stream.GetVersion().User() >= 12) {
		stream.Sync(timeFactor);
		stream.Sync(gravityFactor);
	}

	stream.Sync(friction);

	if (stream.GetVersion().User() >= 12)
		stream.Sync(rollingFrictionMult);

	stream.Sync(restitution);
	stream.Sync(maxLinearVelocity);
	stream.Sync(maxAngularVelocity);
	stream.Sync(penetrationDepth);
	stream.Sync(motionSystem);
	stream.Sync(deactivatorType);
	stream.Sync(solverDeactivation);
	stream.Sync(qualityType);
	stream.Sync(autoRemoveLevel);
	stream.Sync(responseModifierFlag);
	stream.Sync(numShapeKeysInContactPointProps);
	stream.Sync(forceCollideOntoPpu);
	stream.Sync(unkInt2);
	stream.Sync(unkInt3);

	if (stream.GetVersion().User() >= 12)
		stream.Sync(unkInt4);

	constraintRefs.Sync(stream);

	if (stream.GetVersion().User() <= 11)
		stream.Sync(unkInt5);

	if (stream.GetVersion().User() >= 12)
		stream.Sync(bodyFlags);
}

void bhkRigidBody::GetChildRefs(std::set<NiRef*>& refs) {
	bhkEntity::GetChildRefs(refs);

	constraintRefs.GetIndexPtrs(refs);
}

void bhkRigidBody::GetChildIndices(std::vector<int>& indices) {
	bhkEntity::GetChildIndices(indices);

	constraintRefs.GetIndices(indices);
}


void bhkConstraint::Sync(NiStreamReversible& stream) {
	entityRefs.Sync(stream);

	stream.Sync(priority);
}

void bhkConstraint::GetPtrs(std::set<NiPtr*>& ptrs) {
	bhkSerializable::GetPtrs(ptrs);

	entityRefs.GetIndexPtrs(ptrs);
}


void bhkHingeConstraint::Sync(NiStreamReversible& stream) {
	stream.Sync(hinge);
}


void bhkLimitedHingeConstraint::Sync(NiStreamReversible& stream) {
	stream.Sync(limitedHinge.hinge);
	stream.Sync(limitedHinge.minAngle);
	stream.Sync(limitedHinge.maxAngle);
	stream.Sync(limitedHinge.maxFriction);
	limitedHinge.motorDesc.Sync(stream);
}


void ConstraintData::Sync(NiStreamReversible& stream) {
	stream.Sync(type);

	entityRefs.Sync(stream);
	stream.Sync(priority);

	switch (type) {
		case BallAndSocket: stream.Sync(reinterpret_cast<char*>(&desc1), 32); break;
		case Hinge: stream.Sync(reinterpret_cast<char*>(&desc2), 128); break;
		case LimitedHinge:
			stream.Sync(desc3.hinge);
			stream.Sync(desc3.minAngle);
			stream.Sync(desc3.maxAngle);
			stream.Sync(desc3.maxFriction);
			desc3.motorDesc.Sync(stream);
			break;
		case Prismatic:
			stream.Sync(desc4.slidingA);
			stream.Sync(desc4.rotationA);
			stream.Sync(desc4.planeA);
			stream.Sync(desc4.pivotA);
			stream.Sync(desc4.slidingB);
			stream.Sync(desc4.rotationB);
			stream.Sync(desc4.planeB);
			stream.Sync(desc4.pivotB);
			stream.Sync(desc4.minDistance);
			stream.Sync(desc4.maxDistance);
			stream.Sync(desc4.friction);
			desc4.motorDesc.Sync(stream);
			break;
		case Ragdoll:
			stream.Sync(desc5.twistA);
			stream.Sync(desc5.planeA);
			stream.Sync(desc5.motorA);
			stream.Sync(desc5.pivotA);
			stream.Sync(desc5.twistB);
			stream.Sync(desc5.planeB);
			stream.Sync(desc5.motorB);
			stream.Sync(desc5.pivotB);
			stream.Sync(desc5.coneMaxAngle);
			stream.Sync(desc5.planeMinAngle);
			stream.Sync(desc5.planeMaxAngle);
			stream.Sync(desc5.twistMinAngle);
			stream.Sync(desc5.twistMaxAngle);
			stream.Sync(desc5.maxFriction);
			desc5.motorDesc.Sync(stream);
			break;
		case StiffSpring: stream.Sync(reinterpret_cast<char*>(&desc6), 36); break;
	}

	stream.Sync(strength);
}

void ConstraintData::GetPtrs(std::set<NiPtr*>& ptrs) {
	entityRefs.GetIndexPtrs(ptrs);
}


void bhkBreakableConstraint::Sync(NiStreamReversible& stream) {
	subConstraint.Sync(stream);
	stream.Sync(removeWhenBroken);
}

void bhkBreakableConstraint::GetPtrs(std::set<NiPtr*>& ptrs) {
	bhkConstraint::GetPtrs(ptrs);

	subConstraint.GetPtrs(ptrs);
}


void bhkRagdollConstraint::Sync(NiStreamReversible& stream) {
	stream.Sync(ragdoll.twistA);
	stream.Sync(ragdoll.planeA);
	stream.Sync(ragdoll.motorA);
	stream.Sync(ragdoll.pivotA);
	stream.Sync(ragdoll.twistB);
	stream.Sync(ragdoll.planeB);
	stream.Sync(ragdoll.motorB);
	stream.Sync(ragdoll.pivotB);
	stream.Sync(ragdoll.coneMaxAngle);
	stream.Sync(ragdoll.planeMinAngle);
	stream.Sync(ragdoll.planeMaxAngle);
	stream.Sync(ragdoll.twistMinAngle);
	stream.Sync(ragdoll.twistMaxAngle);
	stream.Sync(ragdoll.maxFriction);
	ragdoll.motorDesc.Sync(stream);
}


void bhkStiffSpringConstraint::Sync(NiStreamReversible& stream) {
	stream.Sync(stiffSpring.pivotA);
	stream.Sync(stiffSpring.pivotB);
	stream.Sync(stiffSpring.length);
}


void bhkPrismaticConstraint::Sync(NiStreamReversible& stream) {
	stream.Sync(prismatic.slidingA);
	stream.Sync(prismatic.rotationA);
	stream.Sync(prismatic.planeA);
	stream.Sync(prismatic.pivotA);
	stream.Sync(prismatic.slidingB);
	stream.Sync(prismatic.rotationB);
	stream.Sync(prismatic.planeB);
	stream.Sync(prismatic.pivotB);
	stream.Sync(prismatic.minDistance);
	stream.Sync(prismatic.maxDistance);
	stream.Sync(prismatic.friction);
	prismatic.motorDesc.Sync(stream);
}


void bhkMalleableConstraint::Sync(NiStreamReversible& stream) {
	subConstraint.Sync(stream);
}


void bhkBallAndSocketConstraint::Sync(NiStreamReversible& stream) {
	stream.Sync(ballAndSocket.translationA);
	stream.Sync(ballAndSocket.translationB);
}


void bhkBallSocketConstraintChain::Sync(NiStreamReversible& stream) {
	pivots.Sync(stream);

	stream.Sync(tau);
	stream.Sync(damping);
	stream.Sync(cfm);
	stream.Sync(maxErrorDistance);

	chainedEntityRefs.Sync(stream);

	numEntities = 2;
	stream.Sync(numEntities);
	numEntities = 2;

	entityARef.Sync(stream);
	entityBRef.Sync(stream);
	stream.Sync(priority);
}

void bhkBallSocketConstraintChain::GetPtrs(std::set<NiPtr*>& ptrs) {
	bhkSerializable::GetPtrs(ptrs);

	chainedEntityRefs.GetIndexPtrs(ptrs);
	ptrs.insert(&entityARef);
	ptrs.insert(&entityBRef);
}


void bhkCompressedMeshShapeData::Sync(NiStreamReversible& stream) {
	stream.Sync(bitsPerIndex);
	stream.Sync(bitsPerWIndex);
	stream.Sync(maskWIndex);
	stream.Sync(maskIndex);
	stream.Sync(error);
	stream.Sync(aabbBoundMin);
	stream.Sync(aabbBoundMax);
	stream.Sync(weldingType);
	stream.Sync(materialType);

	mat32.Sync(stream);
	mat16.Sync(stream);
	mat8.Sync(stream);

	materials.Sync(stream);

	stream.Sync(numNamedMat);

	transforms.Sync(stream);
	bigVerts.Sync(stream);

	stream.Sync(numBigTris);
	bigTris.resize(numBigTris);
	for (uint32_t i = 0; i < numBigTris; i++)
		stream.Sync(reinterpret_cast<char*>(&bigTris[i]), 12);

	stream.Sync(numChunks);
	chunks.resize(numChunks);
	for (uint32_t i = 0; i < numChunks; i++) {
		stream.Sync(chunks[i].translation);
		stream.Sync(chunks[i].matIndex);
		stream.Sync(chunks[i].reference);
		stream.Sync(chunks[i].transformIndex);

		stream.Sync(chunks[i].numVerts);
		chunks[i].verts.resize(chunks[i].numVerts);
		for (uint32_t j = 0; j < chunks[i].numVerts; j++)
			stream.Sync(chunks[i].verts[j]);

		stream.Sync(chunks[i].numIndices);
		chunks[i].indices.resize(chunks[i].numIndices);
		for (uint32_t j = 0; j < chunks[i].numIndices; j++)
			stream.Sync(chunks[i].indices[j]);

		stream.Sync(chunks[i].numStrips);
		chunks[i].strips.resize(chunks[i].numStrips);
		for (uint32_t j = 0; j < chunks[i].numStrips; j++)
			stream.Sync(chunks[i].strips[j]);

		stream.Sync(chunks[i].numWeldingInfo);
		chunks[i].weldingInfo.resize(chunks[i].numWeldingInfo);
		for (uint32_t j = 0; j < chunks[i].numWeldingInfo; j++)
			stream.Sync(chunks[i].weldingInfo[j]);
	}

	stream.Sync(numConvexPieceA);
}

std::vector<bhkCMSDBigTris> bhkCompressedMeshShapeData::GetBigTris() const {
	return bigTris;
}

void bhkCompressedMeshShapeData::SetBigTris(const std::vector<bhkCMSDBigTris>& bt) {
	numBigTris = bt.size();
	bigTris = bt;
}

std::vector<bhkCMSDChunk> bhkCompressedMeshShapeData::GetChunks() const {
	return chunks;
}

void bhkCompressedMeshShapeData::SetChunks(const std::vector<bhkCMSDChunk>& bt) {
	numChunks = bt.size();
	chunks = bt;
}


void bhkCompressedMeshShape::Sync(NiStreamReversible& stream) {
	targetRef.Sync(stream);
	stream.Sync(userData);
	stream.Sync(radius);
	stream.Sync(unkFloat);
	stream.Sync(scaling);
	stream.Sync(radius2);
	stream.Sync(scaling2);
	dataRef.Sync(stream);
}

void bhkCompressedMeshShape::GetChildRefs(std::set<NiRef*>& refs) {
	bhkShape::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void bhkCompressedMeshShape::GetChildIndices(std::vector<int>& indices) {
	bhkShape::GetChildIndices(indices);

	indices.push_back(dataRef.index);
}

void bhkCompressedMeshShape::GetPtrs(std::set<NiPtr*>& ptrs) {
	bhkShape::GetPtrs(ptrs);

	ptrs.insert(&targetRef);
}


void bhkPoseArray::Sync(NiStreamReversible& stream) {
	bones.Sync(stream);

	stream.Sync(numPoses);
	poses.resize(numPoses);
	for (uint32_t i = 0; i < numPoses; i++)
		poses[i].Sync(stream);
}

void bhkPoseArray::GetStringRefs(std::vector<NiStringRef*>& refs) {
	NiObject::GetStringRefs(refs);

	for (auto& b : bones)
		refs.emplace_back(&b);
}

std::vector<BonePose> bhkPoseArray::GetPoses() const {
	return poses;
}

void bhkPoseArray::SetPoses(const std::vector<BonePose>& po) {
	numPoses = po.size();
	poses = po;
}


void bhkRagdollTemplate::Sync(NiStreamReversible& stream) {
	boneRefs.Sync(stream);
}

void bhkRagdollTemplate::GetChildRefs(std::set<NiRef*>& refs) {
	NiExtraData::GetChildRefs(refs);

	boneRefs.GetIndexPtrs(refs);
}

void bhkRagdollTemplate::GetChildIndices(std::vector<int>& indices) {
	NiExtraData::GetChildIndices(indices);

	boneRefs.GetIndices(indices);
}


void bhkRagdollTemplateData::Sync(NiStreamReversible& stream) {
	name.Sync(stream);
	stream.Sync(mass);
	stream.Sync(restitution);
	stream.Sync(friction);
	stream.Sync(radius);
	stream.Sync(material);

	stream.Sync(numConstraints);
	constraints.resize(numConstraints);
	for (uint32_t i = 0; i < numConstraints; i++)
		constraints[i].Sync(stream);
}

void bhkRagdollTemplateData::GetStringRefs(std::vector<NiStringRef*>& refs) {
	NiObject::GetStringRefs(refs);

	refs.emplace_back(&name);
}

std::vector<ConstraintData> bhkRagdollTemplateData::GetConstraints() const {
	return constraints;
}

void bhkRagdollTemplateData::SetConstraints(const std::vector<ConstraintData>& cs) {
	numConstraints = cs.size();
	constraints = cs;
}
