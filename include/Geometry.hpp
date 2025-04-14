/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#pragma once

#include "BasicTypes.hpp"
#include "Objects.hpp"
#include "Shaders.hpp"
#include "Skin.hpp"
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

class AdditionalGeomData : public NiCloneable<AdditionalGeomData, NiObject> {};

class NiAdditionalGeometryData : public NiCloneableStreamable<NiAdditionalGeometryData, AdditionalGeomData> {
public:
	uint16_t numVertices = 0;
	NiSyncVector<AdditionalDataInfo> blockInfos;
	NiSyncVector<AdditionalDataBlock> blocks;

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
	: public NiCloneableStreamable<BSPackedAdditionalGeometryData, AdditionalGeomData> {
public:
	uint16_t numVertices = 0;
	NiSyncVector<AdditionalDataInfo> blockInfos;
	NiSyncVector<BSPackedAdditionalDataBlock> blocks;

	static constexpr const char* BlockName = "BSPackedAdditionalGeometryData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

enum ConsistencyType : uint16_t { CT_MUTABLE = 0x0000, CT_STATIC = 0x4000, CT_VOLATILE = 0x8000 };

class NiGeometryData : public NiCloneableStreamable<NiGeometryData, NiObject> {
protected:
	bool isPSys = false;

	uint16_t numVertices = 0;
	bool hasVertices = true;
	bool hasNormals = false;
	bool hasVertexColors = false;
	BoundingSphere bounds;

public:
	std::vector<Vector3> vertices;
	std::vector<Vector3> normals;
	std::vector<Vector3> tangents;
	std::vector<Vector3> bitangents;
	std::vector<Color4> vertexColors;

	int groupID = 0;
	uint8_t compressFlags = 0;
	uint32_t materialCRC = 0;

	uint8_t keepFlags = 0;
	uint16_t dataFlags = 0;
	std::vector<std::vector<Vector2>> uvSets;

	ConsistencyType consistencyFlags = CT_MUTABLE;
	NiBlockRef<AdditionalGeomData> additionalDataRef;

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;

	void notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) override;

	uint16_t GetNumVertices() const;
	void SetVertices(const bool enable);
	bool HasVertices() const { return hasVertices; }

	void SetNormals(const bool enable);
	bool HasNormals() const { return hasNormals; }

	void SetVertexColors(const bool enable);
	bool HasVertexColors() const { return hasVertexColors; }

	void SetUVs(const bool enable);
	bool HasUVs() const { return (dataFlags & (1 << 0)) != 0; }

	void SetTangents(const bool enable);
	bool HasTangents() const { return (dataFlags & (1 << 12)) != 0; }

	virtual uint32_t GetNumTriangles() const;
	virtual bool GetTriangles(std::vector<Triangle>& tris) const;
	virtual void SetTriangles(const std::vector<Triangle>& tris);

	void SetBounds(const BoundingSphere& newBounds) { this->bounds = newBounds; }
	BoundingSphere GetBounds() const { return bounds; }
	void UpdateBounds();

	virtual void Create(NiVersion& version,
						const std::vector<Vector3>* verts,
						const std::vector<Triangle>* tris,
						const std::vector<Vector2>* uvs,
						const std::vector<Vector3>* norms);
	virtual void RecalcNormals(const bool smooth = true,
							   const float smoothThres = 60.0f,
							   std::unordered_set<uint32_t>* lockedIndices = nullptr);
	virtual void CalcTangentSpace();
};

class NiShape : public NiCloneable<NiShape, NiAVObject> {
public:
	virtual NiGeometryData* GetGeomData() const { return nullptr; }
	virtual void SetGeomData(NiGeometryData*) {}

	virtual bool HasData() const { return false; }
	virtual NiBlockRef<NiGeometryData>* DataRef() { return nullptr; }
	virtual const NiBlockRef<NiGeometryData>* DataRef() const { return nullptr; }

	virtual bool HasSkinInstance() const { return false; }
	virtual NiBlockRef<NiBoneContainer>* SkinInstanceRef() { return nullptr; }
	virtual const NiBlockRef<NiBoneContainer>* SkinInstanceRef() const { return nullptr; }

