/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#pragma once

#include "BasicTypes.hpp"
#include "Geometry.hpp"
#include "Nodes.hpp"

namespace nifly {
class NiParticles : public Clonable<NiParticles, NiGeometry> {
public:
	static constexpr const char* BlockName = "NiParticles";
	const char* GetBlockName() override { return BlockName; }
};

class NiAutoNormalParticles : public Clonable<NiAutoNormalParticles, NiParticles> {
public:
	static constexpr const char* BlockName = "NiAutoNormalParticles";
	const char* GetBlockName() override { return BlockName; }
};

class NiParticleMeshes : public Clonable<NiParticleMeshes, NiParticles> {
public:
	static constexpr const char* BlockName = "NiParticleMeshes";
	const char* GetBlockName() override { return BlockName; }
};

class NiRotatingParticles : public Clonable<NiRotatingParticles, NiParticles> {
public:
	static constexpr const char* BlockName = "NiRotatingParticles";
	const char* GetBlockName() override { return BlockName; }
};

class NiParticlesData : public Streamable<NiParticlesData, NiGeometryData> {
private:
	bool hasRadii = false;
	uint16_t numActive = 0;
	bool hasSizes = false;
	bool hasRotations = false;
	bool hasRotationAngles = false;
	bool hasRotationAxes = false;
	bool hasTextureIndices = false;

	uint32_t numSubtexOffsets = 0;
	std::vector<Vector4> subtexOffsets;

	float aspectRatio = 0.0f;
	uint16_t aspectFlags = 0;
	float speedToAspectAspect2 = 0.0f;
	float speedToAspectSpeed1 = 0.0f;
	float speedToAspectSpeed2 = 0.0f;

public:
	NiParticlesData();

	static constexpr const char* BlockName = "NiParticlesData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiAutoNormalParticlesData : public Clonable<NiAutoNormalParticlesData, NiParticlesData> {
public:
	static constexpr const char* BlockName = "NiAutoNormalParticlesData";
	const char* GetBlockName() override { return BlockName; }
};

class NiRotatingParticlesData : public Clonable<NiRotatingParticlesData, NiParticlesData> {
public:
	static constexpr const char* BlockName = "NiRotatingParticlesData";
	const char* GetBlockName() override { return BlockName; }
};

class NiParticleMeshesData : public Streamable<NiParticleMeshesData, NiRotatingParticlesData> {
private:
	BlockRef<NiAVObject> dataRef;

public:
	static constexpr const char* BlockName = "NiParticleMeshesData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class NiPSysData : public Streamable<NiPSysData, NiRotatingParticlesData> {
private:
	Vector3 unknownVector;
	bool hasRotationSpeeds = false;

public:
	static constexpr const char* BlockName = "NiPSysData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiMeshPSysData : public Streamable<NiMeshPSysData, NiPSysData> {
private:
	uint32_t defaultPoolSize = 0;
	bool fillPoolsOnLoad = false;

	uint32_t numGenerations = 0;
	std::vector<uint32_t> generationPoolSize;

