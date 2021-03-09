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
class NiExtraData : public CloneInherit<NiExtraData, NiObject> {
private:
	StringRef name;

public:
	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::vector<StringRef*>& refs) override;

	std::string GetName();
	void SetName(const std::string& extraDataName);
};

class NiBinaryExtraData : public CloneInherit<NiBinaryExtraData, NiExtraData> {
private:
	uint32_t size = 0;
	std::vector<uint8_t> data;

public:
	static constexpr const char* BlockName = "NiBinaryExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	std::vector<uint8_t> GetData();
	void SetData(const std::vector<uint8_t>& dat);
};

class NiFloatExtraData : public CloneInherit<NiFloatExtraData, NiExtraData> {
private:
	float floatData = 0.0f;

public:
	static constexpr const char* BlockName = "NiFloatExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	float GetFloatData();
	void SetFloatData(const float fltData);
};

class NiFloatsExtraData : public CloneInherit<NiFloatsExtraData, NiExtraData> {
private:
	uint32_t numFloats = 0;
	std::vector<float> floatsData;

public:
	static constexpr const char* BlockName = "NiFloatsExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	std::vector<float> GetFloatsData();
	void SetFloatsData(const std::vector<float>& fltsData);
};

class NiStringExtraData : public CloneInherit<NiStringExtraData, NiExtraData> {
private:
	StringRef stringData;

public:
	static constexpr const char* BlockName = "NiStringExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::vector<StringRef*>& refs) override;

	std::string GetStringData();
	void SetStringData(const std::string& str);
};

class NiStringsExtraData : public CloneInherit<NiStringsExtraData, NiExtraData> {
private:
	uint32_t numStrings = 0;
	std::vector<NiString> stringsData;

public:
	static constexpr const char* BlockName = "NiStringsExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	std::vector<NiString> GetStringsData();
	void SetStringsData(const std::vector<NiString>& strsData);
};

class NiBooleanExtraData : public CloneInherit<NiBooleanExtraData, NiExtraData> {
private:
	bool booleanData = false;

public:
	static constexpr const char* BlockName = "NiBooleanExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	bool GetBooleanData();
	void SetBooleanData(const bool boolData);
};

class NiIntegerExtraData : public CloneInherit<NiIntegerExtraData, NiExtraData> {
private:
	uint32_t integerData = 0;

public:
	static constexpr const char* BlockName = "NiIntegerExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	uint32_t GetIntegerData();
	void SetIntegerData(const uint32_t intData);
};

class NiIntegersExtraData : public CloneInherit<NiIntegersExtraData, NiExtraData> {
private:
	uint32_t numIntegers = 0;
	std::vector<uint32_t> integersData;

public:
	static constexpr const char* BlockName = "NiIntegersExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	std::vector<uint32_t> GetIntegersData();
	void SetIntegersData(const std::vector<uint32_t>& intData);
};

class NiVectorExtraData : public CloneInherit<NiVectorExtraData, NiExtraData> {
private:
	Vector4 vectorData;

public:
	static constexpr const char* BlockName = "NiVectorExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	Vector4 GetVectorData();
	void SetVectorData(const Vector4& vecData);
};

class NiColorExtraData : public CloneInherit<NiColorExtraData, NiExtraData> {
private:
	Color4 colorData;

public:
	static constexpr const char* BlockName = "NiColorExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	Color4 GetColorData();
	void SetColorData(const Color4& colData);
};

class BSXFlags : public CloneInherit<BSXFlags, NiIntegerExtraData> {
public:
	static constexpr const char* BlockName = "BSXFlags";
	const char* GetBlockName() override { return BlockName; }
};

class BSWArray : public CloneInherit<BSWArray, NiExtraData> {
private:
	uint32_t numData = 0;
	std::vector<uint32_t> data;

public:
	static constexpr const char* BlockName = "BSWArray";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	std::vector<uint32_t> GetData();
	void SetData(const std::vector<uint32_t>& dat);
};

