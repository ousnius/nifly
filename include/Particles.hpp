/*
BodySlide and Outfit Studio
See the included LICENSE file
*/

#pragma once

#include "BasicTypes.hpp"
#include "Geometry.hpp"
#include "Nodes.hpp"

namespace nifly {
class NiParticles : public NiGeometry {
public:
	static constexpr const char* BlockName = "NiParticles";
	const char* GetBlockName() override { return BlockName; }

	NiParticles* Clone() override { return new NiParticles(*this); }
};

class NiAutoNormalParticles : public NiParticles {
public:
	static constexpr const char* BlockName = "NiAutoNormalParticles";
	const char* GetBlockName() override { return BlockName; }

	NiAutoNormalParticles* Clone() override { return new NiAutoNormalParticles(*this); }
};

class NiParticleMeshes : public NiParticles {
public:
	static constexpr const char* BlockName = "NiParticleMeshes";
	const char* GetBlockName() override { return BlockName; }

	NiParticleMeshes* Clone() override { return new NiParticleMeshes(*this); }
};

class NiRotatingParticles : public NiParticles {
public:
	static constexpr const char* BlockName = "NiRotatingParticles";
	const char* GetBlockName() override { return BlockName; }

	NiRotatingParticles* Clone() override { return new NiRotatingParticles(*this); }
};

class NiParticlesData : public NiGeometryData {
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

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiParticlesData* Clone() override { return new NiParticlesData(*this); }
};

class NiAutoNormalParticlesData : public NiParticlesData {
public:
	static constexpr const char* BlockName = "NiAutoNormalParticlesData";
	const char* GetBlockName() override { return BlockName; }

	NiAutoNormalParticlesData* Clone() override { return new NiAutoNormalParticlesData(*this); }
};

class NiRotatingParticlesData : public NiParticlesData {
public:
	static constexpr const char* BlockName = "NiRotatingParticlesData";
	const char* GetBlockName() override { return BlockName; }

	NiRotatingParticlesData* Clone() override { return new NiRotatingParticlesData(*this); }
};

class NiParticleMeshesData : public NiRotatingParticlesData {
private:
	BlockRef<NiAVObject> dataRef;

public:
	static constexpr const char* BlockName = "NiParticleMeshesData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	NiParticleMeshesData* Clone() override { return new NiParticleMeshesData(*this); }
};

class NiPSysData : public NiRotatingParticlesData {
private:
	Vector3 unknownVector;
	bool hasRotationSpeeds = false;

public:
	static constexpr const char* BlockName = "NiPSysData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiPSysData* Clone() override { return new NiPSysData(*this); }
};

class NiMeshPSysData : public NiPSysData {
private:
	uint32_t defaultPoolSize = 0;
	bool fillPoolsOnLoad = false;

	uint32_t numGenerations = 0;
	std::vector<uint32_t> generationPoolSize;

	BlockRef<NiNode> nodeRef;

public:
	static constexpr const char* BlockName = "NiMeshPSysData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	NiMeshPSysData* Clone() override { return new NiMeshPSysData(*this); }
};

class BSStripPSysData : public NiPSysData {
private:
	uint16_t maxPointCount = 0;
	uint32_t startCapSize = 0;
	uint32_t endCapSize = 0;
	bool doZPrepass = false;

public:
	static constexpr const char* BlockName = "BSStripPSysData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSStripPSysData* Clone() override { return new BSStripPSysData(*this); }
};

class NiPSysEmitterCtlrData : public NiObject {
private:
	KeyGroup<float> floatKeys;
	uint32_t numVisibilityKeys = 0;
	std::vector<Key<uint8_t>> visibilityKeys;

public:
	static constexpr const char* BlockName = "NiPSysEmitterCtlrData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiPSysEmitterCtlrData* Clone() override { return new NiPSysEmitterCtlrData(*this); }
};

class NiParticleSystem;

class NiPSysModifier : public NiObject {
private:
	StringRef name;
	uint32_t order = 0;
	BlockRef<NiParticleSystem> targetRef;
	bool isActive = false;

public:
	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::set<StringRef*>& refs) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;
};

class BSPSysStripUpdateModifier : public NiPSysModifier {
private:
	float updateDeltaTime = 0.0f;

public:
	static constexpr const char* BlockName = "BSPSysStripUpdateModifier";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSPSysStripUpdateModifier* Clone() override { return new BSPSysStripUpdateModifier(*this); }
};

class NiPSysSpawnModifier : public NiPSysModifier {
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

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiPSysSpawnModifier* Clone() override { return new NiPSysSpawnModifier(*this); }
};