	BlockRef<NiNode> nodeRef;

public:
	static constexpr const char* BlockName = "NiMeshPSysData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class BSStripPSysData : public Streamable<BSStripPSysData, NiPSysData> {
private:
	uint16_t maxPointCount = 0;
	uint32_t startCapSize = 0;
	uint32_t endCapSize = 0;
	bool doZPrepass = false;

public:
	static constexpr const char* BlockName = "BSStripPSysData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiPSysEmitterCtlrData : public Streamable<NiPSysEmitterCtlrData, NiObject> {
private:
	KeyGroup<float> floatKeys;
	uint32_t numVisibilityKeys = 0;
	std::vector<Key<uint8_t>> visibilityKeys;

public:
	static constexpr const char* BlockName = "NiPSysEmitterCtlrData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiParticleSystem;

class NiPSysModifier : public Streamable<NiPSysModifier, NiObject> {
private:
	StringRef name;
	uint32_t order = 0;
	BlockRef<NiParticleSystem> targetRef;
	bool isActive = false;

public:
	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<StringRef*>& refs) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;
};

class BSPSysStripUpdateModifier : public Streamable<BSPSysStripUpdateModifier, NiPSysModifier> {
private:
	float updateDeltaTime = 0.0f;

public:
	static constexpr const char* BlockName = "BSPSysStripUpdateModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiPSysSpawnModifier : public Streamable<NiPSysSpawnModifier, NiPSysModifier> {
private:
	uint16_t numSpawnGenerations = 0;
	float percentSpawned = 0.0f;
	uint16_t minSpawned = 0;
	uint16_t maxSpawned = 0;
	float spawnSpeedVariation = 0.0f;
	float spawnDirVariation = 0.0f;
	float lifeSpan = 0.0f;
	float lifeSpanVariation = 0.0f;

public:
	static constexpr const char* BlockName = "NiPSysSpawnModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiPSysAgeDeathModifier : public Streamable<NiPSysAgeDeathModifier, NiPSysModifier> {
private:
	bool spawnOnDeath = false;
	BlockRef<NiPSysSpawnModifier> spawnModifierRef;

public:
	static constexpr const char* BlockName = "NiPSysAgeDeathModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class BSPSysLODModifier : public Streamable<BSPSysLODModifier, NiPSysModifier> {
private:
	float lodBeginDistance = 0.0f;
	float lodEndDistance = 0.0f;
	float unknownFadeFactor1 = 0.0f;
	float unknownFadeFactor2 = 0.0f;

public:
	static constexpr const char* BlockName = "BSPSysLODModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSPSysSimpleColorModifier : public Streamable<BSPSysSimpleColorModifier, NiPSysModifier> {
private:
	float fadeInPercent = 0.0f;
	float fadeOutPercent = 0.0f;
	float color1EndPercent = 0.0f;
	float color2StartPercent = 0.0f;
	float color2EndPercent = 0.0f;
	float color3StartPercent = 0.0f;
	Color4 color1;
	Color4 color2;
	Color4 color3;
	uint16_t unknownShorts[26]{};

public:
	static constexpr const char* BlockName = "BSPSysSimpleColorModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiPSysRotationModifier : public Streamable<NiPSysRotationModifier, NiPSysModifier> {
private:
	float initialSpeed = 0.0f;
	float initialSpeedVariation = 0.0f;
	Vector4 unknownVector;
	uint8_t unknownByte = 0;
	float initialAngle = 0.0f;
	float initialAngleVariation = 0.0f;
	bool randomSpeedSign = false;
	bool randomInitialAxis = false;
	Vector3 initialAxis;

public:
	static constexpr const char* BlockName = "NiPSysRotationModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSPSysScaleModifier : public Streamable<BSPSysScaleModifier, NiPSysModifier> {
private:
	uint32_t numFloats = 0;
	std::vector<float> floats;

public:
	static constexpr const char* BlockName = "BSPSysScaleModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

enum ForceType : uint32_t { FORCE_PLANAR, FORCE_SPHERICAL, FORCE_UNKNOWN };

class NiPSysGravityModifier : public Streamable<NiPSysGravityModifier, NiPSysModifier> {
private:
	BlockRef<NiNode> gravityObjRef;
	Vector3 gravityAxis;
	float decay = 0.0f;
	float strength = 0.0f;
	ForceType forceType = FORCE_UNKNOWN;
	float turbulence = 0.0f;
	float turbulenceScale = 1.0f;
	bool worldAligned = false;

public:
	static constexpr const char* BlockName = "NiPSysGravityModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<Ref*>& ptrs) override;
};

class NiPSysPositionModifier : public Clonable<NiPSysPositionModifier, NiPSysModifier> {
public:
	static constexpr const char* BlockName = "NiPSysPositionModifier";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysBoundUpdateModifier : public Streamable<NiPSysBoundUpdateModifier, NiPSysModifier> {
private:
	uint16_t updateSkip = 0;

public:
	static constexpr const char* BlockName = "NiPSysBoundUpdateModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiPSysDragModifier : public Streamable<NiPSysDragModifier, NiPSysModifier> {
private:
	BlockRef<NiObject> parentRef;
	Vector3 dragAxis;
	float percentage = 0.0f;
	float range = 0.0f;
	float rangeFalloff = 0.0f;

public:
	static constexpr const char* BlockName = "NiPSysDragModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<Ref*>& ptrs) override;
};

class BSPSysInheritVelocityModifier	: public Streamable<BSPSysInheritVelocityModifier, NiPSysModifier> {
private:
	BlockRef<NiNode> targetNodeRef;
	float changeToInherit = 0.0f;
	float velocityMult = 0.0f;
	float velocityVar = 0.0f;

public:
	static constexpr const char* BlockName = "BSPSysInheritVelocityModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<Ref*>& ptrs) override;
};

class BSPSysSubTexModifier : public Streamable<BSPSysSubTexModifier, NiPSysModifier> {
private:
	float startFrame = 0.0f;
	float startFrameVariation = 0.0f;
	float endFrame = 0.0f;
	float loopStartFrame = 0.0f;
	float loopStartFrameVariation = 0.0f;
	float frameCount = 0.0f;
	float frameCountVariation = 0.0f;

public:
	static constexpr const char* BlockName = "BSPSysSubTexModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

enum DecayType : uint32_t { DECAY_NONE, DECAY_LINEAR, DECAY_EXPONENTIAL };

enum SymmetryType : uint32_t { SYMMETRY_SPHERICAL, SYMMETRY_CYLINDRICAL, SYMMETRY_PLANAR };

class NiPSysBombModifier : public Streamable<NiPSysBombModifier, NiPSysModifier> {
private:
	BlockRef<NiNode> bombNodeRef;
	Vector3 bombAxis;
	float decay = 0.0f;
	float deltaV = 0.0f;
	DecayType decayType = DECAY_NONE;
	SymmetryType symmetryType = SYMMETRY_SPHERICAL;

public:
	static constexpr const char* BlockName = "NiPSysBombModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<Ref*>& ptrs) override;
};

class NiColorData : public Streamable<NiColorData, NiObject> {
private:
	KeyGroup<Color4> data;

public:
	static constexpr const char* BlockName = "NiColorData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiPSysColorModifier : public Streamable<NiPSysColorModifier, NiPSysModifier> {
private:
	BlockRef<NiColorData> dataRef;

public:
	static constexpr const char* BlockName = "NiPSysColorModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class NiPSysGrowFadeModifier : public Streamable<NiPSysGrowFadeModifier, NiPSysModifier> {
private:
	float growTime = 0.0f;
	uint16_t growGeneration = 0;
	float fadeTime = 0.0f;
	uint16_t fadeGeneration = 0;
	float baseScale = 0.0f;

public:
	static constexpr const char* BlockName = "NiPSysGrowFadeModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiPSysMeshUpdateModifier : public Streamable<NiPSysMeshUpdateModifier, NiPSysModifier> {
private:
	BlockRefArray<NiAVObject> meshRefs;

public:
	static constexpr const char* BlockName = "NiPSysMeshUpdateModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	BlockRefArray<NiAVObject>& GetMeshes();
};

class NiPSysFieldModifier : public Streamable<NiPSysFieldModifier, NiPSysModifier> {
private:
	BlockRef<NiAVObject> fieldObjectRef;
	float magnitude = 0.0f;
	float attenuation = 0.0f;
	bool useMaxDistance = false;
	float maxDistance = 0.0f;

public:
	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class NiPSysVortexFieldModifier : public Streamable<NiPSysVortexFieldModifier, NiPSysFieldModifier> {
private:
	Vector3 direction;

public:
	static constexpr const char* BlockName = "NiPSysVortexFieldModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiPSysGravityFieldModifier : public Streamable<NiPSysGravityFieldModifier, NiPSysFieldModifier> {
private:
	Vector3 direction;

public:
	static constexpr const char* BlockName = "NiPSysGravityFieldModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiPSysDragFieldModifier : public Streamable<NiPSysDragFieldModifier, NiPSysFieldModifier> {
private:
	bool useDirection = false;
	Vector3 direction;

public:
	static constexpr const char* BlockName = "NiPSysDragFieldModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiPSysTurbulenceFieldModifier	: public Streamable<NiPSysTurbulenceFieldModifier, NiPSysFieldModifier> {
private:
	float frequency = 0.0f;

public:
	static constexpr const char* BlockName = "NiPSysTurbulenceFieldModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiPSysAirFieldModifier : public Streamable<NiPSysAirFieldModifier, NiPSysFieldModifier> {
private:
	Vector3 direction;
	float unkFloat1 = 0.0f;
	float unkFloat2 = 0.0f;
	bool unkBool1 = false;
	bool unkBool2 = false;
	bool unkBool3 = false;
	float unkFloat3 = 0.0f;

public:
	static constexpr const char* BlockName = "NiPSysAirFieldModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiPSysRadialFieldModifier : public Streamable<NiPSysRadialFieldModifier, NiPSysFieldModifier> {
private:
	uint32_t radialType = 0;

public:
	static constexpr const char* BlockName = "NiPSysRadialFieldModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSWindModifier : public Streamable<BSWindModifier, NiPSysModifier> {
private:
	float strength = 0.0f;

public:
	static constexpr const char* BlockName = "BSWindModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSPSysRecycleBoundModifier : public Streamable<BSPSysRecycleBoundModifier, NiPSysModifier> {
private:
	Vector3 boundOffset;
	Vector3 boundExtent;
	BlockRef<NiNode> targetNodeRef;

public:
	static constexpr const char* BlockName = "BSPSysRecycleBoundModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<Ref*>& ptrs) override;
};

class BSPSysHavokUpdateModifier : public Streamable<BSPSysHavokUpdateModifier, NiPSysModifier> {
private:
	BlockRefArray<NiNode> nodeRefs;
	BlockRef<NiPSysModifier> modifierRef;

public:
	static constexpr const char* BlockName = "BSPSysHavokUpdateModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	BlockRefArray<NiNode>& GetNodes();

