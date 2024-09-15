/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#pragma once

#include "Animation.hpp"
#include "BasicTypes.hpp"
#include "ExtraData.hpp"

namespace nifly {
using HavokMaterial = uint32_t;

struct HavokFilter {
	uint8_t layer = 1;
	uint8_t flagsAndParts = 0;
	uint16_t group = 0;
};

struct hkWorldObjCInfoProperty {
	uint32_t data = 0;
	uint32_t size = 0;
	uint32_t capacityAndFlags = 0x80000000;
};

enum MotorType : uint8_t { MOTOR_NONE = 0, MOTOR_POSITION = 1, MOTOR_VELOCITY = 2, MOTOR_SPRING = 3 };

struct bhkLimitedForceConstraintMotor {
	float minForce = -1000000.0f;
	float maxForce = 1000000.0f;
	bool motorEnabled = false;
};

struct bhkPositionConstraintMotor : bhkLimitedForceConstraintMotor {
	float tau = 0.8f;
	float damping = 1.0f;
	float proportionalRecoveryVelocity = 2.0f;
	float constantRecoveryVelocity = 1.0f;

	void Sync(NiStreamReversible& stream) {
		stream.Sync(minForce);
		stream.Sync(maxForce);
		stream.Sync(tau);
		stream.Sync(damping);
		stream.Sync(proportionalRecoveryVelocity);
		stream.Sync(constantRecoveryVelocity);
		stream.Sync(motorEnabled);
	}
};

struct bhkVelocityConstraintMotor : bhkLimitedForceConstraintMotor {
	float tau = 0.0f;
	float velocityTarget = 0.0f;
	bool useVelocityTargetFromConstraintTargets = 0.0f;

	void Sync(NiStreamReversible& stream) {
		stream.Sync(minForce);
		stream.Sync(maxForce);
		stream.Sync(tau);
		stream.Sync(velocityTarget);
		stream.Sync(useVelocityTargetFromConstraintTargets);
		stream.Sync(motorEnabled);
	}
};

struct bhkSpringDamperConstraintMotor : bhkLimitedForceConstraintMotor {
	float springConstant = 0.0f;
	float springDamping = 0.0f;

	void Sync(NiStreamReversible& stream) {
		stream.Sync(minForce);
		stream.Sync(maxForce);
		stream.Sync(springConstant);
		stream.Sync(springDamping);
		stream.Sync(motorEnabled);
	}
};

struct MotorDesc {
	MotorType motorType = MOTOR_NONE;
	bhkPositionConstraintMotor motorPosition;
	bhkVelocityConstraintMotor motorVelocity;
	bhkSpringDamperConstraintMotor motorSpringDamper;

	void Sync(NiStreamReversible& stream) {
		stream.Sync(motorType);

		switch (motorType) {
			case MOTOR_POSITION: motorPosition.Sync(stream); break;
			case MOTOR_VELOCITY: motorVelocity.Sync(stream); break;
			case MOTOR_SPRING: motorSpringDamper.Sync(stream); break;
			case MOTOR_NONE: break;
		}
	}
};

struct HingeDesc {
	Vector4 axleA;
	Vector4 axleInA1;
	Vector4 axleInA2;
	Vector4 pivotA;
	Vector4 axleB;
	Vector4 axleInB1;
	Vector4 axleInB2;
	Vector4 pivotB;

	void Sync(NiStreamReversible& stream) {
		if (stream.GetVersion().File() <= NiFileVersion::V20_0_0_5) {
			stream.Sync(pivotA);
			stream.Sync(axleInA1);
			stream.Sync(axleInA2);
			stream.Sync(pivotB);
			stream.Sync(axleB);
		}
		else if (stream.GetVersion().File() >= NiFileVersion::V20_2_0_7) {
			stream.Sync(axleA);
			stream.Sync(axleInA1);
			stream.Sync(axleInA2);
			stream.Sync(pivotA);
			stream.Sync(axleB);
			stream.Sync(axleInB1);
			stream.Sync(axleInB2);
			stream.Sync(pivotB);
		}
	}
};

struct LimitedHingeDesc {
	Vector4 axleA;
	Vector4 axleInA1;
	Vector4 axleInA2;
	Vector4 pivotA;
	Vector4 axleB;
	Vector4 axleInB1;
	Vector4 axleInB2;
	Vector4 pivotB;
	float minAngle = 0.0f;
	float maxAngle = 0.0f;
	float maxFriction = 0.0f;
	MotorDesc motorDesc;