	virtual bool HasShaderProperty() const { return false; }
	virtual NiBlockRef<NiShader>* ShaderPropertyRef() { return nullptr; }
	virtual const NiBlockRef<NiShader>* ShaderPropertyRef() const { return nullptr; }

	virtual bool HasAlphaProperty() const { return false; }
	virtual NiBlockRef<NiAlphaProperty>* AlphaPropertyRef() { return nullptr; }
	virtual const NiBlockRef<NiAlphaProperty>* AlphaPropertyRef() const { return nullptr; }

	virtual uint16_t GetNumVertices() const;
	virtual void SetVertices(const bool enable);
	virtual bool HasVertices() const;

	virtual void SetUVs(const bool enable);
	virtual bool HasUVs() const;

	virtual void SetNormals(const bool enable);
	virtual bool HasNormals() const;

	virtual void SetTangents(const bool enable);
	virtual bool HasTangents() const;

	virtual void SetVertexColors(const bool enable);
	virtual bool HasVertexColors() const;

	virtual void SetSkinned(const bool enable);
	virtual bool IsSkinned() const;

	virtual uint32_t GetNumTriangles() const;
	virtual bool GetTriangles(std::vector<Triangle>& tris) const;
	virtual void SetTriangles(const std::vector<Triangle>& tris);
	virtual bool ReorderTriangles(const std::vector<uint32_t>& triInds);

	virtual void SetBounds(const BoundingSphere& bounds);
	virtual BoundingSphere GetBounds() const;
	virtual void UpdateBounds();

	int GetBoneID(const NiHeader& hdr, const std::string& boneName) const;
};


class BSTriShape : public NiCloneableStreamable<BSTriShape, NiShape> {
protected:
	NiBlockRef<NiBoneContainer> skinInstanceRef;
	NiBlockRef<NiShader> shaderPropertyRef;
	NiBlockRef<NiAlphaProperty> alphaPropertyRef;

	BoundingSphere bounds;
	float boundMinMax[6]{};

	uint32_t numTriangles = 0;
	uint16_t numVertices = 0;

public:
	VertexDesc vertexDesc;

	uint32_t dataSize = 0;
	uint32_t vertexSize = 0; // Not in file

	uint32_t particleDataSize = 0;
	std::vector<Vector3> particleVerts;
	std::vector<Vector3> particleNorms;
	std::vector<Triangle> particleTris;

	std::vector<Vector3> rawVertices;	// temporary copy filled by UpdateRawVertices function
	std::vector<Vector3> rawNormals;	// temporary copy filled by UpdateRawNormals function
	std::vector<Vector3> rawTangents;	// temporary copy filled by UpdateRawTangents function
	std::vector<Vector3> rawBitangents; // temporary copy filled by UpdateRawBitangents function
	std::vector<Vector2> rawUvs;		// temporary copy filled by UpdateRawUvs function
	std::vector<Color4> rawColors;		// temporary copy filled by UpdateRawColors function
	std::vector<float> rawEyeData;		// temporary copy filled by UpdateRawEyeData function

	std::vector<uint32_t> deletedTris; // temporary storage for BSSubIndexTriShape

	std::vector<BSVertexData> vertData;
	std::vector<Triangle> triangles;

	BSTriShape();

	static constexpr const char* BlockName = "BSTriShape";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) override;
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;

	bool HasSkinInstance() const override { return !skinInstanceRef.IsEmpty(); }
	NiBlockRef<NiBoneContainer>* SkinInstanceRef() override { return &skinInstanceRef; }
	const NiBlockRef<NiBoneContainer>* SkinInstanceRef() const override { return &skinInstanceRef; }

	bool HasShaderProperty() const override { return !shaderPropertyRef.IsEmpty(); }
	NiBlockRef<NiShader>* ShaderPropertyRef() override { return &shaderPropertyRef; }
	const NiBlockRef<NiShader>* ShaderPropertyRef() const override { return &shaderPropertyRef; }