class NiPSysAgeDeathModifier : public NiPSysModifier {
private:
	bool spawnOnDeath = false;
	BlockRef<NiPSysSpawnModifier> spawnModifierRef;

public:
	static constexpr const char* BlockName = "NiPSysAgeDeathModifier";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	NiPSysAgeDeathModifier* Clone() override { return new NiPSysAgeDeathModifier(*this); }
};

class BSPSysLODModifier : public NiPSysModifier {
private:
	float lodBeginDistance = 0.0f;
	float lodEndDistance = 0.0f;
	float unknownFadeFactor1 = 0.0f;
	float unknownFadeFactor2 = 0.0f;

public:
	static constexpr const char* BlockName = "BSPSysLODModifier";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSPSysLODModifier* Clone() override { return new BSPSysLODModifier(*this); }
};

class BSPSysSimpleColorModifier : public NiPSysModifier {
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

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSPSysSimpleColorModifier* Clone() override { return new BSPSysSimpleColorModifier(*this); }
};

class NiPSysRotationModifier : public NiPSysModifier {
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

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiPSysRotationModifier* Clone() override { return new NiPSysRotationModifier(*this); }
};

class BSPSysScaleModifier : public NiPSysModifier {
private:
	uint32_t numFloats = 0;
	std::vector<float> floats;

public:
	static constexpr const char* BlockName = "BSPSysScaleModifier";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSPSysScaleModifier* Clone() override { return new BSPSysScaleModifier(*this); }
};

enum ForceType : uint32_t { FORCE_PLANAR, FORCE_SPHERICAL, FORCE_UNKNOWN };

class NiPSysGravityModifier : public NiPSysModifier {
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

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;
	NiPSysGravityModifier* Clone() override { return new NiPSysGravityModifier(*this); }
};

class NiPSysPositionModifier : public NiPSysModifier {
public:
	static constexpr const char* BlockName = "NiPSysPositionModifier";
	const char* GetBlockName() override { return BlockName; }

	NiPSysPositionModifier* Clone() override { return new NiPSysPositionModifier(*this); }
};

class NiPSysBoundUpdateModifier : public NiPSysModifier {
private:
	uint16_t updateSkip = 0;

public:
	static constexpr const char* BlockName = "NiPSysBoundUpdateModifier";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiPSysBoundUpdateModifier* Clone() override { return new NiPSysBoundUpdateModifier(*this); }
};

class NiPSysDragModifier : public NiPSysModifier {
private:
	BlockRef<NiObject> parentRef;
	Vector3 dragAxis;
	float percentage = 0.0f;
	float range = 0.0f;
	float rangeFalloff = 0.0f;

public:
	static constexpr const char* BlockName = "NiPSysDragModifier";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;
	NiPSysDragModifier* Clone() override { return new NiPSysDragModifier(*this); }
};

class BSPSysInheritVelocityModifier : public NiPSysModifier {
private:
	BlockRef<NiNode> targetNodeRef;
	float changeToInherit = 0.0f;
	float velocityMult = 0.0f;
	float velocityVar = 0.0f;

public:
	static constexpr const char* BlockName = "BSPSysInheritVelocityModifier";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;
	BSPSysInheritVelocityModifier* Clone() override { return new BSPSysInheritVelocityModifier(*this); }
};

class BSPSysSubTexModifier : public NiPSysModifier {
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

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSPSysSubTexModifier* Clone() override { return new BSPSysSubTexModifier(*this); }
};

enum DecayType : uint32_t { DECAY_NONE, DECAY_LINEAR, DECAY_EXPONENTIAL };

enum SymmetryType : uint32_t { SYMMETRY_SPHERICAL, SYMMETRY_CYLINDRICAL, SYMMETRY_PLANAR };

class NiPSysBombModifier : public NiPSysModifier {
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

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;
	NiPSysBombModifier* Clone() override { return new NiPSysBombModifier(*this); }
};

class NiColorData : public NiObject {
private:
	KeyGroup<Color4> data;

public:
	static constexpr const char* BlockName = "NiColorData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiColorData* Clone() override { return new NiColorData(*this); }
};

class NiPSysColorModifier : public NiPSysModifier {
private:
	BlockRef<NiColorData> dataRef;

public:
	static constexpr const char* BlockName = "NiPSysColorModifier";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	NiPSysColorModifier* Clone() override { return new NiPSysColorModifier(*this); }
};

class NiPSysGrowFadeModifier : public NiPSysModifier {
private:
	float growTime = 0.0f;
	uint16_t growGeneration = 0;
	float fadeTime = 0.0f;
	uint16_t fadeGeneration = 0;
	float baseScale = 0.0f;

public:
	static constexpr const char* BlockName = "NiPSysGrowFadeModifier";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiPSysGrowFadeModifier* Clone() override { return new NiPSysGrowFadeModifier(*this); }
};