	void Sync(NiStreamReversible& stream) {
		if (stream.GetVersion().Stream() <= 16) {
			stream.Sync(pivotA);
			stream.Sync(axleA);
			stream.Sync(axleInA1);
			stream.Sync(axleInA2);
			stream.Sync(pivotB);
			stream.Sync(axleB);
			stream.Sync(axleInB2);
		}
		else {
			stream.Sync(axleA);
			stream.Sync(axleInA1);
			stream.Sync(axleInA2);
			stream.Sync(pivotA);
			stream.Sync(axleB);
			stream.Sync(axleInB1);
			stream.Sync(axleInB2);
			stream.Sync(pivotB);
		}

		stream.Sync(minAngle);
		stream.Sync(maxAngle);
		stream.Sync(maxFriction);

		if (stream.GetVersion().File() >= NiFileVersion::V20_2_0_7 && stream.GetVersion().Stream() > 16)
			motorDesc.Sync(stream);
	}
};

struct RagdollDesc {
	Vector4 twistA;
	Vector4 planeA;
	Vector4 motorA;
	Vector4 pivotA;
	Vector4 twistB;
	Vector4 planeB;
	Vector4 motorB;
	Vector4 pivotB;
	float coneMaxAngle = 0.0f;
	float planeMinAngle = 0.0f;
	float planeMaxAngle = 0.0f;
	float twistMinAngle = 0.0f;
	float twistMaxAngle = 0.0f;
	float maxFriction = 0.0f;
	MotorDesc motorDesc;

	void Sync(NiStreamReversible& stream) {
		if (stream.GetVersion().Stream() <= 16) {
			stream.Sync(pivotA);
			stream.Sync(planeA);
			stream.Sync(twistA);
			stream.Sync(pivotB);
			stream.Sync(planeB);
			stream.Sync(twistB);
		}
		else {
			stream.Sync(twistA);
			stream.Sync(planeA);
			stream.Sync(motorA);
			stream.Sync(pivotA);
			stream.Sync(twistB);
			stream.Sync(planeB);
			stream.Sync(motorB);
			stream.Sync(pivotB);
		}

		stream.Sync(coneMaxAngle);
		stream.Sync(planeMinAngle);
		stream.Sync(planeMaxAngle);
		stream.Sync(twistMinAngle);
		stream.Sync(twistMaxAngle);
		stream.Sync(maxFriction);

		if (stream.GetVersion().File() >= NiFileVersion::V20_2_0_7 && stream.GetVersion().Stream() > 16)
			motorDesc.Sync(stream);
	}
};

struct StiffSpringDesc {
	Vector4 pivotA;
	Vector4 pivotB;
	float length = 0.0f;
};

struct BallAndSocketDesc {
	Vector4 translationA;
	Vector4 translationB;
};

struct PrismaticDesc {
	Vector4 slidingA;
	Vector4 rotationA;
	Vector4 planeA;
	Vector4 pivotA;
	Vector4 slidingB;
	Vector4 rotationB;
	Vector4 planeB;
	Vector4 pivotB;
	float minDistance = 0.0f;
	float maxDistance = 0.0f;
	float friction = 0.0f;
	MotorDesc motorDesc;

	void Sync(NiStreamReversible& stream) {
		if (stream.GetVersion().File() <= NiFileVersion::V20_0_0_5) {
			stream.Sync(pivotA);
			stream.Sync(rotationA);
			stream.Sync(planeA);
			stream.Sync(slidingA);
			stream.Sync(slidingB);
			stream.Sync(pivotB);
			stream.Sync(rotationB);
			stream.Sync(planeB);
		}
		else if (stream.GetVersion().File() >= NiFileVersion::V20_2_0_7) {
			stream.Sync(slidingA);
			stream.Sync(rotationA);
			stream.Sync(planeA);
			stream.Sync(pivotA);
			stream.Sync(slidingB);
			stream.Sync(rotationB);
			stream.Sync(planeB);
			stream.Sync(pivotB);
		}

		stream.Sync(minDistance);
		stream.Sync(maxDistance);
		stream.Sync(friction);

		if (stream.GetVersion().File() >= NiFileVersion::V20_2_0_7 && stream.GetVersion().Stream() > 16)
			motorDesc.Sync(stream);
	}
};

enum hkConstraintType : uint32_t {
	BallAndSocket = 0,
	Hinge = 1,
	LimitedHinge = 2,
	Prismatic = 6,
	Ragdoll = 7,
	StiffSpring = 8
};

struct bhkCMSDMaterial {
	HavokMaterial material = 0;
	HavokFilter layer;
};

class bhkCMSDBigTris {
public:
	uint16_t triangle1 = 0;
	uint16_t triangle2 = 0;
	uint16_t triangle3 = 0;
	HavokMaterial material = 0;
	uint16_t weldingInfo = 0;

