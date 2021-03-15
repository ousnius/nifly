/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#pragma once

#include "BasicTypes.hpp"
#include "Objects.hpp"
#include "VertexData.hpp"

#include <deque>

namespace nifly {
struct AdditionalDataInfo {
	int dataType = 0;
	uint32_t numChannelBytesPerElement = 0;
	uint32_t numChannelBytes = 0;
	uint32_t numTotalBytesPerElement = 0;
	uint32_t blockIndex = 0;
	uint32_t channelOffset = 0;
	uint8_t unkByte1 = 2;

	void Sync(NiStreamReversible& stream) {
		stream.Sync(dataType);
		stream.Sync(numChannelBytesPerElement);
		stream.Sync(numChannelBytes);
		stream.Sync(numTotalBytesPerElement);
		stream.Sync(blockIndex);
		stream.Sync(channelOffset);
		stream.Sync(unkByte1);
	}
};

struct AdditionalDataBlock {
	bool hasData = false;
	uint32_t blockSize = 0;

	uint32_t numBlocks = 0;
	std::vector<uint32_t> blockOffsets;

	uint32_t numData = 0;
	std::vector<uint32_t> dataSizes;
	std::vector<std::vector<uint8_t>> data;

	void Sync(NiStreamReversible& stream) {
		stream.Sync(hasData);

		if (hasData) {
			stream.Sync(blockSize);

			stream.Sync(numBlocks);
			blockOffsets.resize(numBlocks);
			for (uint32_t i = 0; i < numBlocks; i++)
				stream.Sync(blockOffsets[i]);

			stream.Sync(numData);
			dataSizes.resize(numData);
			for (uint32_t i = 0; i < numData; i++)
				stream.Sync(dataSizes[i]);

			data.resize(numData);
			for (uint32_t i = 0; i < numData; i++) {
				data[i].resize(blockSize);
				for (uint32_t j = 0; j < blockSize; j++)
					stream.Sync(data[i][j]);
			}
		}
	}
};

class AdditionalGeomData : public NiObjectCRTP<AdditionalGeomData, NiObject> {};

class NiAdditionalGeometryData : public NiObjectCRTP<NiAdditionalGeometryData, AdditionalGeomData, true> {
private:
	uint16_t numVertices = 0;

	uint32_t numBlockInfos = 0;
	std::vector<AdditionalDataInfo> blockInfos;

	uint32_t numBlocks = 0;
	std::vector<AdditionalDataBlock> blocks;

public:
	static constexpr const char* BlockName = "NiAdditionalGeometryData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

struct BSPackedAdditionalDataBlock {
	bool hasData = false;
	uint32_t numTotalBytes = 0;

	uint32_t numBlocks = 0;
	std::vector<uint32_t> blockOffsets;

	uint32_t numAtoms = 0;
	std::vector<uint32_t> atomSizes;
	std::vector<uint8_t> data;

	uint32_t unkInt1 = 0;
	uint32_t numTotalBytesPerElement = 0;

	void Sync(NiStreamReversible& stream) {
		stream.Sync(hasData);

		if (hasData) {
			stream.Sync(numTotalBytes);

			stream.Sync(numBlocks);
			blockOffsets.resize(numBlocks);
			for (uint32_t i = 0; i < numBlocks; i++)
				stream.Sync(blockOffsets[i]);

			stream.Sync(numAtoms);
			atomSizes.resize(numAtoms);
			for (uint32_t i = 0; i < numAtoms; i++)
				stream.Sync(atomSizes[i]);

			data.resize(numTotalBytes);
			for (uint32_t i = 0; i < numTotalBytes; i++)
				stream.Sync(data[i]);
		}

		stream.Sync(unkInt1);
		stream.Sync(numTotalBytesPerElement);
	}
};

class BSPackedAdditionalGeometryData
	: public NiObjectCRTP<BSPackedAdditionalGeometryData, AdditionalGeomData, true> {
private:
	uint16_t numVertices = 0;

	uint32_t numBlockInfos = 0;
	std::vector<AdditionalDataInfo> blockInfos;

	uint32_t numBlocks = 0;
	std::vector<BSPackedAdditionalDataBlock> blocks;

public:
	static constexpr const char* BlockName = "BSPackedAdditionalGeometryData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiGeometryData : public NiObjectCRTP<NiGeometryData, NiObject, true> {
protected:
	bool isPSys = false;

	uint16_t numVertices = 0;
	int groupID = 0;
	uint8_t compressFlags = 0;
	bool hasVertices = true;
	uint32_t materialCRC = 0;
	bool hasNormals = false;
	bool hasVertexColors = false;
	BlockRef<AdditionalGeomData> additionalDataRef;
	BoundingSphere bounds;

public:
	std::vector<Vector3> vertices;
	std::vector<Vector3> normals;
	std::vector<Vector3> tangents;
	std::vector<Vector3> bitangents;
	std::vector<Color4> vertexColors;

	uint8_t keepFlags = 0;
	uint16_t numUVSets = 0;
	std::vector<std::vector<Vector2>> uvSets;

	uint16_t consistencyFlags = 0;

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	void notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) override;