class NiPSysMeshUpdateModifier : public NiPSysModifier {
private:
	BlockRefArray<NiAVObject> meshRefs;

public:
	static constexpr const char* BlockName = "NiPSysMeshUpdateModifier";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	NiPSysMeshUpdateModifier* Clone() override { return new NiPSysMeshUpdateModifier(*this); }

	BlockRefArray<NiAVObject>& GetMeshes();
};

class NiPSysFieldModifier : public NiPSysModifier {
private:
	BlockRef<NiAVObject> fieldObjectRef;
	float magnitude = 0.0f;
	float attenuation = 0.0f;
	bool useMaxDistance = false;
	float maxDistance = 0.0f;

public:
	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class NiPSysVortexFieldModifier : public NiPSysFieldModifier {
private:
	Vector3 direction;

public:
	static constexpr const char* BlockName = "NiPSysVortexFieldModifier";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiPSysVortexFieldModifier* Clone() override { return new NiPSysVortexFieldModifier(*this); }
};

class NiPSysGravityFieldModifier : public NiPSysFieldModifier {
private:
	Vector3 direction;

public:
	static constexpr const char* BlockName = "NiPSysGravityFieldModifier";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiPSysGravityFieldModifier* Clone() override { return new NiPSysGravityFieldModifier(*this); }
};

class NiPSysDragFieldModifier : public NiPSysFieldModifier {
private:
	bool useDirection = false;
	Vector3 direction;

public:
	static constexpr const char* BlockName = "NiPSysDragFieldModifier";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiPSysDragFieldModifier* Clone() override { return new NiPSysDragFieldModifier(*this); }
};

class NiPSysTurbulenceFieldModifier : public NiPSysFieldModifier {
private:
	float frequency = 0.0f;

public:
	static constexpr const char* BlockName = "NiPSysTurbulenceFieldModifier";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiPSysTurbulenceFieldModifier* Clone() override { return new NiPSysTurbulenceFieldModifier(*this); }
};

class NiPSysAirFieldModifier : public NiPSysFieldModifier {
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

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiPSysAirFieldModifier* Clone() override { return new NiPSysAirFieldModifier(*this); }
};

class NiPSysRadialFieldModifier : public NiPSysFieldModifier {
private:
	uint32_t radialType = 0;

public:
	static constexpr const char* BlockName = "NiPSysRadialFieldModifier";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiPSysRadialFieldModifier* Clone() override { return new NiPSysRadialFieldModifier(*this); }
};

class BSWindModifier : public NiPSysModifier {
private:
	float strength = 0.0f;

public:
	static constexpr const char* BlockName = "BSWindModifier";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSWindModifier* Clone() override { return new BSWindModifier(*this); }
};

class BSPSysRecycleBoundModifier : public NiPSysModifier {
private:
	Vector3 boundOffset;
	Vector3 boundExtent;
	BlockRef<NiNode> targetNodeRef;

public:
	static constexpr const char* BlockName = "BSPSysRecycleBoundModifier";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;
	BSPSysRecycleBoundModifier* Clone() override { return new BSPSysRecycleBoundModifier(*this); }
};

class BSPSysHavokUpdateModifier : public NiPSysModifier {
private:
	BlockRefArray<NiNode> nodeRefs;
	BlockRef<NiPSysModifier> modifierRef;

public:
	static constexpr const char* BlockName = "BSPSysHavokUpdateModifier";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	BSPSysHavokUpdateModifier* Clone() override { return new BSPSysHavokUpdateModifier(*this); }

	BlockRefArray<NiNode>& GetNodes();

	int GetModifierRef();
	void SetModifierRef(int modRef);
};

class BSParentVelocityModifier : public NiPSysModifier {
private:
	float damping = 0.0f;

public:
	static constexpr const char* BlockName = "BSParentVelocityModifier";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSParentVelocityModifier* Clone() override { return new BSParentVelocityModifier(*this); }
};

class BSMasterParticleSystem : public NiNode {
private:
	uint16_t maxEmitterObjs = 0;
	BlockRefArray<NiAVObject> particleSysRefs;

public:
	static constexpr const char* BlockName = "BSMasterParticleSystem";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	BSMasterParticleSystem* Clone() override { return new BSMasterParticleSystem(*this); }

