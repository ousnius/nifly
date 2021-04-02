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
class NiExtraData : public NiObjectCRTP<NiExtraData, NiObject, true> {
public:
	NiStringRef name;

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
};

class NiBinaryExtraData : public NiObjectCRTP<NiBinaryExtraData, NiExtraData, true> {
public:
	NiVector<uint8_t> data;

	static constexpr const char* BlockName = "NiBinaryExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiFloatExtraData : public NiObjectCRTP<NiFloatExtraData, NiExtraData, true> {
public:
	float floatData = 0.0f;

	static constexpr const char* BlockName = "NiFloatExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiFloatsExtraData : public NiObjectCRTP<NiFloatsExtraData, NiExtraData, true> {
public:
	NiVector<float> floatsData;

	static constexpr const char* BlockName = "NiFloatsExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiStringExtraData : public NiObjectCRTP<NiStringExtraData, NiExtraData, true> {
public:
	NiStringRef stringData;

	static constexpr const char* BlockName = "NiStringExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
};

class NiStringsExtraData : public NiObjectCRTP<NiStringsExtraData, NiExtraData, true> {
public:
	NiStringVector<> stringsData;

	static constexpr const char* BlockName = "NiStringsExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiBooleanExtraData : public NiObjectCRTP<NiBooleanExtraData, NiExtraData, true> {
public:
	bool booleanData = false;

	static constexpr const char* BlockName = "NiBooleanExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiIntegerExtraData : public NiObjectCRTP<NiIntegerExtraData, NiExtraData, true> {
public:
	uint32_t integerData = 0;

	static constexpr const char* BlockName = "NiIntegerExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiIntegersExtraData : public NiObjectCRTP<NiIntegersExtraData, NiExtraData, true> {
public:
	NiVector<uint32_t> integersData;

	static constexpr const char* BlockName = "NiIntegersExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiVectorExtraData : public NiObjectCRTP<NiVectorExtraData, NiExtraData, true> {
public:
	Vector4 vectorData;

	static constexpr const char* BlockName = "NiVectorExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiColorExtraData : public NiObjectCRTP<NiColorExtraData, NiExtraData, true> {
public:
	Color4 colorData;

	static constexpr const char* BlockName = "NiColorExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSXFlags : public NiObjectCRTP<BSXFlags, NiIntegerExtraData> {
public:
	static constexpr const char* BlockName = "BSXFlags";
	const char* GetBlockName() override { return BlockName; }
};

class BSWArray : public NiObjectCRTP<BSWArray, NiExtraData, true> {
public:
	NiVector<uint32_t> data;