	int GetAdditionalDataRef();
	void SetAdditionalDataRef(int dataRef);

	uint16_t GetNumVertices();
	void SetVertices(const bool enable);
	bool HasVertices() { return hasVertices; }

	void SetNormals(const bool enable);
	bool HasNormals() { return hasNormals; }

	void SetVertexColors(const bool enable);
	bool HasVertexColors() { return hasVertexColors; }

	void SetUVs(const bool enable);
	bool HasUVs() { return (numUVSets & (1 << 0)) != 0; }

	void SetTangents(const bool enable);
	bool HasTangents() { return (numUVSets & (1 << 12)) != 0; }

	virtual uint32_t GetNumTriangles();
	virtual bool GetTriangles(std::vector<Triangle>& tris);
	virtual void SetTriangles(const std::vector<Triangle>& tris);

	void SetBounds(const BoundingSphere& newBounds) { this->bounds = newBounds; }
	BoundingSphere GetBounds() { return bounds; }
	void UpdateBounds();

	virtual void Create(NiVersion& version,
						const std::vector<Vector3>* verts,
						const std::vector<Triangle>* tris,
						const std::vector<Vector2>* uvs,
						const std::vector<Vector3>* norms);
	virtual void RecalcNormals(const bool smooth = true, const float smoothThres = 60.0f);
	virtual void CalcTangentSpace();
};

class NiShape : public NiObjectCRTP<NiShape, NiAVObject> {
public:
	virtual NiGeometryData* GetGeomData();
	virtual void SetGeomData(NiGeometryData* geomDataPtr);

	virtual int GetDataRef();
	virtual void SetDataRef(int dataRef);

	virtual int GetSkinInstanceRef();
	virtual void SetSkinInstanceRef(int skinInstanceRef);

	virtual int GetShaderPropertyRef();
	virtual void SetShaderPropertyRef(int shaderPropertyRef);

	virtual int GetAlphaPropertyRef();
	virtual void SetAlphaPropertyRef(int alphaPropertyRef);

	virtual uint16_t GetNumVertices();
	virtual void SetVertices(const bool enable);
	virtual bool HasVertices();

	virtual void SetUVs(const bool enable);
	virtual bool HasUVs();

	virtual void SetNormals(const bool enable);
	virtual bool HasNormals();

	virtual void SetTangents(const bool enable);
	virtual bool HasTangents();

	virtual void SetVertexColors(const bool enable);
	virtual bool HasVertexColors();

	virtual void SetSkinned(const bool enable);
	virtual bool IsSkinned();

	virtual uint32_t GetNumTriangles();
	virtual bool GetTriangles(std::vector<Triangle>& tris);
	virtual void SetTriangles(const std::vector<Triangle>& tris);
	virtual bool ReorderTriangles(const std::vector<uint32_t>& triInds);

	virtual void SetBounds(const BoundingSphere& bounds);
	virtual BoundingSphere GetBounds();
	virtual void UpdateBounds();