	BlockRefArray<NiAVObject>& GetParticleSystems();
};

class NiParticleSystem : public NiAVObject {
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

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::set<StringRef*>& refs) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	NiParticleSystem* Clone() override { return new NiParticleSystem(*this); }

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

class NiMeshParticleSystem : public NiParticleSystem {
public:
	static constexpr const char* BlockName = "NiMeshParticleSystem";
	const char* GetBlockName() override { return BlockName; }

	NiMeshParticleSystem* Clone() override { return new NiMeshParticleSystem(*this); }
};

class BSStripParticleSystem : public NiParticleSystem {
public:
	static constexpr const char* BlockName = "BSStripParticleSystem";
	const char* GetBlockName() override { return BlockName; }

	BSStripParticleSystem* Clone() override { return new BSStripParticleSystem(*this); }
};

class NiPSysColliderManager;

class NiPSysCollider : public NiObject {
private:
	float bounce = 0.0f;
	bool spawnOnCollide = false;
	bool dieOnCollide = false;
	BlockRef<NiPSysSpawnModifier> spawnModifierRef;
	BlockRef<NiPSysColliderManager> managerRef;
	BlockRef<NiPSysCollider> nextColliderRef;
	BlockRef<NiNode> colliderNodeRef;

public:
	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;
};

class NiPSysSphericalCollider : public NiPSysCollider {
private:
	float radius = 0.0f;

public:
	static constexpr const char* BlockName = "NiPSysSphericalCollider";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiPSysSphericalCollider* Clone() override { return new NiPSysSphericalCollider(*this); }
};

class NiPSysPlanarCollider : public NiPSysCollider {
private:
	float width = 0.0f;
	float height = 0.0f;
	Vector3 xAxis;
	Vector3 yAxis;

public:
	static constexpr const char* BlockName = "NiPSysPlanarCollider";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiPSysPlanarCollider* Clone() override { return new NiPSysPlanarCollider(*this); }
};

class NiPSysColliderManager : public NiPSysModifier {
private:
	BlockRef<NiPSysCollider> colliderRef;

public:
	static constexpr const char* BlockName = "NiPSysColliderManager";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	NiPSysColliderManager* Clone() override { return new NiPSysColliderManager(*this); }
};

class NiPSysEmitter : public NiPSysModifier {
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
	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
};

class NiPSysVolumeEmitter : public NiPSysEmitter {
private:
	BlockRef<NiNode> emitterNodeRef;

public:
	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;
};

class NiPSysSphereEmitter : public NiPSysVolumeEmitter {
private:
	float radius = 0.0f;

public:
	static constexpr const char* BlockName = "NiPSysSphereEmitter";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiPSysSphereEmitter* Clone() override { return new NiPSysSphereEmitter(*this); }
};

class NiPSysCylinderEmitter : public NiPSysVolumeEmitter {
private:
	float radius = 0.0f;
	float height = 0.0f;

public:
	static constexpr const char* BlockName = "NiPSysCylinderEmitter";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiPSysCylinderEmitter* Clone() override { return new NiPSysCylinderEmitter(*this); }
};

class NiPSysBoxEmitter : public NiPSysVolumeEmitter {
private:
	float width = 0.0f;
	float height = 0.0f;
	float depth = 0.0f;

public:
	static constexpr const char* BlockName = "NiPSysBoxEmitter";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiPSysBoxEmitter* Clone() override { return new NiPSysBoxEmitter(*this); }
};

class BSPSysArrayEmitter : public NiPSysVolumeEmitter {
public:
	static constexpr const char* BlockName = "BSPSysArrayEmitter";
	const char* GetBlockName() override { return BlockName; }

	BSPSysArrayEmitter* Clone() override { return new BSPSysArrayEmitter(*this); }
};

enum VelocityType : uint32_t { VELOCITY_USE_NORMALS, VELOCITY_USE_RANDOM, VELOCITY_USE_DIRECTION };

enum EmitFrom : uint32_t {
	EMIT_FROM_VERTICES,
	EMIT_FROM_FACE_CENTER,
	EMIT_FROM_EDGE_CENTER,
	EMIT_FROM_FACE_SURFACE,
	EMIT_FROM_EDGE_SURFACE
};

class NiPSysMeshEmitter : public NiPSysEmitter {
private:
	BlockRefArray<NiAVObject> meshRefs;
	VelocityType velocityType = VELOCITY_USE_NORMALS;
	EmitFrom emissionType = EMIT_FROM_VERTICES;
	Vector3 emissionAxis;

public:
	static constexpr const char* BlockName = "NiPSysMeshEmitter";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	NiPSysMeshEmitter* Clone() override { return new NiPSysMeshEmitter(*this); }

	BlockRefArray<NiAVObject>& GetMeshes();
};
} // namespace nifly