class BSPositionData : public CloneInherit<BSPositionData, NiExtraData> {
private:
	uint32_t numData = 0;
	std::vector<half_float::half> data;

public:
	static constexpr const char* BlockName = "BSPositionData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	std::vector<half_float::half> GetData();
	void SetData(const std::vector<half_float::half>& dat);
};

class BSEyeCenterExtraData : public CloneInherit<BSEyeCenterExtraData, NiExtraData> {
private:
	uint32_t numData = 0;
	std::vector<float> data;

public:
	static constexpr const char* BlockName = "BSEyeCenterExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	std::vector<float> GetData();
	void SetData(const std::vector<float>& dat);
};

struct BSPackedGeomObject {
	uint32_t unkInt1 = 0;
	uint32_t objectHash = 0;
};

struct BSPackedGeomDataLOD {
	uint32_t triangleCount = 0;
	uint32_t triangleOffset = 0;
};

struct BSPackedGeomDataCombined {
	float grayscaleToPaletteScale = 1.0f;
	Matrix3 rotation;
	Vector3 translation;
	float scale = 1.0f;
	BoundingSphere bounds;
};

struct BSPackedGeomData {
	uint32_t numVerts = 0;
	uint32_t lodLevels = 0;
	std::vector<BSPackedGeomDataLOD> lod;
	uint32_t numCombined = 0;
	std::vector<BSPackedGeomDataCombined> combined;
	uint32_t unkInt1 = 0;
	uint32_t unkInt2 = 0;

	void Get(NiStream& stream);
	void Put(NiStream& stream);
};

class BSPackedCombinedSharedGeomDataExtra : public CloneInherit<BSPackedCombinedSharedGeomDataExtra, NiExtraData> {
private:
	VertexDesc vertDesc;
	uint32_t numVertices = 0;
	uint32_t numTriangles = 0;
	uint32_t unkFlags1 = 0;
	uint32_t unkFlags2 = 0;
	uint32_t numData = 0;
	std::vector<BSPackedGeomObject> objects;
	std::vector<BSPackedGeomData> data;

public:
	static constexpr const char* BlockName = "BSPackedCombinedSharedGeomDataExtra";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
};

class BSInvMarker : public CloneInherit<BSInvMarker, NiExtraData> {
private:
	uint16_t rotationX = 4712;
	uint16_t rotationY = 6283;
	uint16_t rotationZ = 0;
	float zoom = 1.0f;

public:
	static constexpr const char* BlockName = "BSInvMarker";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	uint16_t GetRotationX() { return rotationX; }
	void SetRotationX(const uint16_t x) { rotationX = x; }

	uint16_t GetRotationY() { return rotationY; }
	void SetRotationY(const uint16_t y) { rotationY = y; }

	uint16_t GetRotationZ() { return rotationZ; }
	void SetRotationZ(const uint16_t z) { rotationZ = z; }

	float GetZoom() { return zoom; }
	void SetZoom(const float z) { zoom = z; }
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

class BSFurnitureMarker : public CloneInherit<BSFurnitureMarker, NiExtraData> {
private:
	uint32_t numPositions = 0;
	std::vector<FurniturePosition> positions;

public:
	static constexpr const char* BlockName = "BSFurnitureMarker";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	std::vector<FurniturePosition> GetPositions();
	void SetPositions(const std::vector<FurniturePosition>& pos);
};

class BSFurnitureMarkerNode : public CloneInherit<BSFurnitureMarkerNode, BSFurnitureMarker> {
public:
	static constexpr const char* BlockName = "BSFurnitureMarkerNode";
	const char* GetBlockName() override { return BlockName; }
};

struct DecalVectorBlock {
	uint16_t numVectors = 0;
	std::vector<Vector3> points;
	std::vector<Vector3> normals;
};

class BSDecalPlacementVectorExtraData : public CloneInherit<BSDecalPlacementVectorExtraData, NiFloatExtraData> {
private:
	uint16_t numVectorBlocks = 0;
	std::vector<DecalVectorBlock> decalVectorBlocks;

public:
	static constexpr const char* BlockName = "BSDecalPlacementVectorExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	std::vector<DecalVectorBlock> GetDecalVectorBlocks();
	void SetDecalVectorBlocks(const std::vector<DecalVectorBlock>& vectorBlocks);
};

class BSBehaviorGraphExtraData : public CloneInherit<BSBehaviorGraphExtraData, NiExtraData> {
private:
	StringRef behaviorGraphFile;
	bool controlsBaseSkel = false;

public:
	static constexpr const char* BlockName = "BSBehaviorGraphExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::vector<StringRef*>& refs) override;
};

class BSBound : public CloneInherit<BSBound, NiExtraData> {
private:
	Vector3 center;
	Vector3 halfExtents;

public:
	static constexpr const char* BlockName = "BSBound";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	Vector3 GetCenter() { return center; }
	void SetCenter(const Vector3& ctr) { center = ctr; }