	bool HasAlphaProperty() const override { return !alphaPropertyRef.IsEmpty(); }
	NiBlockRef<NiAlphaProperty>* AlphaPropertyRef() override { return &alphaPropertyRef; }
	const NiBlockRef<NiAlphaProperty>* AlphaPropertyRef() const override { return &alphaPropertyRef; }

	std::vector<Vector3>& UpdateRawVertices();
	std::vector<Vector3>& UpdateRawNormals();
	std::vector<Vector3>& UpdateRawTangents();
	std::vector<Vector3>& UpdateRawBitangents();
	std::vector<Vector2>& UpdateRawUvs();
	std::vector<Color4>& UpdateRawColors();
	std::vector<float>& UpdateRawEyeData();

	uint16_t GetNumVertices() const override;
	void SetVertices(const bool enable) override;
	bool HasVertices() const override { return vertexDesc.HasFlag(VF_VERTEX); }

	void SetUVs(const bool enable) override;
	bool HasUVs() const override { return vertexDesc.HasFlag(VF_UV); }

	void SetSecondUVs(const bool enable);
	bool HasSecondUVs() const { return vertexDesc.HasFlag(VF_UV_2); }

	void SetNormals(const bool enable) override;
	bool HasNormals() const override { return vertexDesc.HasFlag(VF_NORMAL); }

	void SetTangents(const bool enable) override;
	bool HasTangents() const override { return vertexDesc.HasFlag(VF_TANGENT); }

	void SetVertexColors(const bool enable) override;
	bool HasVertexColors() const override { return vertexDesc.HasFlag(VF_COLORS); }

	void SetSkinned(const bool enable) override;
	bool IsSkinned() const override { return vertexDesc.HasFlag(VF_SKINNED); }

	void SetEyeData(const bool enable);
	bool HasEyeData() const { return vertexDesc.HasFlag(VF_EYEDATA); }

	void SetFullPrecision(const bool enable);
	bool IsFullPrecision() const { return vertexDesc.HasFlag(VF_FULLPREC); }
	bool CanChangePrecision() const { return (HasVertices()); }

	uint32_t GetNumTriangles() const override;
	bool GetTriangles(std::vector<Triangle>&) const override;
	void SetTriangles(const std::vector<Triangle>&) override;

	void SetBounds(const BoundingSphere& newBounds) override { bounds = newBounds; }
	BoundingSphere GetBounds() const override { return bounds; }
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

class BSSubIndexTriShape : public NiCloneableStreamable<BSSubIndexTriShape, BSTriShape> {
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

protected:
	// SSE
	uint32_t numSegments = 0;
	std::vector<BSGeometrySegmentData> segments;

	// FO4
	BSSITSSegmentation segmentation;

public:
	static constexpr const char* BlockName = "BSSubIndexTriShape";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) override;

	std::vector<BSGeometrySegmentData> GetSegments() const;
	void SetSegments(const std::vector<BSGeometrySegmentData>& sd);

	void GetSegmentation(NifSegmentationInfo& inf, std::vector<int>& triParts) const;
	void SetSegmentation(const NifSegmentationInfo& inf, const std::vector<int>& triParts);

	void SetDefaultSegments();
	void Create(NiVersion& version,
				const std::vector<Vector3>* verts,
				const std::vector<Triangle>* tris,
				const std::vector<Vector2>* uvs,
				const std::vector<Vector3>* normals = nullptr) override;
};

class BSMeshLODTriShape : public NiCloneableStreamable<BSMeshLODTriShape, BSTriShape> {
public:
	uint32_t lodSize0 = 0;
	uint32_t lodSize1 = 0;
	uint32_t lodSize2 = 0;

	static constexpr const char* BlockName = "BSMeshLODTriShape";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) override;
};