	int GetModifierRef();
	void SetModifierRef(int modRef);
};

class BSParentVelocityModifier : public Streamable<BSParentVelocityModifier, NiPSysModifier> {
private:
	float damping = 0.0f;

public:
	static constexpr const char* BlockName = "BSParentVelocityModifier";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSMasterParticleSystem : public Streamable<BSMasterParticleSystem, NiNode> {
private:
	uint16_t maxEmitterObjs = 0;
	BlockRefArray<NiAVObject> particleSysRefs;

public:
	static constexpr const char* BlockName = "BSMasterParticleSystem";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	BlockRefArray<NiAVObject>& GetParticleSystems();
};

class NiParticleSystem : public Streamable<NiParticleSystem, NiAVObject> {
private:
	BlockRef<NiGeometryData> dataRef;
	BlockRef<NiObject> skinInstanceRef;
	BlockRef<NiProperty> shaderPropertyRef;
	BlockRef<NiProperty> alphaPropertyRef;

	uint32_t numMaterials = 0;
	std::vector<StringRef> materialNameRefs;
	std::vector<uint32_t> materials;

	uint32_t activeMaterial = 0;
	uint8_t defaultMatNeedsUpdate = 0;

	BoundingSphere bounds;
	float boundMinMax[6]{};
	uint8_t vertFlags1 = 81;
	uint8_t vertFlags2 = 0;
	uint8_t vertFlags3 = 0;
	uint8_t vertFlags4 = 4;
	uint8_t vertFlags5 = 0;
	uint8_t vertFlags6 = 32;
	uint8_t vertFlags7 = 64;
	uint8_t vertFlags8 = 8;