	int GetBoneID(NiHeader& hdr, const std::string& boneName);
};


class BSTriShape : public NiObjectCRTP<BSTriShape, NiShape, true> {
protected:
	BlockRef<NiObject> skinInstanceRef;
	BlockRef<NiProperty> shaderPropertyRef;
	BlockRef<NiProperty> alphaPropertyRef;

	BoundingSphere bounds;
	float boundMinMax[6]{};

	uint32_t numTriangles = 0;
	std::vector<Triangle> triangles;

	uint16_t numVertices = 0;

public:
	VertexDesc vertexDesc;

	uint32_t dataSize = 0;
	uint32_t vertexSize = 0; // Not in file

	uint32_t particleDataSize = 0;
	std::vector<Vector3> particleVerts;
	std::vector<Vector3> particleNorms;
	std::vector<Triangle> particleTris;

	std::vector<Vector3> rawVertices;	// filled by GetRawVerts function and returned.
	std::vector<Vector3> rawNormals;	// filled by GetNormalData function and returned.
	std::vector<Vector3> rawTangents;	// filled by CalcTangentSpace function and returned.
	std::vector<Vector3> rawBitangents; // filled in CalcTangentSpace
	std::vector<Vector2> rawUvs;		// filled by GetUVData function and returned.
	std::vector<Color4> rawColors;		// filled by GetColorData function and returned.
	std::vector<float> rawEyeData;

	std::vector<uint32_t> deletedTris; // temporary storage for BSSubIndexTriShape

	std::vector<BSVertexData> vertData;

	BSTriShape();

	static constexpr const char* BlockName = "BSTriShape";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	int GetSkinInstanceRef() override;
	void SetSkinInstanceRef(int skinInstRef) override;

	int GetShaderPropertyRef() override;
	void SetShaderPropertyRef(int shaderPropRef) override;

	int GetAlphaPropertyRef() override;
	void SetAlphaPropertyRef(int alphaPropRef) override;

	std::vector<Vector3>* GetRawVerts();
	std::vector<Vector3>* GetNormalData(bool xform = true);
	std::vector<Vector3>* GetTangentData(bool xform = true);
	std::vector<Vector3>* GetBitangentData(bool xform = true);
	std::vector<Vector2>* GetUVData();
	std::vector<Color4>* GetColorData();
	std::vector<float>* GetEyeData();

	uint16_t GetNumVertices() override;
	void SetVertices(const bool enable) override;
	bool HasVertices() override { return vertexDesc.HasFlag(VF_VERTEX); }

	void SetUVs(const bool enable) override;
	bool HasUVs() override { return vertexDesc.HasFlag(VF_UV); }

	void SetSecondUVs(const bool enable);
	bool HasSecondUVs() { return vertexDesc.HasFlag(VF_UV_2); }

	void SetNormals(const bool enable) override;
	bool HasNormals() override { return vertexDesc.HasFlag(VF_NORMAL); }

	void SetTangents(const bool enable) override;
	bool HasTangents() override { return vertexDesc.HasFlag(VF_TANGENT); }

	void SetVertexColors(const bool enable) override;
	bool HasVertexColors() override { return vertexDesc.HasFlag(VF_COLORS); }

	void SetSkinned(const bool enable) override;
	bool IsSkinned() override { return vertexDesc.HasFlag(VF_SKINNED); }

	void SetEyeData(const bool enable);
	bool HasEyeData() { return vertexDesc.HasFlag(VF_EYEDATA); }

	void SetFullPrecision(const bool enable);
	bool IsFullPrecision() { return vertexDesc.HasFlag(VF_FULLPREC); }
	bool CanChangePrecision() { return (HasVertices()); }

	uint32_t GetNumTriangles() override;
	bool GetTriangles(std::vector<Triangle>&) override;
	void SetTriangles(const std::vector<Triangle>&) override;

	void SetBounds(const BoundingSphere& newBounds) override { bounds = newBounds; }
	BoundingSphere GetBounds() override { return bounds; }
	void UpdateBounds() override;

	void SetVertexData(const std::vector<BSVertexData>& bsVertData);