class BSDynamicTriShape : public NiCloneableStreamable<BSDynamicTriShape, BSTriShape> {
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

// BSGeometryMeshData is not a nif block object.  In order to be able to use the data as if it were a block
// data object for reading and modifying geometry data, we inherit the NiGeometryData interface, and override
// the Sync function.  The stream provided to sync for this object is not the same stream that is working with
// a nif file.  
class BSGeometryMeshData : public NiCloneableStreamable<BSGeometryMeshData, NiGeometryData> {
private:
	// Traditional scale based on havok to unit transform used in skyrim, fallout, etc. In Starfield mesh files are normalized to metric units,
	// this scale makes default vertex positions closely match the older games
	const float havokScale = 69.969f;
	// experimentally, the below scale produced very accurate values to SSE mesh sizes (comparing markerxheading.nif)
	// const float havokScale = 69.9866f;

public:
	struct BoneWeight {
		uint16_t boneIndex = 0;
		uint16_t weight = 0;
	};

	struct Meshlet {
		uint32_t vertCount = 0;
		uint32_t vertOffset = 0;
		uint32_t primCount = 0;
		uint32_t primOffset = 0;
	};

	struct CullData {
		Vector3 center;
		Vector3 expand;
	};

	uint32_t version = 0;

	uint32_t nTriIndices = 0;
	std::vector<Triangle> tris;

	float scale = 0.0f;
	uint32_t nWeightsPerVert = 0;

	// Vert count is a full 32 bits, versus the 16 bit count in NiGeometryData
	uint32_t nVertices = 0;
	// vertices from NIGeometryData

	uint32_t nUV1 = 0;
	uint32_t nUV2 = 0;
	// uvSets from NiGeometryData  -- read/write interspersed with nUV1, nUV2

	uint32_t nColors = 0;
	std::vector<ByteColor4> vColors;
	// vertexColors from NiGeometryData

	uint32_t nNormals = 0;
	// normals from NiGeometryData  (UDEC3 packed in file)

	uint32_t nTangents = 0;
	// tangents from NiGeometryData  (UDEC3 packed in file)

	uint32_t nTotalWeights = 0;
	std::vector<std::vector<BoneWeight>> skinWeights;

	uint32_t nLODS = 0;
	std::vector<std::vector<Triangle>> lods;

	uint32_t nMeshlets = 0;
	std::vector<Meshlet> meshletList;

	uint32_t nCullData = 0;
	std::vector<CullData> cullDataList;

	void Sync(NiStreamReversible& stream);
};

struct BSGeometryMesh {
	uint32_t triSize = 0;
	uint32_t numVerts = 0;
	uint32_t flags = 0;		// Often 64

	// in official files, this is 41 characters: hex characters from sha1 of the mesh data split into 2 parts
	// with a path separator. The game does not seem to check the digest, so the same name can be used for
	// replacement, or probably a human-readable one
	NiString meshName;		

	BSGeometryMeshData meshData;
	void Sync(NiStreamReversible& stream);
};

class BSGeometry : public NiCloneableStreamable<BSGeometry, NiShape> {
protected:
	BoundingSphere bounds;
	float boundMinMax[6]{};

	NiBlockRef<NiBoneContainer> skinInstanceRef;
	NiBlockRef<NiShader> shaderPropertyRef;
	NiBlockRef<NiAlphaProperty> alphaPropertyRef;

	std::vector<BSGeometryMesh> meshes;

	// A currently selected BSGeometryMesh in the list of meshes. All get/set data accessors use this to
	// address a desired mesh
	uint8_t selectedMesh = 0;

public:
	static constexpr const char* BlockName = "BSGeometry";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
		
	NiGeometryData* GetGeomData() const override;

	bool GetTriangles(std::vector<Triangle>& tris) const override;
	void SetTriangles(const std::vector<Triangle>& tris) override;

	uint8_t MeshCount() { return (uint8_t) meshes.size();	}