	uint16_t farBegin = 0;
	uint16_t farEnd = 0;
	uint16_t nearBegin = 0;
	uint16_t nearEnd = 0;

	BlockRef<NiPSysData> psysDataRef;

	bool isWorldSpace = false;
	BlockRefArray<NiPSysModifier> modifierRefs;

public:
	static constexpr const char* BlockName = "NiParticleSystem";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<StringRef*>& refs) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	int GetDataRef();
	void SetDataRef(int datRef);

	int GetSkinInstanceRef();
	void SetSkinInstanceRef(int skinRef);

	int GetShaderPropertyRef();
	void SetShaderPropertyRef(int shaderRef);

	int GetAlphaPropertyRef();
	void SetAlphaPropertyRef(int alphaRef);

	int GetPSysDataRef();
	void SetPSysDataRef(int psysDatRef);

	BlockRefArray<NiPSysModifier>& GetModifiers();
};

class NiMeshParticleSystem : public Clonable<NiMeshParticleSystem, NiParticleSystem> {
public:
	static constexpr const char* BlockName = "NiMeshParticleSystem";
	const char* GetBlockName() override { return BlockName; }
};

class BSStripParticleSystem : public Clonable<BSStripParticleSystem, NiParticleSystem> {
public:
	static constexpr const char* BlockName = "BSStripParticleSystem";
	const char* GetBlockName() override { return BlockName; }
};

class NiPSysColliderManager;

class NiPSysCollider : public Streamable<NiPSysCollider, NiObject> {
private:
	float bounce = 0.0f;
	bool spawnOnCollide = false;
	bool dieOnCollide = false;
	BlockRef<NiPSysSpawnModifier> spawnModifierRef;
	BlockRef<NiPSysColliderManager> managerRef;
	BlockRef<NiPSysCollider> nextColliderRef;
	BlockRef<NiNode> colliderNodeRef;

public:
	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;
};

class NiPSysSphericalCollider : public Streamable<NiPSysSphericalCollider, NiPSysCollider> {
private:
	float radius = 0.0f;

public:
	static constexpr const char* BlockName = "NiPSysSphericalCollider";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiPSysPlanarCollider : public Streamable<NiPSysPlanarCollider, NiPSysCollider> {
private:
	float width = 0.0f;
	float height = 0.0f;
	Vector3 xAxis;
	Vector3 yAxis;

public:
	static constexpr const char* BlockName = "NiPSysPlanarCollider";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiPSysColliderManager : public Streamable<NiPSysColliderManager, NiPSysModifier> {
private:
	BlockRef<NiPSysCollider> colliderRef;

public:
	static constexpr const char* BlockName = "NiPSysColliderManager";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class NiPSysEmitter : public Streamable<NiPSysEmitter, NiPSysModifier> {
private:
	float speed = 0.0f;
	float speedVariation = 0.0f;
	float declination = 0.0f;
	float declinationVariation = 0.0f;
	float planarAngle = 0.0f;
	float planarAngleVariation = 0.0f;
	Color4 color;
	float radius = 0.0f;
	float radiusVariation = 0.0f;
	float lifeSpan = 0.0f;
	float lifeSpanVariation = 0.0f;

public:
	void Sync(NiStreamReversible& stream);
};

class NiPSysVolumeEmitter : public Streamable<NiPSysVolumeEmitter, NiPSysEmitter> {
private:
	BlockRef<NiNode> emitterNodeRef;

public:
	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<Ref*>& ptrs) override;
};

class NiPSysSphereEmitter : public Streamable<NiPSysSphereEmitter, NiPSysVolumeEmitter> {
private:
	float radius = 0.0f;

public:
	static constexpr const char* BlockName = "NiPSysSphereEmitter";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiPSysCylinderEmitter : public Streamable<NiPSysCylinderEmitter, NiPSysVolumeEmitter> {
private:
	float radius = 0.0f;
	float height = 0.0f;

public:
	static constexpr const char* BlockName = "NiPSysCylinderEmitter";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiPSysBoxEmitter : public Streamable<NiPSysBoxEmitter, NiPSysVolumeEmitter> {
private:
	float width = 0.0f;
	float height = 0.0f;
	float depth = 0.0f;

public:
	static constexpr const char* BlockName = "NiPSysBoxEmitter";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSPSysArrayEmitter : public Clonable<BSPSysArrayEmitter, NiPSysVolumeEmitter> {
public:
	static constexpr const char* BlockName = "BSPSysArrayEmitter";
	const char* GetBlockName() override { return BlockName; }
};

enum VelocityType : uint32_t { VELOCITY_USE_NORMALS, VELOCITY_USE_RANDOM, VELOCITY_USE_DIRECTION };

enum EmitFrom : uint32_t {
	EMIT_FROM_VERTICES,
	EMIT_FROM_FACE_CENTER,
	EMIT_FROM_EDGE_CENTER,
	EMIT_FROM_FACE_SURFACE,
	EMIT_FROM_EDGE_SURFACE
};

class NiPSysMeshEmitter : public Streamable<NiPSysMeshEmitter, NiPSysEmitter> {
private:
	BlockRefArray<NiAVObject> meshRefs;
	VelocityType velocityType = VELOCITY_USE_NORMALS;
	EmitFrom emissionType = EMIT_FROM_VERTICES;
	Vector3 emissionAxis;

public:
	static constexpr const char* BlockName = "NiPSysMeshEmitter";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	BlockRefArray<NiAVObject>& GetMeshes();
};
} // namespace nifly
