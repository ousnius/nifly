/*
BodySlide and Outfit Studio
See the included LICENSE file
*/

#pragma once

#include "Animation.hpp"
#include "BasicTypes.hpp"
#include "ExtraData.hpp"

namespace nifly {
typedef uint32_t HavokMaterial;

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

	void Get(NiStream& stream) {
		stream >> minForce;
		stream >> maxForce;
		stream >> tau;
		stream >> damping;
		stream >> proportionalRecoveryVelocity;
		stream >> constantRecoveryVelocity;
		stream >> motorEnabled;
	}

	void Put(NiStream& stream) {
		stream << minForce;
		stream << maxForce;
		stream << tau;
		stream << damping;
		stream << proportionalRecoveryVelocity;
		stream << constantRecoveryVelocity;
		stream << motorEnabled;
	}
};

struct bhkVelocityConstraintMotor : bhkLimitedForceConstraintMotor {
	float tau = 0.0f;
	float velocityTarget = 0.0f;
	bool useVelocityTargetFromConstraintTargets = 0.0f;

	void Get(NiStream& stream) {
		stream >> minForce;
		stream >> maxForce;
		stream >> tau;
		stream >> velocityTarget;
		stream >> useVelocityTargetFromConstraintTargets;
		stream >> motorEnabled;
	}

	void Put(NiStream& stream) {
		stream << minForce;
		stream << maxForce;
		stream << tau;
		stream << velocityTarget;
		stream << useVelocityTargetFromConstraintTargets;
		stream << motorEnabled;
	}
};

struct bhkSpringDamperConstraintMotor : bhkLimitedForceConstraintMotor {
	float springConstant = 0.0f;
	float springDamping = 0.0f;

	void Get(NiStream& stream) {
		stream >> minForce;
		stream >> maxForce;
		stream >> springConstant;
		stream >> springDamping;
		stream >> motorEnabled;
	}

	void Put(NiStream& stream) {
		stream << minForce;
		stream << maxForce;
		stream << springConstant;
		stream << springDamping;
		stream << motorEnabled;
	}
};

struct MotorDesc {
	MotorType motorType = MOTOR_NONE;
	bhkPositionConstraintMotor motorPosition;
	bhkVelocityConstraintMotor motorVelocity;
	bhkSpringDamperConstraintMotor motorSpringDamper;

	void Get(NiStream& stream) {
		stream >> motorType;

		switch (motorType) {
			case MOTOR_POSITION: motorPosition.Get(stream); break;
			case MOTOR_VELOCITY: motorVelocity.Get(stream); break;
			case MOTOR_SPRING: motorSpringDamper.Get(stream); break;
			default: break;
		}
	}