	// SelectMesh provides a way to choose which mesh from the BSGeometryMesh list data accesessors will use.
	// If this is not called, functions to retrieve vertices, triangles, etc will default to the first mesh.
	// Returns a pointer to the mesh data selected.
	// TODO: this is not thread safe.  A mutex should be set in SelectMesh and released in ReleaseMesh to
	// avoid synchronization issues.  Alternatively, Get/Set data functions could be changed to take a
	// selector option, but that's a significant API change.
	BSGeometryMesh* SelectMesh(uint8_t whichMesh) {
		if (whichMesh < meshes.size()) {
			selectedMesh = whichMesh;
			return &meshes[selectedMesh];
		}
		return nullptr;
	}
	// ReleaseMesh resets the selected mesh data to default.  This is a stand in for a mutex unlock operation
	// so should always be called as soon after SelectMesh as possble.
	void ReleaseMesh() {
		selectedMesh = 0;
		return;
	}
};

class NiSkinInstance;

class NiGeometry : public NiCloneableStreamable<NiGeometry, NiShape> {
protected:
	NiBlockRef<NiGeometryData> dataRef;
	NiBlockRef<NiBoneContainer> skinInstanceRef;
	NiBlockRef<NiShader> shaderPropertyRef;
	NiBlockRef<NiAlphaProperty> alphaPropertyRef;

public:
	NiSyncVector<NiStringRef> materialNames;
	NiVector<uint32_t> materialExtraData;

	int activeMaterial = 0;
	uint8_t defaultMatNeedsUpdateFlag = 0;

	bool shader = false;
	NiStringRef shaderName;
	uint32_t implementation = 0;

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;

	bool IsSkinned() const override;

	bool HasData() const override { return !dataRef.IsEmpty(); }
	NiBlockRef<NiGeometryData>* DataRef() override { return &dataRef; }
	const NiBlockRef<NiGeometryData>* DataRef() const override { return &dataRef; }

	bool HasSkinInstance() const override { return !skinInstanceRef.IsEmpty(); }
	NiBlockRef<NiBoneContainer>* SkinInstanceRef() override { return &skinInstanceRef; }
	const NiBlockRef<NiBoneContainer>* SkinInstanceRef() const override { return &skinInstanceRef; }

	bool HasShaderProperty() const override { return !shaderPropertyRef.IsEmpty(); }
	NiBlockRef<NiShader>* ShaderPropertyRef() override { return &shaderPropertyRef; }
	const NiBlockRef<NiShader>* ShaderPropertyRef() const override { return &shaderPropertyRef; }

	bool HasAlphaProperty() const override { return !alphaPropertyRef.IsEmpty(); }
	NiBlockRef<NiAlphaProperty>* AlphaPropertyRef() override { return &alphaPropertyRef; }
	const NiBlockRef<NiAlphaProperty>* AlphaPropertyRef() const override { return &alphaPropertyRef; }
};

class NiTriBasedGeom : public NiCloneable<NiTriBasedGeom, NiGeometry> {};

class NiTriBasedGeomData : public NiCloneableStreamable<NiTriBasedGeomData, NiGeometryData> {
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

class NiTriShapeData : public NiCloneableStreamable<NiTriShapeData, NiTriBasedGeomData> {
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

	std::vector<MatchGroup> GetMatchGroups() const;
	void SetMatchGroups(const std::vector<MatchGroup>& mg);

	uint32_t GetNumTriangles() const override;
	bool GetTriangles(std::vector<Triangle>& tris) const override;
	void SetTriangles(const std::vector<Triangle>& tris) override;

	void RecalcNormals(const bool smooth = true,
					   const float smoothThres = 60.0f,
					   std::unordered_set<uint32_t>* lockedIndices = nullptr) override;
	void CalcTangentSpace() override;
};

class NiTriShape : public NiCloneable<NiTriShape, NiTriBasedGeom> {
protected:
	NiTriShapeData* shapeData = nullptr;

public:
	static constexpr const char* BlockName = "NiTriShape";
	const char* GetBlockName() override { return BlockName; }

	NiGeometryData* GetGeomData() const override;
	void SetGeomData(NiGeometryData* geomDataPtr) override;
};

class StripsInfo {
public:
	NiVector<uint16_t, uint16_t> stripLengths;
	bool hasPoints = true;
	std::vector<std::vector<uint16_t>> points;

