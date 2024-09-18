/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#pragma once

#include "BasicTypes.hpp"
#include "Keys.hpp"
#include "VertexData.hpp"
#include "half.hpp"

namespace nifly {
class NiExtraData : public NiCloneableStreamable<NiExtraData, NiObject> {
public:
	NiStringRef name;

	static constexpr const char* BlockName = "NiExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
};

class NiBinaryExtraData : public NiCloneableStreamable<NiBinaryExtraData, NiExtraData> {
public:
	NiVector<uint8_t> data;

	static constexpr const char* BlockName = "NiBinaryExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiFloatExtraData : public NiCloneableStreamable<NiFloatExtraData, NiExtraData> {
public:
	float floatData = 0.0f;

	static constexpr const char* BlockName = "NiFloatExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiFloatsExtraData : public NiCloneableStreamable<NiFloatsExtraData, NiExtraData> {
public:
	NiVector<float> floatsData;

	static constexpr const char* BlockName = "NiFloatsExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiStringExtraData : public NiCloneableStreamable<NiStringExtraData, NiExtraData> {
public:
	NiStringRef stringData;

	static constexpr const char* BlockName = "NiStringExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
};

class NiStringsExtraData : public NiCloneableStreamable<NiStringsExtraData, NiExtraData> {
public:
	NiStringVector<> stringsData;

	static constexpr const char* BlockName = "NiStringsExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiBooleanExtraData : public NiCloneableStreamable<NiBooleanExtraData, NiExtraData> {
public:
	bool booleanData = false;

	static constexpr const char* BlockName = "NiBooleanExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiIntegerExtraData : public NiCloneableStreamable<NiIntegerExtraData, NiExtraData> {
public:
	uint32_t integerData = 0;

	static constexpr const char* BlockName = "NiIntegerExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiIntegersExtraData : public NiCloneableStreamable<NiIntegersExtraData, NiExtraData> {
public:
	NiVector<uint32_t> integersData;

	static constexpr const char* BlockName = "NiIntegersExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiVectorExtraData : public NiCloneableStreamable<NiVectorExtraData, NiExtraData> {
public:
	Vector4 vectorData;

	static constexpr const char* BlockName = "NiVectorExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiColorExtraData : public NiCloneableStreamable<NiColorExtraData, NiExtraData> {
public:
	Color4 colorData;

	static constexpr const char* BlockName = "NiColorExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

enum BSXFlagsEnum : uint32_t {
	BSX_ANIMATED = 1 << 0,
	BSX_HAVOK = 1 << 1,
	BSX_RAGDOLL = 1 << 2,
	BSX_COMPLEX = 1 << 3,
	BSX_ADDON = 1 << 4,
	BSX_EDITOR_MARKER = 1 << 5,
	BSX_DYNAMIC = 1 << 6,
	BSX_ARTICULATED = 1 << 7,
	BSX_NEEDS_TRANSFORM_UPDATES = 1 << 8,
	BSX_EXTERNAL_EMITTANCE = 1 << 9
};

class BSXFlags : public NiCloneable<BSXFlags, NiIntegerExtraData> {
public:
	static constexpr const char* BlockName = "BSXFlags";
	const char* GetBlockName() override { return BlockName; }
};

class BSWArray : public NiCloneableStreamable<BSWArray, NiExtraData> {
public:
	NiVector<uint32_t> data;