	void SetNormals(const std::vector<Vector3>& inNorms);
	void RecalcNormals(const bool smooth = true,
					   const float smoothThres = 60.0f,
					   std::unordered_set<uint32_t>* lockedIndices = nullptr);
	void CalcTangentSpace();
	int CalcDataSizes(NiVersion& version);

	void SetTangentData(const std::vector<Vector3>& in);
	void SetBitangentData(const std::vector<Vector3>& in);
	void SetEyeData(const std::vector<float>& in);

	virtual void Create(NiVersion& version,
						const std::vector<Vector3>* verts,
						const std::vector<Triangle>* tris,
						const std::vector<Vector2>* uvs,
						const std::vector<Vector3>* normals = nullptr);
};


// NifSubSegmentInfo: not in file.  The portion of a subsegment's data
// that has nothing to do with triangle set partitioning.
struct NifSubSegmentInfo {
	// partID: a small nonnegative integer uniquely identifying this
	// subsegment among all the segments and subsegments.  Used as a value
	// in triParts.  Not in the file.
	int partID = 0;
	uint32_t userSlotID = 0;
	uint32_t material = 0;
	std::vector<float> extraData;
};

// NifSegmentInfo: not in file.  The portion of a segment's data that
// has nothing to do with triangle set partitioning.
struct NifSegmentInfo {
	// partID: a small nonnegative integer uniquely identifying this
	// segment among all the segments and subsegments.  Used as a value
	// in triParts.  Not in the file.
	int partID = 0;
	std::vector<NifSubSegmentInfo> subs;
};

// NifSegmentationInfo: not in file.  The portion of a shape's
// segmentation data that has nothing to do with triangle set partitioning.
// The intention is that this data structure can be used for any type of
// segmentation data, both BSSITSSegmentation and BSGeometrySegmentData.
struct NifSegmentationInfo {
	std::vector<NifSegmentInfo> segs;
	std::string ssfFile;
};


class BSGeometrySegmentData {
public:
	uint8_t flags = 0;
	uint32_t index = 0;
	uint32_t numTris = 0;

	void Sync(NiStreamReversible& stream);
};

class BSSubIndexTriShape : public NiObjectCRTP<BSSubIndexTriShape, BSTriShape, true> {
public:
	class BSSITSSubSegment {
	public:
		uint32_t startIndex = 0;
		uint32_t numPrimitives = 0;
		uint32_t arrayIndex = 0;
		uint32_t unkInt1 = 0;
	};

	class BSSITSSegment {
	public:
		uint32_t startIndex = 0;
		uint32_t numPrimitives = 0;
		uint32_t parentArrayIndex = 0xFFFFFFFF;
		uint32_t numSubSegments = 0;
		std::vector<BSSITSSubSegment> subSegments;
	};

	class BSSITSSubSegmentDataRecord {
	public:
		uint32_t userSlotID = 0;
		uint32_t material = 0xFFFFFFFF;
		uint32_t numData = 0;
		std::vector<float> extraData;
	};

	class BSSITSSubSegmentData {
	public:
		uint32_t numSegments = 0;
		uint32_t numTotalSegments = 0;
		std::vector<uint32_t> arrayIndices;
		std::vector<BSSITSSubSegmentDataRecord> dataRecords;
		NiString ssfFile;
	};

	class BSSITSSegmentation {
	public:
		uint32_t numPrimitives = 0;
		uint32_t numSegments = 0;
		uint32_t numTotalSegments = 0;
		std::vector<BSSITSSegment> segments;
		BSSITSSubSegmentData subSegmentData;
	};

	// SSE
	uint32_t numSegments = 0;
	std::vector<BSGeometrySegmentData> segments;

private:
	// FO4
	BSSITSSegmentation segmentation;

public:
	static constexpr const char* BlockName = "BSSubIndexTriShape";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) override;

	void GetSegmentation(NifSegmentationInfo& inf, std::vector<int>& triParts);
	void SetSegmentation(const NifSegmentationInfo& inf, const std::vector<int>& triParts);