	void Sync(NiStreamReversible& stream);
};

class NiTriStripsData : public NiCloneableStreamable<NiTriStripsData, NiTriBasedGeomData> {
public:
	StripsInfo stripsInfo;

	static constexpr const char* BlockName = "NiTriStripsData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) override;

	uint32_t GetNumTriangles() const override;
	bool GetTriangles(std::vector<Triangle>& tris) const override;
	void SetTriangles(const std::vector<Triangle>& tris) override;
	std::vector<Triangle> StripsToTris() const;

	void RecalcNormals(const bool smooth = true,
					   const float smoothThres = 60.0f,
					   std::unordered_set<uint32_t>* lockedIndices = nullptr) override;
	void CalcTangentSpace() override;
};

class NiTriStrips : public NiCloneable<NiTriStrips, NiTriBasedGeom> {
protected:
	NiTriStripsData* stripsData = nullptr;

public:
	static constexpr const char* BlockName = "NiTriStrips";
	const char* GetBlockName() override { return BlockName; }

	NiGeometryData* GetGeomData() const override;
	void SetGeomData(NiGeometryData* geomDataPtr) override;

	bool ReorderTriangles(const std::vector<uint32_t>&) override { return false; }
};

class NiLinesData : public NiCloneableStreamable<NiLinesData, NiGeometryData> {
public:
	std::deque<bool> lineFlags;

	static constexpr const char* BlockName = "NiLinesData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) override;
};

class NiLines : public NiCloneable<NiLines, NiTriBasedGeom> {
protected:
	NiLinesData* linesData = nullptr;

public:
	static constexpr const char* BlockName = "NiLines";
	const char* GetBlockName() override { return BlockName; }

	NiGeometryData* GetGeomData() const override;
	void SetGeomData(NiGeometryData* geomDataPtr) override;
};

struct PolygonInfo {
	uint16_t numVertices = 0;
	uint16_t vertexOffset = 0;
	uint16_t numTriangles = 0;
	uint16_t triangleOffset = 0;
};

class NiScreenElementsData : public NiCloneableStreamable<NiScreenElementsData, NiTriShapeData> {
protected:
	uint16_t maxPolygons = 0;
	std::vector<PolygonInfo> polygons;
	std::vector<uint16_t> polygonIndices;

	uint16_t polygonGrowBy = 1;
	uint16_t numPolygons = 0;
	uint16_t maxVertices = 0;
	uint16_t verticesGrowBy = 1;
	uint16_t maxIndices = 0;
	uint16_t indicesGrowBy = 1;

public:
	static constexpr const char* BlockName = "NiScreenElementsData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) override;
};

class NiScreenElements : public NiCloneable<NiScreenElements, NiTriShape> {
protected:
	NiScreenElementsData* elemData = nullptr;

public:
	static constexpr const char* BlockName = "NiScreenElements";
	const char* GetBlockName() override { return BlockName; }

	NiGeometryData* GetGeomData() const override;
	void SetGeomData(NiGeometryData* geomDataPtr) override;
};

class BSLODTriShape : public NiCloneableStreamable<BSLODTriShape, NiTriBasedGeom> {
protected:
	NiTriShapeData* shapeData = nullptr;

public:
	uint32_t level0 = 0;
	uint32_t level1 = 0;
	uint32_t level2 = 0;

	static constexpr const char* BlockName = "BSLODTriShape";
	const char* GetBlockName() override { return BlockName; }

	NiGeometryData* GetGeomData() const override;
	void SetGeomData(NiGeometryData* geomDataPtr) override;

	void Sync(NiStreamReversible& stream);
};

class BSSegmentedTriShape : public NiCloneableStreamable<BSSegmentedTriShape, NiTriShape> {
protected:
	uint32_t numSegments = 0;
	std::vector<BSGeometrySegmentData> segments;

public:
	static constexpr const char* BlockName = "BSSegmentedTriShape";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	std::vector<BSGeometrySegmentData> GetSegments() const;
	void SetSegments(const std::vector<BSGeometrySegmentData>& sd);
};
} // namespace nifly