	Vector3 GetHalfExtents() { return halfExtents; }
	void SetHalfExtents(const Vector3& hExtents) { halfExtents = hExtents; }
};

struct BoneLOD {
	uint32_t distance = 0;
	StringRef boneName;
};

class BSBoneLODExtraData : public CloneInherit<BSBoneLODExtraData, NiExtraData> {
private:
	uint32_t numBoneLODs = 0;
	std::vector<BoneLOD> boneLODs;

public:
	static constexpr const char* BlockName = "BSBoneLODExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::vector<StringRef*>& refs) override;

	std::vector<BoneLOD> GetBoneLODs();
	void SetBoneLODs(const std::vector<BoneLOD>& lods);
};

class NiTextKeyExtraData : public CloneInherit<NiTextKeyExtraData, NiExtraData> {
private:
	uint32_t numTextKeys = 0;
	std::vector<Key<StringRef>> textKeys;

public:
	static constexpr const char* BlockName = "NiTextKeyExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::vector<StringRef*>& refs) override;

	std::vector<Key<StringRef>> GetTextKeys();
	void SetTextKeys(const std::vector<Key<StringRef>>& keys);
};

class BSDistantObjectLargeRefExtraData : public CloneInherit<BSDistantObjectLargeRefExtraData, NiExtraData> {
private:
	bool largeRef = true;

public:
	static constexpr const char* BlockName = "BSDistantObjectLargeRefExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	bool IsLargeRef() { return largeRef; }
	void SetLargeRef(const bool isLargeRef) { largeRef = isLargeRef; }
};

struct BSConnectPoint {
	NiString root;
	NiString variableName;
	Quaternion rotation;
	Vector3 translation;
	float scale = 1.0f;

	void Get(NiStream& stream);
	void Put(NiStream& stream);
};

class BSConnectPointParents : public CloneInherit<BSConnectPointParents, NiExtraData> {
private:
	uint32_t numConnectPoints = 0;
	std::vector<BSConnectPoint> connectPoints;

public:
	static constexpr const char* BlockName = "BSConnectPoint::Parents";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	std::vector<BSConnectPoint> GetConnectPoints();
	void SetConnectPoints(const std::vector<BSConnectPoint>& cps);
};

class BSConnectPointChildren : public CloneInherit<BSConnectPointChildren, NiExtraData> {
private:
	uint8_t unkByte = 1;
	uint32_t numTargets = 0;
	std::vector<NiString> targets;

public:
	static constexpr const char* BlockName = "BSConnectPoint::Children";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	std::vector<NiString> GetTargets();
	void SetTargets(const std::vector<NiString>& targ);
};

class BSExtraData : public CloneInherit<BSExtraData, NiObject> {};

class BSClothExtraData : public CloneInherit<BSClothExtraData, BSExtraData> {
private:
	uint32_t numBytes = 0;
	std::vector<char> data;

public:
	BSClothExtraData() {}
	BSClothExtraData(const uint32_t size);

	static constexpr const char* BlockName = "BSClothExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	std::vector<char> GetData();
	void SetData(const std::vector<char>& dat);

	bool ToHKX(const std::string& fileName);
	bool FromHKX(const std::string& fileName);
};
} // namespace nifly