	void SetDefaultSegments();
	void Create(NiVersion& version,
				const std::vector<Vector3>* verts,
				const std::vector<Triangle>* tris,
				const std::vector<Vector2>* uvs,
				const std::vector<Vector3>* normals = nullptr) override;
};

class BSMeshLODTriShape : public NiObjectCRTP<BSMeshLODTriShape, BSTriShape, true> {
public:
	uint32_t lodSize0 = 0;
	uint32_t lodSize1 = 0;
	uint32_t lodSize2 = 0;

	static constexpr const char* BlockName = "BSMeshLODTriShape";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) override;
};

class BSDynamicTriShape : public NiObjectCRTP<BSDynamicTriShape, BSTriShape, true> {
public:
	uint32_t dynamicDataSize;
	std::vector<Vector4> dynamicData;

	BSDynamicTriShape();

	static constexpr const char* BlockName = "BSDynamicTriShape";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) override;
	void CalcDynamicData();

	void Create(NiVersion& version,
				const std::vector<Vector3>* verts,
				const std::vector<Triangle>* tris,
				const std::vector<Vector2>* uvs,
				const std::vector<Vector3>* normals = nullptr) override;
};

class NiSkinInstance;

class NiGeometry : public NiObjectCRTP<NiGeometry, NiShape, true> {
protected:
	BlockRef<NiGeometryData> dataRef;
	BlockRef<NiSkinInstance> skinInstanceRef;
	BlockRef<NiProperty> shaderPropertyRef;
	BlockRef<NiProperty> alphaPropertyRef;

	uint32_t numMaterials = 0;
	std::vector<StringRef> materialNameRefs;
	std::vector<uint32_t> materials;
	int activeMaterial = 0;
	uint8_t defaultMatNeedsUpdateFlag = 0;

	bool shader = false;
	StringRef shaderName;
	uint32_t implementation = 0;

public:
	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<StringRef*>& refs) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	bool IsSkinned();

	int GetDataRef();
	void SetDataRef(int datRef);

	int GetSkinInstanceRef();
	void SetSkinInstanceRef(int skinInstRef);

	int GetShaderPropertyRef();
	void SetShaderPropertyRef(int shaderPropRef);

	int GetAlphaPropertyRef();
	void SetAlphaPropertyRef(int alphaPropRef);
};

class NiTriBasedGeom : public NiObjectCRTP<NiTriBasedGeom, NiGeometry> {};

class NiTriBasedGeomData : public NiObjectCRTP<NiTriBasedGeomData, NiGeometryData, true> {
protected:
	uint16_t numTriangles = 0;

public:
	void Sync(NiStreamReversible& stream);

	void Create(NiVersion& version,
				const std::vector<Vector3>* verts,
				const std::vector<Triangle>* tris,
				const std::vector<Vector2>* uvs,
				const std::vector<Vector3>* norms) override;
};

struct MatchGroup {
	uint16_t count = 0;
	std::vector<uint16_t> matches;
};

class NiTriShapeData : public NiObjectCRTP<NiTriShapeData, NiTriBasedGeomData, true> {
protected:
	uint32_t numTrianglePoints = 0;
	bool hasTriangles = false;
	std::vector<Triangle> triangles;

	uint16_t numMatchGroups = 0;
	std::vector<MatchGroup> matchGroups;

public:
	static constexpr const char* BlockName = "NiTriShapeData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void Create(NiVersion& version,
				const std::vector<Vector3>* verts,
				const std::vector<Triangle>* tris,
				const std::vector<Vector2>* uvs,
				const std::vector<Vector3>* norms) override;
	void notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) override;

	uint32_t GetNumTriangles() override;
	bool GetTriangles(std::vector<Triangle>& tris) override;
	void SetTriangles(const std::vector<Triangle>& tris) override;

	void RecalcNormals(const bool smooth = true, const float smoothThres = 60.0f) override;
	void CalcTangentSpace() override;
};

class NiTriShape : public NiObjectCRTP<NiTriShape, NiTriBasedGeom> {
protected:
	NiTriShapeData* shapeData = nullptr;

public:
	static constexpr const char* BlockName = "NiTriShape";
	const char* GetBlockName() override { return BlockName; }

