/*
BodySlide and Outfit Studio
See the included LICENSE file
*/

#pragma once

#include "BasicTypes.hpp"
#include "Keys.hpp"
#include "VertexData.hpp"
#include "half.hpp"

namespace nifly {
class NiExtraData : public NiObject {
private:
	StringRef name;

public:
	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::set<StringRef*>& refs) override;

	std::string GetName();
	void SetName(const std::string& extraDataName);
};

class NiBinaryExtraData : public NiExtraData {
private:
	uint32_t size = 0;
	std::vector<uint8_t> data;

public:
	static constexpr const char* BlockName = "NiBinaryExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiBinaryExtraData* Clone() override { return new NiBinaryExtraData(*this); }

	std::vector<uint8_t> GetData();
	void SetData(const std::vector<uint8_t>& dat);
};

class NiFloatExtraData : public NiExtraData {
private:
	float floatData = 0.0f;

public:
	static constexpr const char* BlockName = "NiFloatExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiFloatExtraData* Clone() override { return new NiFloatExtraData(*this); }

	float GetFloatData();
	void SetFloatData(const float fltData);
};

class NiFloatsExtraData : public NiExtraData {
private:
	uint32_t numFloats = 0;
	std::vector<float> floatsData;

public:
	static constexpr const char* BlockName = "NiFloatsExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiFloatsExtraData* Clone() override { return new NiFloatsExtraData(*this); }

	std::vector<float> GetFloatsData();
	void SetFloatsData(const std::vector<float>& fltsData);
};

class NiStringExtraData : public NiExtraData {
private:
	StringRef stringData;

public:
	static constexpr const char* BlockName = "NiStringExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::set<StringRef*>& refs) override;
	NiStringExtraData* Clone() override { return new NiStringExtraData(*this); }

	std::string GetStringData();
	void SetStringData(const std::string& str);
};

class NiStringsExtraData : public NiExtraData {
private:
	uint32_t numStrings = 0;
	std::vector<NiString> stringsData;

public:
	static constexpr const char* BlockName = "NiStringsExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiStringsExtraData* Clone() override { return new NiStringsExtraData(*this); }

	std::vector<NiString> GetStringsData();
	void SetStringsData(const std::vector<NiString>& strsData);
};

class NiBooleanExtraData : public NiExtraData {
private:
	bool booleanData = false;

public:
	static constexpr const char* BlockName = "NiBooleanExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiBooleanExtraData* Clone() override { return new NiBooleanExtraData(*this); }

	bool GetBooleanData();
	void SetBooleanData(const bool boolData);
};

class NiIntegerExtraData : public NiExtraData {
private:
	uint32_t integerData = 0;

public:
	static constexpr const char* BlockName = "NiIntegerExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiIntegerExtraData* Clone() override { return new NiIntegerExtraData(*this); }

	uint32_t GetIntegerData();
	void SetIntegerData(const uint32_t intData);
};

class NiIntegersExtraData : public NiExtraData {
private:
	uint32_t numIntegers = 0;
	std::vector<uint32_t> integersData;

public:
	static constexpr const char* BlockName = "NiIntegersExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiIntegersExtraData* Clone() override { return new NiIntegersExtraData(*this); }

	std::vector<uint32_t> GetIntegersData();
	void SetIntegersData(const std::vector<uint32_t>& intData);
};

class NiVectorExtraData : public NiExtraData {
private:
	Vector4 vectorData;

public:
	static constexpr const char* BlockName = "NiVectorExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiVectorExtraData* Clone() override { return new NiVectorExtraData(*this); }

	Vector4 GetVectorData();
	void SetVectorData(const Vector4& vecData);
};

class NiColorExtraData : public NiExtraData {
private:
	Color4 colorData;

public:
	static constexpr const char* BlockName = "NiColorExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiColorExtraData* Clone() override { return new NiColorExtraData(*this); }

	Color4 GetColorData();
	void SetColorData(const Color4& colData);
};

class BSXFlags : public NiIntegerExtraData {
public:
	static constexpr const char* BlockName = "BSXFlags";
	const char* GetBlockName() override { return BlockName; }

	BSXFlags* Clone() override { return new BSXFlags(*this); }
};

class BSWArray : public NiExtraData {
private:
	uint32_t numData = 0;
	std::vector<uint32_t> data;

public:
	static constexpr const char* BlockName = "BSWArray";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSWArray* Clone() override { return new BSWArray(*this); }

	std::vector<uint32_t> GetData();
	void SetData(const std::vector<uint32_t>& dat);
};

class BSPositionData : public NiExtraData {
private:
	uint32_t numData = 0;
	std::vector<half_float::half> data;

public:
	static constexpr const char* BlockName = "BSPositionData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSPositionData* Clone() override { return new BSPositionData(*this); }

	std::vector<half_float::half> GetData();
	void SetData(const std::vector<half_float::half>& dat);
};

class BSEyeCenterExtraData : public NiExtraData {
private:
	uint32_t numData = 0;
	std::vector<float> data;

public:
	static constexpr const char* BlockName = "BSEyeCenterExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSEyeCenterExtraData* Clone() override { return new BSEyeCenterExtraData(*this); }

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

class BSPackedCombinedSharedGeomDataExtra : public NiExtraData {
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
	BSPackedCombinedSharedGeomDataExtra* Clone() override {
		return new BSPackedCombinedSharedGeomDataExtra(*this);
	}
};

class BSInvMarker : public NiExtraData {
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
	BSInvMarker* Clone() override { return new BSInvMarker(*this); }

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