	void Sync(NiStreamReversible& stream) {
		stream.Sync(triangle1);
		stream.Sync(triangle2);
		stream.Sync(triangle3);
		stream.Sync(material);
		stream.Sync(weldingInfo);
	}
};

struct bhkCMSDTransform {
	Vector4 translation;
	QuaternionXYZW rotation;
};

class bhkCMSDChunk {
public:
	Vector4 translation;
	uint32_t matIndex = 0;
	uint16_t reference = 0;
	uint16_t transformIndex = 0;

	NiVector<uint16_t> verts;
	NiVector<uint16_t> indices;
	NiVector<uint16_t> strips;
	NiVector<uint16_t> weldingInfo;

	void Sync(NiStreamReversible& stream) {
		stream.Sync(translation);
		stream.Sync(matIndex);
		stream.Sync(reference);
		stream.Sync(transformIndex);

		verts.Sync(stream);
		indices.Sync(stream);
		strips.Sync(stream);
		weldingInfo.Sync(stream);
	}
};

class NiAVObject;

class NiCollisionObject : public NiCloneableStreamable<NiCollisionObject, NiObject> {
public:
	NiBlockPtr<NiAVObject> targetRef;

	static constexpr const char* BlockName = "NiCollisionObject";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<NiPtr*>& ptrs) override;
};

enum PropagationMode : uint32_t {
	PROPAGATE_ON_SUCCESS,
	PROPAGATE_ON_FAILURE,
	PROPAGATE_ALWAYS,
	PROPAGATE_NEVER
};

enum CollisionMode : uint32_t { CM_USE_OBB, CM_USE_TRI, CM_USE_ABV, CM_NOTEST, CM_USE_NIBOUND };

enum BoundVolumeType : uint32_t {
	BASE_BV = 0xFFFFFFFF,
	SPHERE_BV = 0,
	BOX_BV = 1,
	CAPSULE_BV = 2,
	UNION_BV = 4,
	HALFSPACE_BV = 5
};

struct BoxBV {
	Vector3 center;
	Vector3 axis1;
	Vector3 axis2;
	Vector3 axis3;
	float extent1 = 0.0f;
	float extent2 = 0.0f;
	float extent3 = 0.0f;
};

struct CapsuleBV {
	Vector3 center;
	Vector3 origin;
	float extent = 0.0f;
	float radius = 0.0f;
};

struct HalfSpaceBV {
	NiPlane plane;
	Vector3 center;
};

struct UnionBV;

struct BoundingVolume {
	BoundVolumeType collisionType = BASE_BV;
	BoundingSphere bvSphere;
	BoxBV bvBox;
	CapsuleBV bvCapsule;
	std::unique_ptr<UnionBV> bvUnion = std::make_unique<UnionBV>();
	HalfSpaceBV bvHalfSpace;

	BoundingVolume() = default;

	BoundingVolume(const BoundingVolume& other)
		: collisionType(other.collisionType)
		, bvSphere(other.bvSphere)
		, bvBox(other.bvBox)
		, bvCapsule(other.bvCapsule)
		, bvUnion(std::make_unique<UnionBV>(*other.bvUnion))
		, bvHalfSpace(other.bvHalfSpace) {}

	void Sync(NiStreamReversible& stream);
};

struct UnionBV {
	uint32_t numBV = 0;
	std::vector<BoundingVolume> boundingVolumes;

	void Sync(NiStreamReversible& stream) {
		stream.Sync(numBV);
		boundingVolumes.resize(numBV);
		for (uint32_t i = 0; i < numBV; i++)
			boundingVolumes[i].Sync(stream);
	}
};

class NiCollisionData : public NiCloneableStreamable<NiCollisionData, NiCollisionObject> {
public:
	PropagationMode propagationMode = PROPAGATE_ON_SUCCESS;
	CollisionMode collisionMode = CM_USE_OBB;
	bool useABV = false;
	BoundingVolume boundingVolume;