	static constexpr const char* BlockName = "BSWArray";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSPositionData : public NiObjectCRTP<BSPositionData, NiExtraData, true> {
public:
	NiVector<half_float::half> data;

	static constexpr const char* BlockName = "BSPositionData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSEyeCenterExtraData : public NiObjectCRTP<BSEyeCenterExtraData, NiExtraData, true> {
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
	: public NiObjectCRTP<BSPackedCombinedSharedGeomDataExtra, NiExtraData, true> {
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

class BSInvMarker : public NiObjectCRTP<BSInvMarker, NiExtraData, true> {
public:
	uint16_t rotationX = 4712;
	uint16_t rotationY = 6283;
	uint16_t rotationZ = 0;
	float zoom = 1.0f;

	static constexpr const char* BlockName = "BSInvMarker";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

struct FurniturePosition {
	Vector3 offset;

	uint16_t orientation = 0; // User Version <= 11
	uint8_t posRef1 = 0;	  // User Version <= 11
	uint8_t posRef2 = 0;	  // User Version <= 11

	float heading = 0.0f;		// User Version >= 12
	uint16_t animationType = 0; // User Version >= 12
	uint16_t entryPoints = 0;	// User Version >= 12
};

class BSFurnitureMarker : public NiObjectCRTP<BSFurnitureMarker, NiExtraData, true> {
private:
	uint32_t numPositions = 0;
	std::vector<FurniturePosition> positions;

public:
	static constexpr const char* BlockName = "BSFurnitureMarker";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	std::vector<FurniturePosition> GetPositions();
	void SetPositions(const std::vector<FurniturePosition>& pos);
};

class BSFurnitureMarkerNode : public NiObjectCRTP<BSFurnitureMarkerNode, BSFurnitureMarker> {
public:
	static constexpr const char* BlockName = "BSFurnitureMarkerNode";
	const char* GetBlockName() override { return BlockName; }
};

struct DecalVectorBlock {
	uint16_t numVectors = 0;
	std::vector<Vector3> points;
	std::vector<Vector3> normals;
};

class BSDecalPlacementVectorExtraData
	: public NiObjectCRTP<BSDecalPlacementVectorExtraData, NiFloatExtraData, true> {
private:
	uint16_t numVectorBlocks = 0;
	std::vector<DecalVectorBlock> decalVectorBlocks;

public:
	static constexpr const char* BlockName = "BSDecalPlacementVectorExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	std::vector<DecalVectorBlock> GetDecalVectorBlocks();
	void SetDecalVectorBlocks(const std::vector<DecalVectorBlock>& vectorBlocks);
};

class BSBehaviorGraphExtraData : public NiObjectCRTP<BSBehaviorGraphExtraData, NiExtraData, true> {
public:
	NiStringRef behaviorGraphFile;
	bool controlsBaseSkel = false;

	static constexpr const char* BlockName = "BSBehaviorGraphExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
};

class BSBound : public NiObjectCRTP<BSBound, NiExtraData, true> {
public:
	Vector3 center;
	Vector3 halfExtents;

	static constexpr const char* BlockName = "BSBound";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

struct BoneLOD {
	uint32_t distance = 0;
	NiStringRef boneName;
};

class BSBoneLODExtraData : public NiObjectCRTP<BSBoneLODExtraData, NiExtraData, true> {
private:
	uint32_t numBoneLODs = 0;
	std::vector<BoneLOD> boneLODs;

public:
	static constexpr const char* BlockName = "BSBoneLODExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;

	std::vector<BoneLOD> GetBoneLODs();
	void SetBoneLODs(const std::vector<BoneLOD>& lods);
};

class NiTextKeyExtraData : public NiObjectCRTP<NiTextKeyExtraData, NiExtraData, true> {
private:
	uint32_t numTextKeys = 0;
	std::vector<Key<NiStringRef>> textKeys;

public:
	static constexpr const char* BlockName = "NiTextKeyExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;

	std::vector<Key<NiStringRef>> GetTextKeys();
	void SetTextKeys(const std::vector<Key<NiStringRef>>& keys);
};

class BSDistantObjectLargeRefExtraData
	: public NiObjectCRTP<BSDistantObjectLargeRefExtraData, NiExtraData, true> {
public:
	bool largeRef = true;

	static constexpr const char* BlockName = "BSDistantObjectLargeRefExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

struct BSConnectPoint {
	NiString root;
	NiString variableName;
	Quaternion rotation;
	Vector3 translation;
	float scale = 1.0f;

	void Sync(NiStreamReversible& stream);
};

class BSConnectPointParents : public NiObjectCRTP<BSConnectPointParents, NiExtraData, true> {
private:
	uint32_t numConnectPoints = 0;
	std::vector<BSConnectPoint> connectPoints;

public:
	static constexpr const char* BlockName = "BSConnectPoint::Parents";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	std::vector<BSConnectPoint> GetConnectPoints();
	void SetConnectPoints(const std::vector<BSConnectPoint>& cps);
};

class BSConnectPointChildren : public NiObjectCRTP<BSConnectPointChildren, NiExtraData, true> {
public:
	bool skinned = true;
	NiStringVector<> targets;

	static constexpr const char* BlockName = "BSConnectPoint::Children";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSExtraData : public NiObjectCRTP<BSExtraData, NiObject> {};

class BSClothExtraData : public NiObjectCRTP<BSClothExtraData, BSExtraData, true> {
private:
	uint32_t numBytes = 0;
	std::vector<char> data;

public:
	BSClothExtraData() {}
	BSClothExtraData(const uint32_t size);

	static constexpr const char* BlockName = "BSClothExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	std::vector<char> GetData();
	void SetData(const std::vector<char>& dat);

	bool ToHKX(const std::string& fileName);
	bool FromHKX(const std::string& fileName);
};
} // namespace nifly