	uint16_t orientation; // User Version <= 11
	uint8_t posRef1;		// User Version <= 11
	uint8_t posRef2;		// User Version <= 11

	float heading;		  // User Version >= 12
	uint16_t animationType; // User Version >= 12
	uint16_t entryPoints;	  // User Version >= 12
};

class BSFurnitureMarker : public NiExtraData {
private:
	uint32_t numPositions = 0;
	std::vector<FurniturePosition> positions;

public:
	static constexpr const char* BlockName = "BSFurnitureMarker";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSFurnitureMarker* Clone() override { return new BSFurnitureMarker(*this); }

	std::vector<FurniturePosition> GetPositions();
	void SetPositions(const std::vector<FurniturePosition>& pos);
};

class BSFurnitureMarkerNode : public BSFurnitureMarker {
public:
	static constexpr const char* BlockName = "BSFurnitureMarkerNode";
	const char* GetBlockName() override { return BlockName; }

	BSFurnitureMarkerNode* Clone() override { return new BSFurnitureMarkerNode(*this); }
};

struct DecalVectorBlock {
	uint16_t numVectors;
	std::vector<Vector3> points;
	std::vector<Vector3> normals;
};

class BSDecalPlacementVectorExtraData : public NiFloatExtraData {
private:
	uint16_t numVectorBlocks = 0;
	std::vector<DecalVectorBlock> decalVectorBlocks;

public:
	static constexpr const char* BlockName = "BSDecalPlacementVectorExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSDecalPlacementVectorExtraData* Clone() override { return new BSDecalPlacementVectorExtraData(*this); }

	std::vector<DecalVectorBlock> GetDecalVectorBlocks();
	void SetDecalVectorBlocks(const std::vector<DecalVectorBlock>& vectorBlocks);
};

class BSBehaviorGraphExtraData : public NiExtraData {
private:
	StringRef behaviorGraphFile;
	bool controlsBaseSkel = false;

public:
	static constexpr const char* BlockName = "BSBehaviorGraphExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::set<StringRef*>& refs) override;
	BSBehaviorGraphExtraData* Clone() override { return new BSBehaviorGraphExtraData(*this); }
};

class BSBound : public NiExtraData {
private:
	Vector3 center;
	Vector3 halfExtents;

public:
	static constexpr const char* BlockName = "BSBound";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSBound* Clone() override { return new BSBound(*this); }

	Vector3 GetCenter() { return center; }
	void SetCenter(const Vector3& ctr) { center = ctr; }

	Vector3 GetHalfExtents() { return halfExtents; }
	void SetHalfExtents(const Vector3& hExtents) { halfExtents = hExtents; }
};

struct BoneLOD {
	uint32_t distance;
	StringRef boneName;
};

class BSBoneLODExtraData : public NiExtraData {
private:
	uint32_t numBoneLODs = 0;
	std::vector<BoneLOD> boneLODs;

public:
	static constexpr const char* BlockName = "BSBoneLODExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::set<StringRef*>& refs) override;
	BSBoneLODExtraData* Clone() override { return new BSBoneLODExtraData(*this); }

	std::vector<BoneLOD> GetBoneLODs();
	void SetBoneLODs(const std::vector<BoneLOD>& lods);
};

class NiTextKeyExtraData : public NiExtraData {
private:
	uint32_t numTextKeys = 0;
	std::vector<Key<StringRef>> textKeys;

public:
	static constexpr const char* BlockName = "NiTextKeyExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::set<StringRef*>& refs) override;
	NiTextKeyExtraData* Clone() override { return new NiTextKeyExtraData(*this); }

	std::vector<Key<StringRef>> GetTextKeys();
	void SetTextKeys(const std::vector<Key<StringRef>>& keys);
};

class BSDistantObjectLargeRefExtraData : public NiExtraData {
private:
	bool largeRef = true;

public:
	static constexpr const char* BlockName = "BSDistantObjectLargeRefExtraData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSDistantObjectLargeRefExtraData* Clone() override { return new BSDistantObjectLargeRefExtraData(*this); }

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

class BSConnectPointParents : public NiExtraData {
private:
	uint32_t numConnectPoints = 0;
	std::vector<BSConnectPoint> connectPoints;

public:
	static constexpr const char* BlockName = "BSConnectPoint::Parents";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSConnectPointParents* Clone() override { return new BSConnectPointParents(*this); }

	std::vector<BSConnectPoint> GetConnectPoints();
	void SetConnectPoints(const std::vector<BSConnectPoint>& cps);
};

class BSConnectPointChildren : public NiExtraData {
private:
	uint8_t unkByte = 1;
	uint32_t numTargets = 0;
	std::vector<NiString> targets;

public:
	static constexpr const char* BlockName = "BSConnectPoint::Children";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSConnectPointChildren* Clone() override { return new BSConnectPointChildren(*this); }

	std::vector<NiString> GetTargets();
	void SetTargets(const std::vector<NiString>& targ);
};

class BSExtraData : public NiObject {};

class BSClothExtraData : public BSExtraData {
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
	BSClothExtraData* Clone() override { return new BSClothExtraData(*this); }

	std::vector<char> GetData();
	void SetData(const std::vector<char>& dat);

	bool ToHKX(const std::string& fileName);
	bool FromHKX(const std::string& fileName);
};
} // namespace nifly