	static constexpr const char* BlockName = "NiCollisionData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class bhkNiCollisionObject : public NiCloneableStreamable<bhkNiCollisionObject, NiCollisionObject> {
public:
	uint16_t flags = 1;
	NiBlockRef<NiObject> bodyRef;

	static constexpr const char* BlockName = "bhkNiCollisionObject";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class bhkCollisionObject : public NiCloneable<bhkCollisionObject, bhkNiCollisionObject> {
public:
	static constexpr const char* BlockName = "bhkCollisionObject";
	const char* GetBlockName() override { return BlockName; }
};

class bhkNPCollisionObject : public NiCloneableStreamable<bhkNPCollisionObject, bhkCollisionObject> {
public:
	uint32_t bodyID = 0;

	static constexpr const char* BlockName = "bhkNPCollisionObject";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class bhkPCollisionObject : public NiCloneable<bhkPCollisionObject, bhkNiCollisionObject> {
public:
	static constexpr const char* BlockName = "bhkPCollisionObject";
	const char* GetBlockName() override { return BlockName; }
};

class bhkSPCollisionObject : public NiCloneable<bhkSPCollisionObject, bhkPCollisionObject> {
public:
	static constexpr const char* BlockName = "bhkSPCollisionObject";
	const char* GetBlockName() override { return BlockName; }
};

class bhkBlendCollisionObject : public NiCloneableStreamable<bhkBlendCollisionObject, bhkCollisionObject> {
public:
	float heirGain = 0.0f;
	float velGain = 0.0f;

