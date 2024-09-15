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

void bhkNiCollisionObject::GetChildIndices(std::vector<uint32_t>& indices) {
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
	data.resize(size);
}

void bhkPhysicsSystem::Sync(NiStreamReversible& stream) {
	data.SyncByteArray(stream);
}


bhkRagdollSystem::bhkRagdollSystem(const uint32_t size) {
	data.resize(size);
}

void bhkRagdollSystem::Sync(NiStreamReversible& stream) {
	data.SyncByteArray(stream);
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

void bhkConvexListShape::GetChildIndices(std::vector<uint32_t>& indices) {
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


void bhkCylinderShape::Sync(NiStreamReversible& stream) {
	stream.Sync(reinterpret_cast<char*>(unused1), 8);
	stream.Sync(vertexA);
	stream.Sync(vertexB);
	stream.Sync(cylinderRadius);
	stream.Sync(reinterpret_cast<char*>(unused2), 12);
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

void bhkTransformShape::GetChildIndices(std::vector<uint32_t>& indices) {
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
	uint32_t sz = data.SyncSize(stream);
	stream.Sync(offset);

	if (stream.GetVersion().User() >= 12)
		stream.Sync(buildType);

	data.SyncData(stream, sz);
}

void bhkMoppBvTreeShape::GetChildRefs(std::set<NiRef*>& refs) {
	bhkBvTreeShape::GetChildRefs(refs);

	refs.insert(&shapeRef);
}

void bhkMoppBvTreeShape::GetChildIndices(std::vector<uint32_t>& indices) {
	bhkBvTreeShape::GetChildIndices(indices);

	indices.push_back(shapeRef.index);
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

void bhkNiTriStripsShape::GetChildIndices(std::vector<uint32_t>& indices) {
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

void bhkListShape::GetChildIndices(std::vector<uint32_t>& indices) {
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

void bhkPackedNiTriStripsShape::GetChildIndices(std::vector<uint32_t>& indices) {
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

void bhkWorldObject::GetChildIndices(std::vector<uint32_t>& indices) {
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

	if (stream.GetVersion().Stream() > 34) {
		if (stream.GetVersion().Stream() < 130)
			stream.Sync(timeFactor);

		stream.Sync(gravityFactor);
	}

	stream.Sync(friction);

	if (stream.GetVersion().Stream() > 34)
		stream.Sync(rollingFrictionMult);

	stream.Sync(restitution);
	stream.Sync(maxLinearVelocity);
	stream.Sync(maxAngularVelocity);
	stream.Sync(penetrationDepth);
	stream.Sync(motionSystem);
	stream.Sync(deactivatorType);
	stream.Sync(solverDeactivation);
	stream.Sync(qualityType);

	if (stream.GetVersion().Stream() > 34) {
		stream.Sync(autoRemoveLevel);
		stream.Sync(responseModifierFlag);
		stream.Sync(numShapeKeysInContactPointProps);
		stream.Sync(forceCollideOntoPpu);
	}

	if (stream.GetVersion().IsFO4())
		stream.Sync(reinterpret_cast<char*>(unusedBytes2), 3);
	else
		stream.Sync(reinterpret_cast<char*>(unusedInts1), 12);

	constraintRefs.Sync(stream);

	if (stream.GetVersion().Stream() < 76)
		stream.Sync(bodyFlagsInt);
	else
		stream.Sync(bodyFlags);
}

void bhkRigidBody::GetChildRefs(std::set<NiRef*>& refs) {
	bhkEntity::GetChildRefs(refs);

	constraintRefs.GetIndexPtrs(refs);
}

void bhkRigidBody::GetChildIndices(std::vector<uint32_t>& indices) {
	bhkEntity::GetChildIndices(indices);

	constraintRefs.GetIndices(indices);
}


void bhkConstraint::Sync(NiStreamReversible& stream) {
	entityRefs.SetKeepEmptyRefs();
	entityRefs.SetSize(2);
	entityRefs.Sync(stream);

	stream.Sync(priority);
}

void bhkConstraint::GetPtrs(std::set<NiPtr*>& ptrs) {
	bhkSerializable::GetPtrs(ptrs);

	entityRefs.GetIndexPtrs(ptrs);
}


void bhkHingeConstraint::Sync(NiStreamReversible& stream) {
	hinge.Sync(stream);
}


void bhkLimitedHingeConstraint::Sync(NiStreamReversible& stream) {
	limitedHinge.Sync(stream);
}


void ConstraintData::Sync(NiStreamReversible& stream) {
	stream.Sync(type);

	entityRefs.SetKeepEmptyRefs();
	entityRefs.SetSize(2);
	entityRefs.Sync(stream);
	stream.Sync(priority);

	switch (type) {
		case BallAndSocket: stream.Sync(reinterpret_cast<char*>(&desc1), 32); break;
		case Hinge: desc2.Sync(stream); break;
		case LimitedHinge: desc3.Sync(stream); break;
		case Prismatic: desc4.Sync(stream); break;
		case Ragdoll: desc5.Sync(stream); break;
		case StiffSpring: stream.Sync(reinterpret_cast<char*>(&desc6), 36); break;
	}

	if (stream.GetVersion().File() <= NiFileVersion::V20_0_0_5) {
		stream.Sync(tau);
		stream.Sync(damping);
	}
	else if (stream.GetVersion().File() >= NiFileVersion::V20_2_0_7)
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
	if (stream.GetVersion().Stream() <= 16) {
		// OB/FO3
		stream.Sync(ragdoll.pivotA);
		stream.Sync(ragdoll.planeA);
		stream.Sync(ragdoll.twistA);
		stream.Sync(ragdoll.pivotB);
		stream.Sync(ragdoll.planeB);
		stream.Sync(ragdoll.twistB);
	}
	else {
		// FO3 and later
		stream.Sync(ragdoll.twistA);
		stream.Sync(ragdoll.planeA);
		stream.Sync(ragdoll.motorA);
		stream.Sync(ragdoll.pivotA);
		stream.Sync(ragdoll.twistB);
		stream.Sync(ragdoll.planeB);
		stream.Sync(ragdoll.motorB);
		stream.Sync(ragdoll.pivotB);
	}

	stream.Sync(ragdoll.coneMaxAngle);
	stream.Sync(ragdoll.planeMinAngle);
	stream.Sync(ragdoll.planeMaxAngle);
	stream.Sync(ragdoll.twistMinAngle);
	stream.Sync(ragdoll.twistMaxAngle);
	stream.Sync(ragdoll.maxFriction);

	if (stream.GetVersion().Stream() > 16)
		ragdoll.motorDesc.Sync(stream);
}


void bhkStiffSpringConstraint::Sync(NiStreamReversible& stream) {
	stream.Sync(stiffSpring.pivotA);
	stream.Sync(stiffSpring.pivotB);
	stream.Sync(stiffSpring.length);
}


void bhkPrismaticConstraint::Sync(NiStreamReversible& stream) {
	prismatic.Sync(stream);
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

	bigTris.Sync(stream);
	chunks.Sync(stream);

	stream.Sync(numConvexPieceA);
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

void bhkCompressedMeshShape::GetChildIndices(std::vector<uint32_t>& indices) {
	bhkShape::GetChildIndices(indices);

	indices.push_back(dataRef.index);
}

void bhkCompressedMeshShape::GetPtrs(std::set<NiPtr*>& ptrs) {
	bhkShape::GetPtrs(ptrs);

	ptrs.insert(&targetRef);
}


void bhkPoseArray::Sync(NiStreamReversible& stream) {
	bones.Sync(stream);
	poses.Sync(stream);
}

void bhkPoseArray::GetStringRefs(std::vector<NiStringRef*>& refs) {
	NiObject::GetStringRefs(refs);

	for (auto& b : bones)
		refs.emplace_back(&b);
}


void bhkRagdollTemplate::Sync(NiStreamReversible& stream) {
	boneRefs.Sync(stream);
}

void bhkRagdollTemplate::GetChildRefs(std::set<NiRef*>& refs) {
	NiExtraData::GetChildRefs(refs);

	boneRefs.GetIndexPtrs(refs);
}

void bhkRagdollTemplate::GetChildIndices(std::vector<uint32_t>& indices) {
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
	constraints.Sync(stream);
}

void bhkRagdollTemplateData::GetStringRefs(std::vector<NiStringRef*>& refs) {
	NiObject::GetStringRefs(refs);

	refs.emplace_back(&name);
}