	void Put(NiStream& stream) {
		stream << motorType;

		switch (motorType) {
			case MOTOR_POSITION: motorPosition.Put(stream); break;
			case MOTOR_VELOCITY: motorVelocity.Put(stream); break;
			case MOTOR_SPRING: motorSpringDamper.Put(stream); break;
			default: break;
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
};

struct LimitedHingeDesc {
	HingeDesc hinge;
	float minAngle = 0.0f;
	float maxAngle = 0.0f;
	float maxFriction = 0.0f;
	MotorDesc motorDesc;
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

struct bhkCMSDBigTris {
	uint16_t triangle1 = 0;
	uint16_t triangle2 = 0;
	uint16_t triangle3 = 0;
	HavokMaterial material = 0;
	uint16_t weldingInfo = 0;
};

struct bhkCMSDTransform {
	Vector4 translation;
	QuaternionXYZW rotation;
};

struct bhkCMSDChunk {
	Vector4 translation;
	uint32_t matIndex = 0;
	uint16_t reference = 0;
	uint16_t transformIndex = 0;

	uint32_t numVerts = 0;
	std::vector<uint16_t> verts;
	uint32_t numIndices = 0;
	std::vector<uint16_t> indices;
	uint32_t numStrips = 0;
	std::vector<uint16_t> strips;
	uint32_t numWeldingInfo = 0;
	std::vector<uint16_t> weldingInfo;
};

class NiAVObject;

class NiCollisionObject : public NiObject {
private:
	BlockRef<NiAVObject> targetRef;

public:
	static constexpr const char* BlockName = "NiCollisionObject";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;
	NiCollisionObject* Clone() override { return new NiCollisionObject(*this); }

	int GetTargetRef() { return targetRef.GetIndex(); }
	void SetTargetRef(const int ref) { targetRef.SetIndex(ref); }
};

enum PropagationMode : uint32_t { PROPAGATE_ON_SUCCESS, PROPAGATE_ON_FAILURE, PROPAGATE_ALWAYS, PROPAGATE_NEVER };

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
	float unkFloat1 = 0.0f;
	float unkFloat2 = 0.0f;
};

struct HalfSpaceBV {
	Vector3 normal;
	Vector3 center;
	float unkFloat1 = 0.0f;
};

struct UnionBV;

struct BoundingVolume {
	BoundVolumeType collisionType = BASE_BV;
	BoundingSphere bvSphere;
	BoxBV bvBox;
	CapsuleBV bvCapsule;
	UnionBV* bvUnion = nullptr;
	HalfSpaceBV bvHalfSpace;

	BoundingVolume();
	~BoundingVolume();

	void Get(NiStream& stream);
	void Put(NiStream& stream);
};

struct UnionBV {
	uint32_t numBV = 0;
	std::vector<BoundingVolume> boundingVolumes;

	void Get(NiStream& stream) {
		stream >> numBV;
		boundingVolumes.resize(numBV);
		for (int i = 0; i < numBV; i++)
			boundingVolumes[i].Get(stream);
	}

	void Put(NiStream& stream) {
		stream << numBV;
		for (int i = 0; i < numBV; i++)
			boundingVolumes[i].Put(stream);
	}
};

class NiCollisionData : public NiCollisionObject {
private:
	PropagationMode propagationMode = PROPAGATE_ON_SUCCESS;
	CollisionMode collisionMode = CM_USE_OBB;
	bool useABV = false;
	BoundingVolume boundingVolume;

public:
	static constexpr const char* BlockName = "NiCollisionData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiCollisionData* Clone() override { return new NiCollisionData(*this); }
};

class bhkNiCollisionObject : public NiCollisionObject {
private:
	uint16_t flags = 1;
	BlockRef<NiObject> bodyRef;

public:
	static constexpr const char* BlockName = "bhkNiCollisionObject";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	bhkNiCollisionObject* Clone() override { return new bhkNiCollisionObject(*this); }

	int GetBodyRef() { return bodyRef.GetIndex(); }
	void SetBodyRef(const int ref) { bodyRef.SetIndex(ref); }
};

class bhkCollisionObject : public bhkNiCollisionObject {
public:
	static constexpr const char* BlockName = "bhkCollisionObject";
	const char* GetBlockName() override { return BlockName; }

	bhkCollisionObject* Clone() override { return new bhkCollisionObject(*this); }
};

class bhkNPCollisionObject : public bhkCollisionObject {
private:
	uint32_t bodyID = 0;

public:
	static constexpr const char* BlockName = "bhkNPCollisionObject";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	bhkNPCollisionObject* Clone() override { return new bhkNPCollisionObject(*this); }
};

class bhkPCollisionObject : public bhkNiCollisionObject {
public:
	static constexpr const char* BlockName = "bhkPCollisionObject";
	const char* GetBlockName() override { return BlockName; }

	bhkPCollisionObject* Clone() override { return new bhkPCollisionObject(*this); }
};

class bhkSPCollisionObject : public bhkPCollisionObject {
public:
	static constexpr const char* BlockName = "bhkSPCollisionObject";
	const char* GetBlockName() override { return BlockName; }

	bhkSPCollisionObject* Clone() override { return new bhkSPCollisionObject(*this); }
};

class bhkBlendCollisionObject : public bhkCollisionObject {
private:
	float heirGain = 0.0f;
	float velGain = 0.0f;

public:
	static constexpr const char* BlockName = "bhkBlendCollisionObject";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	bhkBlendCollisionObject* Clone() override { return new bhkBlendCollisionObject(*this); }
};

class bhkPhysicsSystem : public BSExtraData {
private:
	uint32_t numBytes = 0;
	std::vector<char> data;

public:
	bhkPhysicsSystem(const uint32_t size = 0);

	static constexpr const char* BlockName = "bhkPhysicsSystem";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	bhkPhysicsSystem* Clone() override { return new bhkPhysicsSystem(*this); }
};

class bhkRagdollSystem : public BSExtraData {
private:
	uint32_t numBytes = 0;
	std::vector<char> data;

public:
	bhkRagdollSystem(const uint32_t size = 0);

	static constexpr const char* BlockName = "bhkRagdollSystem";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	bhkRagdollSystem* Clone() override { return new bhkRagdollSystem(*this); }
};

class bhkBlendController : public NiTimeController {
private:
	uint32_t keys = 0;

public:
	static constexpr const char* BlockName = "bhkBlendController";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	bhkBlendController* Clone() override { return new bhkBlendController(*this); }
};

class bhkRefObject : public NiObject {};

class bhkSerializable : public bhkRefObject {};

class bhkShape : public bhkSerializable {
public:
	virtual HavokMaterial GetMaterial() { return 0; }
	virtual void SetMaterial(HavokMaterial) {}
};

class bhkHeightFieldShape : public bhkShape {};

class bhkPlaneShape : public bhkHeightFieldShape {
private:
	HavokMaterial material = 0;
	Vector3 unkVec;
	Vector3 direction;
	float constant = 0.0f;
	Vector4 halfExtents;
	Vector4 center;

public:
	static constexpr const char* BlockName = "bhkPlaneShape";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	bhkPlaneShape* Clone() override { return new bhkPlaneShape(*this); }

	HavokMaterial GetMaterial() override { return material; }
	void SetMaterial(HavokMaterial mat) override { material = mat; }
};

class bhkSphereRepShape : public bhkShape {
private:
	HavokMaterial material = 0;
	float radius = 0.0f;

public:
	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	HavokMaterial GetMaterial() { return material; }
	void SetMaterial(HavokMaterial mat) { material = mat; }

	float GetRadius() { return radius; }
	void SetRadius(const float r) { radius = r; }
};

class bhkMultiSphereShape : public bhkSphereRepShape {
private:
	float unkFloat1 = 0.0f;
	float unkFloat2 = 0.0f;
	uint32_t numSpheres = 0;
	std::vector<BoundingSphere> spheres;

public:
	static constexpr const char* BlockName = "bhkMultiSphereShape";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	bhkMultiSphereShape* Clone() override { return new bhkMultiSphereShape(*this); }
};

class bhkConvexShape : public bhkSphereRepShape {};

class bhkConvexListShape : public bhkShape {
private:
	BlockRefArray<bhkConvexShape> shapeRefs;
	HavokMaterial material = 0;
	float radius = 0.0f;
	uint32_t unkInt1 = 0;
	float unkFloat1 = 0.0f;
	hkWorldObjCInfoProperty childShapeProp;
	uint8_t unkByte1 = 0;
	float unkFloat2 = 0.0f;

public:
	static constexpr const char* BlockName = "bhkConvexListShape";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	bhkConvexListShape* Clone() override { return new bhkConvexListShape(*this); }

	BlockRefArray<bhkConvexShape>& GetShapes();

	HavokMaterial GetMaterial() { return material; }
	void SetMaterial(HavokMaterial mat) { material = mat; }

	float GetRadius() { return radius; }
	void SetRadius(const float r) { radius = r; }
};

class bhkConvexVerticesShape : public bhkConvexShape {
private:
	hkWorldObjCInfoProperty vertsProp;
	hkWorldObjCInfoProperty normalsProp;

	uint32_t numVerts = 0;
	std::vector<Vector4> verts;

	uint32_t numNormals = 0;
	std::vector<Vector4> normals;

public:
	static constexpr const char* BlockName = "bhkConvexVerticesShape";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	bhkConvexVerticesShape* Clone() override { return new bhkConvexVerticesShape(*this); }
};

class bhkBoxShape : public bhkConvexShape {
private:
	uint64_t padding = 0;
	Vector3 dimensions;
	float radius2 = 0.0f;

public:
	static constexpr const char* BlockName = "bhkBoxShape";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	bhkBoxShape* Clone() override { return new bhkBoxShape(*this); }

	Vector3 GetDimensions() { return dimensions; }
	void SetDimensions(const Vector3 d) { dimensions = d; }

	float GetRadius2() { return radius2; }
	void SetRadius2(const float r) { radius2 = r; }
};

class bhkSphereShape : public bhkConvexShape {
public:
	static constexpr const char* BlockName = "bhkSphereShape";
	const char* GetBlockName() override { return BlockName; }

	bhkSphereShape* Clone() override { return new bhkSphereShape(*this); }
};

class bhkTransformShape : public bhkShape {
private:
	BlockRef<bhkShape> shapeRef;
	HavokMaterial material = 0;
	float radius = 0.0f;
	uint64_t padding = 0;
	Matrix4 xform;

public:
	static constexpr const char* BlockName = "bhkTransformShape";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	bhkTransformShape* Clone() override { return new bhkTransformShape(*this); }

	int GetShapeRef() { return shapeRef.GetIndex(); }
	void SetShapeRef(const int ref) { shapeRef.SetIndex(ref); }

	HavokMaterial GetMaterial() { return material; }
	void SetMaterial(HavokMaterial mat) { material = mat; }

	float GetRadius() { return radius; }
	void SetRadius(const float r) { radius = r; }
};

class bhkConvexTransformShape : public bhkTransformShape {
public:
	static constexpr const char* BlockName = "bhkConvexTransformShape";
	const char* GetBlockName() override { return BlockName; }

	bhkConvexTransformShape* Clone() override { return new bhkConvexTransformShape(*this); }
};

class bhkCapsuleShape : public bhkConvexShape {
private:
	uint64_t padding = 0;
	Vector3 point1;
	float radius1 = 0.0f;
	Vector3 point2;
	float radius2 = 0.0f;

public:
	static constexpr const char* BlockName = "bhkCapsuleShape";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	bhkCapsuleShape* Clone() override { return new bhkCapsuleShape(*this); }

	Vector3 GetPoint1() { return point1; }
	void SetPoint1(const Vector3 p) { point1 = p; }

	Vector3 GetPoint2() { return point2; }
	void SetPoint2(const Vector3 p) { point2 = p; }

	float GetRadius1() { return radius1; }
	void SetRadius1(const float r) { radius1 = r; }

	float GetRadius2() { return radius2; }
	void SetRadius2(const float r) { radius2 = r; }
};

class bhkBvTreeShape : public bhkShape {};

class bhkMoppBvTreeShape : public bhkBvTreeShape {
private:
	BlockRef<bhkShape> shapeRef;
	uint32_t userData = 0;
	uint32_t shapeCollection = 0;
	uint32_t code = 0;
	float scale = 0.0f;
	uint32_t dataSize = 0;
	Vector4 offset;
	uint8_t buildType = 2; // User Version >= 12
	std::vector<uint8_t> data;

public:
	static constexpr const char* BlockName = "bhkMoppBvTreeShape";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	bhkMoppBvTreeShape* Clone() override { return new bhkMoppBvTreeShape(*this); }

	int GetShapeRef() { return shapeRef.GetIndex(); }
	void SetShapeRef(const int ref) { shapeRef.SetIndex(ref); }
};

class NiTriStripsData;

class bhkNiTriStripsShape : public bhkShape {
private:
	HavokMaterial material = 0;
	float radius = 0.1f;
	uint32_t unused1 = 0;
	uint32_t unused2 = 0;
	uint32_t unused3 = 0;
	uint32_t unused4 = 0;
	uint32_t unused5 = 0;
	uint32_t growBy = 1;
	Vector4 scale = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	BlockRefArray<NiTriStripsData> partRefs;

	uint32_t numFilters = 0;
	std::vector<uint32_t> filters;

public:
	static constexpr const char* BlockName = "bhkNiTriStripsShape";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	bhkNiTriStripsShape* Clone() override { return new bhkNiTriStripsShape(*this); }

	BlockRefArray<NiTriStripsData>& GetParts();

	HavokMaterial GetMaterial() { return material; }
	void SetMaterial(HavokMaterial mat) { material = mat; }

	float GetRadius() { return radius; }
	void SetRadius(const float r) { radius = r; }
};

class bhkShapeCollection : public bhkShape {};

class bhkListShape : public bhkShapeCollection {
private:
	BlockRefArray<bhkShape> subShapeRefs;
	HavokMaterial material = 0;
	hkWorldObjCInfoProperty childShapeProp;
	hkWorldObjCInfoProperty childFilterProp;
	uint32_t numUnkInts = 0;
	std::vector<uint32_t> unkInts;

public:
	static constexpr const char* BlockName = "bhkListShape";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	bhkListShape* Clone() override { return new bhkListShape(*this); }

	BlockRefArray<bhkShape>& GetSubShapes();

	HavokMaterial GetMaterial() { return material; }
	void SetMaterial(HavokMaterial mat) { material = mat; }
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

class hkPackedNiTriStripsData : public bhkShapeCollection {
private:
	uint32_t keyCount = 0;
	std::vector<hkTriangleData> triData;
	std::vector<hkTriangleNormalData> triNormData;

	uint32_t numVerts = 0;
	uint8_t unkByte = 0;
	std::vector<Vector3> compressedVertData;

	uint16_t partCount = 0;
	std::vector<hkSubPartData> data;

public:
	static constexpr const char* BlockName = "hkPackedNiTriStripsData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	hkPackedNiTriStripsData* Clone() override { return new hkPackedNiTriStripsData(*this); }
};

class bhkPackedNiTriStripsShape : public bhkShapeCollection {
private:
	uint16_t partCount = 0;
	std::vector<hkSubPartData> data;

	uint32_t userData = 0;
	uint32_t unused1 = 0;
	float radius = 0.0f;
	uint32_t unused2 = 0;
	Vector4 scaling;
	float radius2 = 0.0f;
	Vector4 scaling2;
	BlockRef<hkPackedNiTriStripsData> dataRef;

public:
	static constexpr const char* BlockName = "bhkPackedNiTriStripsShape";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	bhkPackedNiTriStripsShape* Clone() override { return new bhkPackedNiTriStripsShape(*this); }

	float GetRadius() { return radius; }
	void SetRadius(const float r) { radius = r; }

	float GetRadius2() { return radius2; }
	void SetRadius2(const float r) { radius2 = r; }

	int GetDataRef() { return dataRef.GetIndex(); }
	void SetDataRef(const int ref) { dataRef.SetIndex(ref); }
};

class bhkLiquidAction : public bhkSerializable {
private:
	uint32_t userData = 0;
	uint32_t unkInt1 = 0;
	uint32_t unkInt2 = 0;
	float initialStickForce = 0.0f;
	float stickStrength = 0.0f;
	float neighborDistance = 0.0f;
	float neighborStrength = 0.0f;

public:
	static constexpr const char* BlockName = "bhkLiquidAction";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	bhkLiquidAction* Clone() override { return new bhkLiquidAction(*this); }
};

class bhkOrientHingedBodyAction : public bhkSerializable {
private:
	BlockRef<NiObject> bodyRef;
	uint32_t unkInt1 = 0;
	uint32_t unkInt2 = 0;
	uint64_t padding = 0;
	Vector4 hingeAxisLS;
	Vector4 forwardLS;
	float strength = 0.0f;
	float damping = 0.0f;
	uint64_t padding2 = 0;

public:
	static constexpr const char* BlockName = "bhkOrientHingedBodyAction";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;
	bhkOrientHingedBodyAction* Clone() override { return new bhkOrientHingedBodyAction(*this); }

	int GetBodyRef() { return bodyRef.GetIndex(); }
	void SetBodyRef(const int ref) { bodyRef.SetIndex(ref); }
};

class bhkWorldObject : public bhkSerializable {
private:
	BlockRef<bhkShape> shapeRef;
	HavokFilter collisionFilter;
	int unkInt1;
	uint8_t broadPhaseType;
	uint8_t unkBytes[3];
	hkWorldObjCInfoProperty prop;

public:
	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	bhkWorldObject* Clone() override { return new bhkWorldObject(*this); }

	int GetShapeRef() { return shapeRef.GetIndex(); }
	void SetShapeRef(const int ref) { shapeRef.SetIndex(ref); }

	HavokFilter GetCollisionFilter() { return collisionFilter; }
	void SetCollisionFilter(const HavokFilter cf) { collisionFilter = cf; }

	uint8_t GetBroadPhaseType() { return broadPhaseType; }
	void SetBroadPhaseType(const uint8_t bpt) { broadPhaseType = bpt; }
};

class bhkPhantom : public bhkWorldObject {};

class bhkShapePhantom : public bhkPhantom {};

class bhkSimpleShapePhantom : public bhkShapePhantom {
private:
	uint64_t padding = 0;
	Matrix4 transform;

public:
	static constexpr const char* BlockName = "bhkSimpleShapePhantom";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	bhkSimpleShapePhantom* Clone() override { return new bhkSimpleShapePhantom(*this); }
};

class bhkAabbPhantom : public bhkShapePhantom {
private:
	uint64_t padding = 0;
	Vector4 aabbMin;
	Vector4 aabbMax;

public:
	static constexpr const char* BlockName = "bhkAabbPhantom";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	bhkAabbPhantom* Clone() override { return new bhkAabbPhantom(*this); }
};

class bhkEntity : public bhkWorldObject {};

class bhkConstraint : public bhkSerializable {
private:
	BlockRefArray<bhkEntity> entityRefs;
	uint32_t priority;

public:
	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;

	BlockRefArray<bhkEntity>& GetEntities();
};

class bhkHingeConstraint : public bhkConstraint {
private:
	HingeDesc hinge;

public:
	static constexpr const char* BlockName = "bhkHingeConstraint";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	bhkHingeConstraint* Clone() override { return new bhkHingeConstraint(*this); }
};

class bhkLimitedHingeConstraint : public bhkConstraint {
private:
	LimitedHingeDesc limitedHinge;

public:
	static constexpr const char* BlockName = "bhkLimitedHingeConstraint";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	bhkLimitedHingeConstraint* Clone() override { return new bhkLimitedHingeConstraint(*this); }
};

class ConstraintData {
private:
	hkConstraintType type;
	BlockRefArray<bhkEntity> entityRefs;
	uint32_t priority = 1;

	BallAndSocketDesc desc1;
	HingeDesc desc2;
	LimitedHingeDesc desc3;
	PrismaticDesc desc4;
	RagdollDesc desc5;
	StiffSpringDesc desc6;

	float strength = 0.0f;

public:
	void Get(NiStream& stream);
	void Put(NiStream& stream);
	void GetPtrs(std::set<Ref*>& ptrs);

	BlockRefArray<bhkEntity>& GetEntities();
};

class bhkBreakableConstraint : public bhkConstraint {
private:
	ConstraintData subConstraint;
	bool removeWhenBroken = false;

public:
	static constexpr const char* BlockName = "bhkBreakableConstraint";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;
	bhkBreakableConstraint* Clone() override { return new bhkBreakableConstraint(*this); }
};

class bhkRagdollConstraint : public bhkConstraint {
private:
	RagdollDesc ragdoll;

public:
	static constexpr const char* BlockName = "bhkRagdollConstraint";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	bhkRagdollConstraint* Clone() override { return new bhkRagdollConstraint(*this); }
};

class bhkStiffSpringConstraint : public bhkConstraint {
private:
	StiffSpringDesc stiffSpring;

public:
	static constexpr const char* BlockName = "bhkStiffSpringConstraint";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	bhkStiffSpringConstraint* Clone() override { return new bhkStiffSpringConstraint(*this); }
};

class bhkPrismaticConstraint : public bhkConstraint {
private:
	PrismaticDesc prismatic;

public:
	static constexpr const char* BlockName = "bhkPrismaticConstraint";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	bhkPrismaticConstraint* Clone() override { return new bhkPrismaticConstraint(*this); }
};

class bhkMalleableConstraint : public bhkConstraint {
private:
	ConstraintData subConstraint;

public:
	static constexpr const char* BlockName = "bhkMalleableConstraint";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	bhkMalleableConstraint* Clone() override { return new bhkMalleableConstraint(*this); }
};

class bhkBallAndSocketConstraint : public bhkConstraint {
private:
	BallAndSocketDesc ballAndSocket;

public:
	static constexpr const char* BlockName = "bhkBallAndSocketConstraint";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	bhkBallAndSocketConstraint* Clone() override { return new bhkBallAndSocketConstraint(*this); }
};

class bhkBallSocketConstraintChain : public bhkSerializable {
private:
	uint32_t numPivots = 0;
	std::vector<Vector4> pivots;

	float tau = 1.0f;
	float damping = 0.6f;
	float cfm = 1.1920929e-08f;
	float maxErrorDistance = 0.1f;

	BlockRefArray<bhkEntity> entityARefs;

	uint32_t numEntities = 0;
	BlockRef<bhkEntity> entityARef;
	BlockRef<bhkEntity> entityBRef;
	uint32_t priority = 0;

public:
	static constexpr const char* BlockName = "bhkBallSocketConstraintChain";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;
	bhkBallSocketConstraintChain* Clone() override { return new bhkBallSocketConstraintChain(*this); }

	BlockRefArray<bhkEntity>& GetEntitiesA();

	int GetEntityARef();
	void SetEntityARef(int entityRef);

	int GetEntityBRef();
	void SetEntityBRef(int entityRef);
};

enum hkResponseType : uint8_t { RESPONSE_INVALID, RESPONSE_SIMPLE_CONTACT, RESPONSE_REPORTING, RESPONSE_NONE };

class bhkRigidBody : public bhkEntity {
private:
	hkResponseType collisionResponse = RESPONSE_SIMPLE_CONTACT;
	uint8_t unusedByte1 = 0;
	uint16_t processContactCallbackDelay = 0xFFFF;
	uint32_t unkInt1 = 0;
	HavokFilter collisionFilterCopy;
	uint16_t unkShorts2[6];
	Vector4 translation;
	QuaternionXYZW rotation;
	Vector4 linearVelocity;
	Vector4 angularVelocity;
	float inertiaMatrix[12];
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
	uint32_t unkInt2 = 0;
	uint32_t unkInt3 = 0;
	uint32_t unkInt4 = 0; // User Version >= 12
	BlockRefArray<bhkSerializable> constraintRefs;
	uint32_t unkInt5 = 0;	  // User Version <= 11
	uint16_t bodyFlags = 0; // User Version >= 12

public:
	static constexpr const char* BlockName = "bhkRigidBody";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	bhkRigidBody* Clone() override { return new bhkRigidBody(*this); }

	HavokFilter GetCollisionFilterCopy() { return collisionFilterCopy; }
	void SetCollisionFilterCopy(const HavokFilter cf) { collisionFilterCopy = cf; }

	BlockRefArray<bhkSerializable>& GetConstraints();
};

class bhkRigidBodyT : public bhkRigidBody {
public:
	static constexpr const char* BlockName = "bhkRigidBodyT";
	const char* GetBlockName() override { return BlockName; }

	bhkRigidBodyT* Clone() override { return new bhkRigidBodyT(*this); }
};

class bhkCompressedMeshShapeData : public bhkRefObject {
private:
	uint32_t bitsPerIndex = 0;
	uint32_t bitsPerWIndex = 0;
	uint32_t maskWIndex = 0;
	uint32_t maskIndex = 0;
	float error = 0.0f;
	Vector4 aabbBoundMin;
	Vector4 aabbBoundMax;
	uint8_t weldingType = 0;
	uint8_t materialType = 0;

	uint32_t numMat32 = 0;
	std::vector<uint32_t> mat32;
	uint32_t numMat16 = 0;
	std::vector<uint32_t> mat16;
	uint32_t numMat8 = 0;
	std::vector<uint32_t> mat8;

	uint32_t numMaterials = 0;
	std::vector<bhkCMSDMaterial> materials;

	uint32_t numNamedMat = 0;

	uint32_t numTransforms = 0;
	std::vector<bhkCMSDTransform> transforms;

	uint32_t numBigVerts = 0;
	std::vector<Vector4> bigVerts;

	uint32_t numBigTris = 0;
	std::vector<bhkCMSDBigTris> bigTris;

	uint32_t numChunks = 0;
	std::vector<bhkCMSDChunk> chunks;

	uint32_t numConvexPieceA = 0;

public:
	static constexpr const char* BlockName = "bhkCompressedMeshShapeData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	bhkCompressedMeshShapeData* Clone() override { return new bhkCompressedMeshShapeData(*this); }
};

class bhkCompressedMeshShape : public bhkShape {
private:
	BlockRef<NiAVObject> targetRef;
	uint32_t userData = 0;
	float radius = 0.005f;
	float unkFloat = 0.0f;
	Vector4 scaling = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	float radius2 = 0.005f;
	Vector4 scaling2 = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	BlockRef<bhkCompressedMeshShapeData> dataRef;

public:
	static constexpr const char* BlockName = "bhkCompressedMeshShape";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;
	bhkCompressedMeshShape* Clone() override { return new bhkCompressedMeshShape(*this); }

	int GetTargetRef() { return targetRef.GetIndex(); }
	void SetTargetRef(const int ref) { targetRef.SetIndex(ref); }

	float GetRadius() { return radius; }
	void SetRadius(const float r) { radius = r; }

	float GetRadius2() { return radius2; }
	void SetRadius2(const float r) { radius2 = r; }

	int GetDataRef() { return dataRef.GetIndex(); }
	void SetDataRef(const int ref) { dataRef.SetIndex(ref); }
};

struct BoneMatrix {
	Vector3 translation;
	QuaternionXYZW rotation;
	Vector3 scale;
};

struct BonePose {
	uint32_t numMatrices = 0;
	std::vector<BoneMatrix> matrices;

	void Get(NiStream& stream) {
		stream >> numMatrices;
		matrices.resize(numMatrices);
		for (int i = 0; i < numMatrices; i++)
			stream >> matrices[i];
	}

	void Put(NiStream& stream) {
		stream << numMatrices;
		for (int i = 0; i < numMatrices; i++)
			stream << matrices[i];
	}
};

class bhkPoseArray : public NiObject {
private:
	uint32_t numBones = 0;
	std::vector<StringRef> bones;

	uint32_t numPoses = 0;
	std::vector<BonePose> poses;

public:
	static constexpr const char* BlockName = "bhkPoseArray";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::set<StringRef*>& refs) override;
	bhkPoseArray* Clone() override { return new bhkPoseArray(*this); }
};

class bhkRagdollTemplate : public NiExtraData {
private:
	uint32_t numBones = 0;
	BlockRefArray<NiObject> boneRefs;

public:
	static constexpr const char* BlockName = "bhkRagdollTemplate";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	bhkRagdollTemplate* Clone() override { return new bhkRagdollTemplate(*this); }

	BlockRefArray<NiObject>& GetBones();
};

class bhkRagdollTemplateData : public NiObject {
private:
	StringRef name;
	float mass = 9.0f;
	float restitution = 0.8f;
	float friction = 0.3f;
	float radius = 1.0f;
	HavokMaterial material = 7;
	uint32_t numConstraints = 0;
	std::vector<ConstraintData> constraints;

public:
	static constexpr const char* BlockName = "bhkRagdollTemplateData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::set<StringRef*>& refs) override;
	bhkRagdollTemplateData* Clone() override { return new bhkRagdollTemplateData(*this); }

	HavokMaterial GetMaterial() { return material; }
	void SetMaterial(HavokMaterial mat) { material = mat; }

	float GetRadius() { return radius; }
	void SetRadius(const float r) { radius = r; }
};
} // namespace nifly