	NiGeometryData* GetGeomData();
	void SetGeomData(NiGeometryData* geomDataPtr);
};

class NiTriStripsData : public NiObjectCRTP<NiTriStripsData, NiTriBasedGeomData, true> {
protected:
	uint16_t numStrips = 0;
	std::vector<uint16_t> stripLengths;
	bool hasPoints = false;
	std::vector<std::vector<uint16_t>> points;

public:
	static constexpr const char* BlockName = "NiTriStripsData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) override;

	uint32_t GetNumTriangles() override;
	bool GetTriangles(std::vector<Triangle>& tris) override;
	void SetTriangles(const std::vector<Triangle>& tris) override;
	std::vector<Triangle> StripsToTris();

	void RecalcNormals(const bool smooth = true, const float smoothThres = 60.0f) override;
	void CalcTangentSpace() override;
};

class NiTriStrips : public NiObjectCRTP<NiTriStrips, NiTriBasedGeom> {
protected:
	NiTriStripsData* stripsData = nullptr;

public:
	static constexpr const char* BlockName = "NiTriStrips";
	const char* GetBlockName() override { return BlockName; }

	NiGeometryData* GetGeomData();
	void SetGeomData(NiGeometryData* geomDataPtr);

	bool ReorderTriangles(const std::vector<uint32_t>&) override { return false; }
};

class NiLinesData : public NiObjectCRTP<NiLinesData, NiGeometryData, true> {
protected:
	std::deque<bool> lineFlags;

public:
	static constexpr const char* BlockName = "NiLinesData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) override;
};

class NiLines : public NiObjectCRTP<NiLines, NiTriBasedGeom> {
protected:
	NiLinesData* linesData = nullptr;

public:
	static constexpr const char* BlockName = "NiLines";
	const char* GetBlockName() override { return BlockName; }

	NiGeometryData* GetGeomData();
	void SetGeomData(NiGeometryData* geomDataPtr);
};

struct PolygonInfo {
	uint16_t numVertices = 0;
	uint16_t vertexOffset = 0;
	uint16_t numTriangles = 0;
	uint16_t triangleOffset = 0;
};

class NiScreenElementsData : public NiObjectCRTP<NiScreenElementsData, NiTriShapeData, true> {
protected:
	uint16_t maxPolygons = 0;
	std::vector<PolygonInfo> polygons;
	std::vector<uint16_t> polygonIndices;

	uint16_t unkShort1 = 1;
	uint16_t numPolygons = 0;
	uint16_t usedVertices = 0;
	uint16_t unkShort2 = 1;
	uint16_t usedTrianglePoints = 0;
	uint16_t unkShort3 = 1;

public:
	static constexpr const char* BlockName = "NiScreenElementsData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) override;
};

class NiScreenElements : public NiObjectCRTP<NiScreenElements, NiTriShape> {
protected:
	NiScreenElementsData* elemData = nullptr;

public:
	static constexpr const char* BlockName = "NiScreenElements";
	const char* GetBlockName() override { return BlockName; }

	NiGeometryData* GetGeomData();
	void SetGeomData(NiGeometryData* geomDataPtr);
};

class BSLODTriShape : public NiObjectCRTP<BSLODTriShape, NiTriBasedGeom, true> {
protected:
	NiTriShapeData* shapeData = nullptr;

	uint32_t level0 = 0;
	uint32_t level1 = 0;
	uint32_t level2 = 0;

public:
	static constexpr const char* BlockName = "BSLODTriShape";
	const char* GetBlockName() override { return BlockName; }

	NiGeometryData* GetGeomData();
	void SetGeomData(NiGeometryData* geomDataPtr);

	void Sync(NiStreamReversible& stream);
};

class BSSegmentedTriShape : public NiObjectCRTP<BSSegmentedTriShape, NiTriShape, true> {
public:
	uint32_t numSegments = 0;
	std::vector<BSGeometrySegmentData> segments;

	static constexpr const char* BlockName = "BSSegmentedTriShape";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};
} // namespace nifly