	static constexpr const char* BlockName = "bhkBlendCollisionObject";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class bhkPhysicsSystem : public NiCloneableStreamable<bhkPhysicsSystem, BSExtraData> {
public:
	NiVector<char> data;

	bhkPhysicsSystem(const uint32_t size = 0);

	static constexpr const char* BlockName = "bhkPhysicsSystem";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class bhkRagdollSystem : public NiCloneableStreamable<bhkRagdollSystem, BSExtraData> {
public:
	NiVector<char> data;

	bhkRagdollSystem(const uint32_t size = 0);

	static constexpr const char* BlockName = "bhkRagdollSystem";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class bhkBlendController : public NiCloneableStreamable<bhkBlendController, NiTimeController> {
public:
	uint32_t keys = 0;

	static constexpr const char* BlockName = "bhkBlendController";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class bhkRefObject : public NiCloneable<bhkRefObject, NiObject> {};

class bhkSerializable : public NiCloneable<bhkSerializable, bhkRefObject> {};

class bhkShape : public NiCloneable<bhkShape, bhkSerializable> {
public:
	virtual HavokMaterial GetMaterial() const { return 0; }
	virtual void SetMaterial(HavokMaterial) {}
};

class bhkHeightFieldShape : public NiCloneableStreamable<bhkHeightFieldShape, bhkShape> {
protected:
	HavokMaterial material = 0;

public:
	void Sync(NiStreamReversible& stream);

	HavokMaterial GetMaterial() const override { return material; }
	void SetMaterial(HavokMaterial mat) override { material = mat; }
};

class bhkPlaneShape : public NiCloneableStreamable<bhkPlaneShape, bhkHeightFieldShape> {
protected:
public:
	Vector3 unkVec;
	NiPlane plane;
	Vector4 halfExtents;
	Vector4 center;

	static constexpr const char* BlockName = "bhkPlaneShape";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class bhkSphereRepShape : public NiCloneableStreamable<bhkSphereRepShape, bhkShape> {
protected:
	HavokMaterial material = 0;

public:
	void Sync(NiStreamReversible& stream);

	HavokMaterial GetMaterial() const override { return material; }
	void SetMaterial(HavokMaterial mat) override { material = mat; }
};

class bhkConvexShape : public NiCloneableStreamable<bhkConvexShape, bhkSphereRepShape> {
public:
	float radius = 0.0f;

	void Sync(NiStreamReversible& stream);
};

class bhkMultiSphereShape : public NiCloneableStreamable<bhkMultiSphereShape, bhkSphereRepShape> {
public:
	hkWorldObjCInfoProperty shapeProperty;
	NiVector<BoundingSphere> spheres;

	static constexpr const char* BlockName = "bhkMultiSphereShape";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class bhkConvexListShape : public NiCloneableStreamable<bhkConvexListShape, bhkShape> {
public:
	NiBlockRefArray<bhkConvexShape> shapeRefs;
	HavokMaterial material = 0;
	float radius = 0.0f;
	uint32_t unkInt1 = 0;
	float unkFloat1 = 0.0f;
	hkWorldObjCInfoProperty childShapeProp;
	bool useCachedAABB = false;
	float closestPointMinDistance = 0.0f;

	static constexpr const char* BlockName = "bhkConvexListShape";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class bhkConvexVerticesShape : public NiCloneableStreamable<bhkConvexVerticesShape, bhkConvexShape> {
public:
	hkWorldObjCInfoProperty vertsProp;
	hkWorldObjCInfoProperty normalsProp;

	NiVector<Vector4> verts;
	NiVector<Vector4> normals;

	static constexpr const char* BlockName = "bhkConvexVerticesShape";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class bhkBoxShape : public NiCloneableStreamable<bhkBoxShape, bhkConvexShape> {
private:
	uint64_t padding = 0;

public:
	Vector3 dimensions;
	float radius2 = 0.0f;

	static constexpr const char* BlockName = "bhkBoxShape";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class bhkSphereShape : public NiCloneable<bhkSphereShape, bhkConvexShape> {
public:
	static constexpr const char* BlockName = "bhkSphereShape";
	const char* GetBlockName() override { return BlockName; }
};

class bhkCylinderShape : public NiCloneableStreamable<bhkCylinderShape, bhkConvexShape> {
private:
	uint8_t unused1[8]{};
	uint8_t unused2[12]{};

public:
	Vector4 vertexA;
	Vector4 vertexB;
	float cylinderRadius = 0.0f;

	static constexpr const char* BlockName = "bhkCylinderShape";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class bhkTransformShape : public NiCloneableStreamable<bhkTransformShape, bhkShape> {
private:
	uint64_t padding = 0;

public:
	NiBlockRef<bhkShape> shapeRef;
	HavokMaterial material = 0;
	float radius = 0.0f;
	Matrix4 xform;

	static constexpr const char* BlockName = "bhkTransformShape";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class bhkConvexTransformShape : public NiCloneable<bhkConvexTransformShape, bhkTransformShape> {
public:
	static constexpr const char* BlockName = "bhkConvexTransformShape";
	const char* GetBlockName() override { return BlockName; }
};

class bhkCapsuleShape : public NiCloneableStreamable<bhkCapsuleShape, bhkConvexShape> {
private:
	uint64_t padding = 0;

public:
	Vector3 point1;
	float radius1 = 0.0f;
	Vector3 point2;
	float radius2 = 0.0f;

	static constexpr const char* BlockName = "bhkCapsuleShape";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class bhkBvTreeShape : public NiCloneable<bhkBvTreeShape, bhkShape> {};

class bhkMoppBvTreeShape : public NiCloneableStreamable<bhkMoppBvTreeShape, bhkBvTreeShape> {
public:
	NiBlockRef<bhkShape> shapeRef;
	uint32_t userData = 0;
	uint32_t shapeCollection = 0;
	uint32_t code = 0;
	float scale = 0.0f;
	NiVector<uint8_t> data;
	Vector4 offset;
	uint8_t buildType = 2; // User Version >= 12

	static constexpr const char* BlockName = "bhkMoppBvTreeShape";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class NiTriStripsData;

class bhkNiTriStripsShape : public NiCloneableStreamable<bhkNiTriStripsShape, bhkShape> {
protected:
	HavokMaterial material = 0;

public:
	float radius = 0.1f;
	uint32_t unused1 = 0;
	uint32_t unused2 = 0;
	uint32_t unused3 = 0;
	uint32_t unused4 = 0;
	uint32_t unused5 = 0;
	uint32_t growBy = 1;
	Vector4 scale = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	NiBlockRefArray<NiTriStripsData> partRefs;
	NiVector<uint32_t> filters;

	static constexpr const char* BlockName = "bhkNiTriStripsShape";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;

	HavokMaterial GetMaterial() const override { return material; }
	void SetMaterial(HavokMaterial mat) override { material = mat; }
};

class bhkShapeCollection : public NiCloneable<bhkShapeCollection, bhkShape> {};

class bhkListShape : public NiCloneableStreamable<bhkListShape, bhkShapeCollection> {
protected:
	HavokMaterial material = 0;

public:
	NiBlockRefArray<bhkShape> subShapeRefs;
	hkWorldObjCInfoProperty childShapeProp;
	hkWorldObjCInfoProperty childFilterProp;
	NiVector<HavokFilter> filters;

	static constexpr const char* BlockName = "bhkListShape";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;

	HavokMaterial GetMaterial() const override { return material; }
	void SetMaterial(HavokMaterial mat) override { material = mat; }
};

struct hkTriangleData {
	Triangle tri;
	uint16_t weldingInfo = 0;
};

struct hkTriangleNormalData {
	Triangle tri;
	uint16_t weldingInfo = 0;
	Vector3 normal;
};

struct hkSubPartData {
	HavokFilter filter;
	uint32_t numVerts = 0;
	HavokMaterial material = 0;
};

class hkPackedNiTriStripsData : public NiCloneableStreamable<hkPackedNiTriStripsData, bhkShapeCollection> {
public:
	uint32_t keyCount = 0;
	std::vector<hkTriangleData> triData;
	std::vector<hkTriangleNormalData> triNormData;

	uint32_t numVerts = 0;
	bool compressed = false;
	std::vector<Vector3> compressedVertData;

	NiVector<hkSubPartData, uint16_t> subPartData;

	static constexpr const char* BlockName = "hkPackedNiTriStripsData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class bhkPackedNiTriStripsShape
	: public NiCloneableStreamable<bhkPackedNiTriStripsShape, bhkShapeCollection> {
private:
	uint32_t unused1 = 0;
	uint32_t unused2 = 0;

public:
	NiVector<hkSubPartData, uint16_t> subPartData;

	uint32_t userData = 0;
	float radius = 0.0f;
	Vector4 scaling;
	float radius2 = 0.0f;
	Vector4 scaling2;
	NiBlockRef<hkPackedNiTriStripsData> dataRef;

	static constexpr const char* BlockName = "bhkPackedNiTriStripsShape";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class bhkLiquidAction : public NiCloneableStreamable<bhkLiquidAction, bhkSerializable> {
public:
	uint32_t userData = 0;
	uint32_t unkInt1 = 0;
	uint32_t unkInt2 = 0;
	float initialStickForce = 0.0f;
	float stickStrength = 0.0f;
	float neighborDistance = 0.0f;
	float neighborStrength = 0.0f;

	static constexpr const char* BlockName = "bhkLiquidAction";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class bhkOrientHingedBodyAction : public NiCloneableStreamable<bhkOrientHingedBodyAction, bhkSerializable> {
private:
	uint64_t padding = 0;
	uint64_t padding2 = 0;

public:
	NiBlockPtr<NiObject> bodyRef;
	uint32_t unkInt1 = 0;
	uint32_t unkInt2 = 0;
	Vector4 hingeAxisLS;
	Vector4 forwardLS;
	float strength = 0.0f;
	float damping = 0.0f;

	static constexpr const char* BlockName = "bhkOrientHingedBodyAction";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<NiPtr*>& ptrs) override;
};

class bhkWorldObject : public NiCloneableStreamable<bhkWorldObject, bhkSerializable> {
public:
	NiBlockRef<bhkShape> shapeRef;
	HavokFilter collisionFilter;
	int unkInt1 = 0;
	uint8_t broadPhaseType = 0;
	uint8_t unkBytes[3]{};
	hkWorldObjCInfoProperty prop;

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class bhkPhantom : public NiCloneable<bhkPhantom, bhkWorldObject> {};

class bhkShapePhantom : public NiCloneable<bhkShapePhantom, bhkPhantom> {};

class bhkSimpleShapePhantom : public NiCloneableStreamable<bhkSimpleShapePhantom, bhkShapePhantom> {
private:
	uint64_t padding = 0;

public:
	Matrix4 transform;

	static constexpr const char* BlockName = "bhkSimpleShapePhantom";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class bhkAabbPhantom : public NiCloneableStreamable<bhkAabbPhantom, bhkShapePhantom> {
private:
	uint64_t padding = 0;

public:
	Vector4 aabbMin;
	Vector4 aabbMax;

	static constexpr const char* BlockName = "bhkAabbPhantom";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class bhkEntity : public NiCloneable<bhkEntity, bhkWorldObject> {};

enum hkResponseType : uint8_t {
	RESPONSE_INVALID,
	RESPONSE_SIMPLE_CONTACT,
	RESPONSE_REPORTING,
	RESPONSE_NONE
};

class bhkRigidBody : public NiCloneableStreamable<bhkRigidBody, bhkEntity> {
public:
	hkResponseType collisionResponse = RESPONSE_SIMPLE_CONTACT;
	uint8_t unusedByte1 = 0;
	uint16_t processContactCallbackDelay = 0xFFFF;
	uint32_t unkInt1 = 0;
	HavokFilter collisionFilterCopy;
	uint16_t unkShorts2[6]{};
	Vector4 translation;
	QuaternionXYZW rotation;
	Vector4 linearVelocity;
	Vector4 angularVelocity;
	float inertiaMatrix[12]{};
	Vector4 center;
	float mass = 1.0f;
	float linearDamping = 0.1f;
	float angularDamping = 0.05f;
	float timeFactor = 1.0f;	// User Version >= 12
	float gravityFactor = 1.0f; // User Version >= 12
	float friction = 0.5f;
	float rollingFrictionMult = 1.0f; // User Version >= 12
	float restitution = 0.4f;
	float maxLinearVelocity = 104.4f;
	float maxAngularVelocity = 31.57f;
	float penetrationDepth = 0.15f;
	uint8_t motionSystem = 1;
	uint8_t deactivatorType = 1;
	uint8_t solverDeactivation = 1;
	uint8_t qualityType = 1;
	uint8_t autoRemoveLevel = 0;
	uint8_t responseModifierFlag = 0;
	uint8_t numShapeKeysInContactPointProps = 0;
	bool forceCollideOntoPpu = false;
	uint32_t unusedInts1[3]{};
	uint8_t unusedBytes2[3]{};
	NiBlockRefArray<bhkSerializable> constraintRefs;
	uint32_t bodyFlagsInt = 0;
	uint16_t bodyFlags = 0;

	static constexpr const char* BlockName = "bhkRigidBody";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class bhkRigidBodyT : public NiCloneable<bhkRigidBodyT, bhkRigidBody> {
public:
	static constexpr const char* BlockName = "bhkRigidBodyT";
	const char* GetBlockName() override { return BlockName; }
};

class bhkConstraint : public NiCloneableStreamable<bhkConstraint, bhkSerializable> {
public:
	NiBlockPtrArray<bhkEntity> entityRefs;
	uint32_t priority = 0;

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<NiPtr*>& ptrs) override;
};

class bhkHingeConstraint : public NiCloneableStreamable<bhkHingeConstraint, bhkConstraint> {
public:
	HingeDesc hinge;

	static constexpr const char* BlockName = "bhkHingeConstraint";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class bhkLimitedHingeConstraint : public NiCloneableStreamable<bhkLimitedHingeConstraint, bhkConstraint> {
public:
	LimitedHingeDesc limitedHinge;

	static constexpr const char* BlockName = "bhkLimitedHingeConstraint";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class ConstraintData {
public:
	hkConstraintType type = BallAndSocket;
	NiBlockRefArray<bhkEntity> entityRefs;
	uint32_t priority = 1;

	BallAndSocketDesc desc1;
	HingeDesc desc2;
	LimitedHingeDesc desc3;
	PrismaticDesc desc4;
	RagdollDesc desc5;
	StiffSpringDesc desc6;

	float tau = 0.0f;
	float damping = 0.0f;
	float strength = 0.0f;

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<NiPtr*>& ptrs);
};

class bhkBreakableConstraint : public NiCloneableStreamable<bhkBreakableConstraint, bhkConstraint> {
public:
	ConstraintData subConstraint;
	bool removeWhenBroken = false;

	static constexpr const char* BlockName = "bhkBreakableConstraint";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<NiPtr*>& ptrs) override;
};

class bhkRagdollConstraint : public NiCloneableStreamable<bhkRagdollConstraint, bhkConstraint> {
public:
	RagdollDesc ragdoll;

	static constexpr const char* BlockName = "bhkRagdollConstraint";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class bhkStiffSpringConstraint : public NiCloneableStreamable<bhkStiffSpringConstraint, bhkConstraint> {
public:
	StiffSpringDesc stiffSpring;

	static constexpr const char* BlockName = "bhkStiffSpringConstraint";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class bhkPrismaticConstraint : public NiCloneableStreamable<bhkPrismaticConstraint, bhkConstraint> {
public:
	PrismaticDesc prismatic;

	static constexpr const char* BlockName = "bhkPrismaticConstraint";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class bhkMalleableConstraint : public NiCloneableStreamable<bhkMalleableConstraint, bhkConstraint> {
public:
	ConstraintData subConstraint;

	static constexpr const char* BlockName = "bhkMalleableConstraint";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class bhkBallAndSocketConstraint : public NiCloneableStreamable<bhkBallAndSocketConstraint, bhkConstraint> {
public:
	BallAndSocketDesc ballAndSocket;

	static constexpr const char* BlockName = "bhkBallAndSocketConstraint";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class bhkBallSocketConstraintChain
	: public NiCloneableStreamable<bhkBallSocketConstraintChain, bhkSerializable> {
public:
	NiVector<Vector4> pivots;

	float tau = 1.0f;
	float damping = 0.6f;
	float cfm = 1.1920929e-08f;
	float maxErrorDistance = 0.1f;

	NiBlockPtrArray<bhkRigidBody> chainedEntityRefs;

	uint32_t numEntities = 2; // Always 2
	NiBlockPtr<bhkEntity> entityARef;
	NiBlockPtr<bhkEntity> entityBRef;
	uint32_t priority = 0;

	static constexpr const char* BlockName = "bhkBallSocketConstraintChain";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<NiPtr*>& ptrs) override;
};

class bhkCompressedMeshShapeData : public NiCloneableStreamable<bhkCompressedMeshShapeData, bhkRefObject> {
public:
	uint32_t bitsPerIndex = 0;
	uint32_t bitsPerWIndex = 0;
	uint32_t maskWIndex = 0;
	uint32_t maskIndex = 0;
	float error = 0.0f;
	Vector4 aabbBoundMin;
	Vector4 aabbBoundMax;
	uint8_t weldingType = 0;
	uint8_t materialType = 0;

	NiVector<uint32_t> mat32;
	NiVector<uint32_t> mat16;
	NiVector<uint32_t> mat8;

	NiVector<bhkCMSDMaterial> materials;

	uint32_t numNamedMat = 0;

	NiVector<bhkCMSDTransform> transforms;
	NiVector<Vector4> bigVerts;

	NiSyncVector<bhkCMSDBigTris> bigTris;
	NiSyncVector<bhkCMSDChunk> chunks;

	uint32_t numConvexPieceA = 0;

	static constexpr const char* BlockName = "bhkCompressedMeshShapeData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class bhkCompressedMeshShape : public NiCloneableStreamable<bhkCompressedMeshShape, bhkShape> {
public:
	NiBlockPtr<NiAVObject> targetRef;
	uint32_t userData = 0;
	float radius = 0.005f;
	float unkFloat = 0.0f;
	Vector4 scaling = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	float radius2 = 0.005f;
	Vector4 scaling2 = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	NiBlockRef<bhkCompressedMeshShapeData> dataRef;

	static constexpr const char* BlockName = "bhkCompressedMeshShape";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
	void GetPtrs(std::set<NiPtr*>& ptrs) override;
};

struct BoneMatrix {
	Vector3 translation;
	QuaternionXYZW rotation;
	Vector3 scale;
};

class BonePose {
public:
	NiVector<BoneMatrix> matrices;

	void Sync(NiStreamReversible& stream) { matrices.Sync(stream); }
};

class bhkPoseArray : public NiCloneableStreamable<bhkPoseArray, NiObject> {
public:
	NiStringRefVector<> bones;
	NiSyncVector<BonePose> poses;

	static constexpr const char* BlockName = "bhkPoseArray";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
};

class bhkRagdollTemplate : public NiCloneableStreamable<bhkRagdollTemplate, NiExtraData> {
public:
	NiBlockRefArray<NiObject> boneRefs;

	static constexpr const char* BlockName = "bhkRagdollTemplate";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class bhkRagdollTemplateData : public NiCloneableStreamable<bhkRagdollTemplateData, NiObject> {
public:
	NiStringRef name;
	float mass = 9.0f;
	float restitution = 0.8f;
	float friction = 0.3f;
	float radius = 1.0f;
	HavokMaterial material = 7;
	NiSyncVector<ConstraintData> constraints;

	static constexpr const char* BlockName = "bhkRagdollTemplateData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
};
} // namespace nifly