	static constexpr const char* BlockName = "BSWArray";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSPositionData : public NiCloneableStreamable<BSPositionData, NiExtraData> {
public:
	NiVector<half_float::half> data;

	static constexpr const char* BlockName = "BSPositionData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSEyeCenterExtraData : public NiCloneableStreamable<BSEyeCenterExtraData, NiExtraData> {
public:
	NiVector<float> data;

	static constexpr const char* BlockName = "BSEyeCenterExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

struct BSPackedGeomObject {
	uint32_t fileNameHash = 0;
	uint32_t dataOffset = 0;
};

struct BSPackedGeomDataCombined {
	float grayscaleToPaletteScale = 1.0f;
	Matrix3 rotation;
	Vector3 translation;
	float scale = 1.0f;
	BoundingSphere bounds;
};

struct BSPackedGeomData {
	uint32_t numVertices = 0;
	uint32_t lodLevels = 0;
	uint32_t triCountLod0 = 0;
	uint32_t triOffsetLod0 = 0;
	uint32_t triCountLod1 = 0;
	uint32_t triOffsetLod1 = 0;
	uint32_t triCountLod2 = 0;
	uint32_t triOffsetLod2 = 0;
	NiVector<BSPackedGeomDataCombined> combined;
	VertexDesc vertexDesc;
	std::vector<BSVertexData> vertData;
	std::vector<Triangle> triangles;

	void Sync(NiStreamReversible& stream);

	void SetVertices(const bool enable);
	bool HasVertices() const { return vertexDesc.HasFlag(VF_VERTEX); }

	void SetUVs(const bool enable);
	bool HasUVs() const { return vertexDesc.HasFlag(VF_UV); }

	void SetSecondUVs(const bool enable);
	bool HasSecondUVs() { return vertexDesc.HasFlag(VF_UV_2); }

	void SetNormals(const bool enable);
	bool HasNormals() const { return vertexDesc.HasFlag(VF_NORMAL); }

	void SetTangents(const bool enable);
	bool HasTangents() const { return vertexDesc.HasFlag(VF_TANGENT); }

	void SetVertexColors(const bool enable);
	bool HasVertexColors() const { return vertexDesc.HasFlag(VF_COLORS); }

	void SetSkinned(const bool enable);
	bool IsSkinned() const { return vertexDesc.HasFlag(VF_SKINNED); }

	void SetEyeData(const bool enable);
	bool HasEyeData() const { return vertexDesc.HasFlag(VF_EYEDATA); }

	void SetFullPrecision(const bool enable);
	bool IsFullPrecision() const { return vertexDesc.HasFlag(VF_FULLPREC); }
	bool CanChangePrecision() const { return (HasVertices()); }
};

class BSPackedCombinedSharedGeomDataExtra
	: public NiCloneableStreamable<BSPackedCombinedSharedGeomDataExtra, NiExtraData> {
public:
	VertexDesc vertexDesc;
	uint32_t numVertices = 0;
	uint32_t numTriangles = 0;
	uint32_t unkFlags1 = 0;
	uint32_t unkFlags2 = 0;
	uint32_t numData = 0;
	std::vector<BSPackedGeomObject> objects;
	std::vector<BSPackedGeomData> data;

	static constexpr const char* BlockName = "BSPackedCombinedSharedGeomDataExtra";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSInvMarker : public NiCloneableStreamable<BSInvMarker, NiExtraData> {
public:
	uint16_t rotationX = 4712;
	uint16_t rotationY = 6283;
	uint16_t rotationZ = 0;
	float zoom = 1.0f;

	static constexpr const char* BlockName = "BSInvMarker";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class FurniturePosition {
public:
	Vector3 offset;

	uint16_t orientation = 0; // User Version <= 11
	uint8_t posRef1 = 0;	  // User Version <= 11
	uint8_t posRef2 = 0;	  // User Version <= 11

	float heading = 0.0f;		// User Version >= 12
	uint16_t animationType = 0; // User Version >= 12
	uint16_t entryPoints = 0;	// User Version >= 12

	void Sync(NiStreamReversible& stream);
};

class BSFurnitureMarker : public NiCloneableStreamable<BSFurnitureMarker, NiExtraData> {
public:
	NiSyncVector<FurniturePosition> positions;

	static constexpr const char* BlockName = "BSFurnitureMarker";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSFurnitureMarkerNode : public NiCloneable<BSFurnitureMarkerNode, BSFurnitureMarker> {
public:
	static constexpr const char* BlockName = "BSFurnitureMarkerNode";
	const char* GetBlockName() override { return BlockName; }
};

class DecalVectorBlock {
public:
	NiVector<Vector3, uint16_t> points;
	NiVector<Vector3, uint16_t> normals;

	void Sync(NiStreamReversible&);
};

class BSDecalPlacementVectorExtraData
	: public NiCloneableStreamable<BSDecalPlacementVectorExtraData, NiFloatExtraData> {
public:
	NiSyncVector<DecalVectorBlock, uint16_t> decalVectorBlocks;

	static constexpr const char* BlockName = "BSDecalPlacementVectorExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSBehaviorGraphExtraData : public NiCloneableStreamable<BSBehaviorGraphExtraData, NiExtraData> {
public:
	NiStringRef behaviorGraphFile;
	bool controlsBaseSkel = false;

	static constexpr const char* BlockName = "BSBehaviorGraphExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
};

class BSBound : public NiCloneableStreamable<BSBound, NiExtraData> {
public:
	Vector3 center;
	Vector3 halfExtents;

	static constexpr const char* BlockName = "BSBound";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BoneLOD {
public:
	uint32_t distance = 0;
	NiStringRef boneName;

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs);
};

class BSBoneLODExtraData : public NiCloneableStreamable<BSBoneLODExtraData, NiExtraData> {
public:
	NiSyncVector<BoneLOD> boneLODs;

	static constexpr const char* BlockName = "BSBoneLODExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
};

class NiTextKeyExtraData : public NiCloneableStreamable<NiTextKeyExtraData, NiExtraData> {
public:
	NiSyncVector<NiTextKey> textKeys;

	static constexpr const char* BlockName = "NiTextKeyExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
};

class BSDistantObjectLargeRefExtraData
	: public NiCloneableStreamable<BSDistantObjectLargeRefExtraData, NiExtraData> {
public:
	bool largeRef = true;

	static constexpr const char* BlockName = "BSDistantObjectLargeRefExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSDistantObjectExtraData
	: public NiCloneableStreamable<BSDistantObjectExtraData, NiExtraData> {
public:
	uint32_t distantObjectFlags = 0;

	static constexpr const char* BlockName = "BSDistantObjectExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSConnectPoint {
public:
	NiString root;
	NiString variableName;
	Quaternion rotation;
	Vector3 translation;
	float scale = 1.0f;

	void Sync(NiStreamReversible& stream);
};

class BSConnectPointParents : public NiCloneableStreamable<BSConnectPointParents, NiExtraData> {
public:
	NiSyncVector<BSConnectPoint> connectPoints;

	static constexpr const char* BlockName = "BSConnectPoint::Parents";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSConnectPointChildren : public NiCloneableStreamable<BSConnectPointChildren, NiExtraData> {
public:
	bool skinned = true;
	NiStringVector<> targets;

	static constexpr const char* BlockName = "BSConnectPoint::Children";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSExtraData : public NiCloneable<BSExtraData, NiObject> {};

class BSClothExtraData : public NiCloneableStreamable<BSClothExtraData, BSExtraData> {
public:
	NiVector<char> data;

	BSClothExtraData() {}
	BSClothExtraData(const uint32_t size);

	static constexpr const char* BlockName = "BSClothExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	bool ToHKX(const std::string& fileName);
	bool FromHKX(const std::string& fileName);
};

class BSCollisionQueryProxyExtraData : public NiCloneableStreamable<BSCollisionQueryProxyExtraData, BSExtraData> {
public:
	NiVector<char> data;

	static constexpr const char* BlockName = "BSCollisionQueryProxyExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class SkinAttach : public NiCloneableStreamable<SkinAttach, NiExtraData> {
public:
	NiStringVector<> bones;

	static constexpr const char* BlockName = "SkinAttach";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

struct BoneTranslation {
	NiString bone;
	Vector3 trans;
};

class BoneTranslations : public NiCloneableStreamable<BoneTranslations, NiExtraData> {
public:
	uint32_t numTranslations = 0;
	std::vector<BoneTranslation> translations;

	static constexpr const char* BlockName = "BoneTranslations";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};
} // namespace nifly
