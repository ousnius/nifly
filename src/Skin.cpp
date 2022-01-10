/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#include "Skin.hpp"
#include "NifUtil.hpp"

#include <unordered_map>

using namespace nifly;

void NiSkinData::Sync(NiStreamReversible& stream) {
	stream.Sync(skinTransform.rotation);
	stream.Sync(skinTransform.translation);
	stream.Sync(skinTransform.scale);
	stream.Sync(numBones);
	stream.Sync(hasVertWeights);

	if (hasVertWeights > 1)
		hasVertWeights = 1;

	bones.resize(numBones);
	for (uint32_t i = 0; i < numBones; i++) {
		auto& boneData = bones[i];
		stream.Sync(boneData.boneTransform.rotation);
		stream.Sync(boneData.boneTransform.translation);
		stream.Sync(boneData.boneTransform.scale);
		stream.Sync(boneData.bounds);

		uint16_t numVerts = boneData.numVertices;
		if (!hasVertWeights)
			numVerts = 0;

		stream.Sync(numVerts);

		if (stream.GetMode() == NiStreamReversible::Mode::Reading)
			boneData.numVertices = numVerts;

		boneData.vertexWeights.resize(numVerts);

		// Num Verts * 6 bytes (index + weight)
		stream.Sync((char*) boneData.vertexWeights.data(),
					static_cast<std::streamsize>(numVerts) * sizeof(SkinWeight));
	}
}

void NiSkinData::notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) {
	uint16_t highestRemoved = vertIndices.back();
	uint16_t mapSize = highestRemoved + 1;
	std::vector<int> indexCollapse = GenerateIndexCollapseMap(vertIndices, mapSize);

	NiObject::notifyVerticesDelete(vertIndices);

	uint16_t ival = 0;
	for (auto& b : bones) {
		for (uint16_t i = b.numVertices - 1; i != static_cast<uint16_t>(-1); i--) {
			ival = b.vertexWeights[i].index;
			if (b.vertexWeights[i].index > highestRemoved) {
				b.vertexWeights[i].index -= static_cast<uint16_t>(vertIndices.size());
			}
			else if (indexCollapse[ival] == -1) {
				b.vertexWeights.erase(b.vertexWeights.begin() + i);
				b.numVertices--;
			}
			else
				b.vertexWeights[i].index = static_cast<uint16_t>(indexCollapse[ival]);
		}
	}
}


void NiSkinPartition::Sync(NiStreamReversible& stream) {
	stream.Sync(numPartitions);
	partitions.resize(numPartitions);

	if (stream.GetVersion().User() >= 12 && stream.GetVersion().Stream() == 100) {
		if (stream.GetMode() == NiStreamReversible::Mode::Reading)
			bMappedIndices = false;

		if (stream.GetMode() == NiStreamReversible::Mode::Writing)
			dataSize = vertexSize * numVertices;

		stream.Sync(dataSize);
		stream.Sync(vertexSize);
		vertexDesc.Sync(stream);

		if (dataSize > 0) {
			if (stream.GetMode() == NiStreamReversible::Mode::Reading)
				numVertices = dataSize / vertexSize;

			vertData.resize(numVertices);

			for (uint32_t i = 0; i < numVertices; i++) {
				auto& vertex = vertData[i];
				if (HasVertices()) {
					if (IsFullPrecision()) {
						// Full precision (vert + bitangentX = 16 bytes)
						stream.Sync((char*) &vertex.vert, sizeof(vertex.vert) + sizeof(vertex.bitangentX));
					}
					else {
						// Half precision (vert + bitangentX = 8 bytes)
						stream.SyncHalf(vertex.vert.x);
						stream.SyncHalf(vertex.vert.y);
						stream.SyncHalf(vertex.vert.z);

						stream.SyncHalf(vertex.bitangentX);
					}
				}

				if (HasUVs()) {
					stream.SyncHalf(vertex.uv.u);
					stream.SyncHalf(vertex.uv.v);
				}

				if (HasNormals()) {
					// 3 normals + bitangentY = 4 bytes
					stream.Sync((char*) &vertex.normal, sizeof(vertex.normal) + sizeof(vertex.bitangentY));

					if (HasTangents()) {
						// 3 tangents + bitangentZ = 4 bytes
						stream.Sync((char*) &vertex.tangent,
									sizeof(vertex.tangent) + sizeof(vertex.bitangentZ));
					}
				}

				if (HasVertexColors()) {
					// 4 vertex colors = 4 bytes
					stream.Sync((char*) &vertex.colorData, sizeof(vertex.colorData));
				}

				if (IsSkinned()) {
					// 4 weights = 8 bytes
					for (float& weight : vertex.weights)
						stream.SyncHalf(weight);

					// 4 bones = 4 bytes
					stream.Sync((char*) &vertex.weightBones, sizeof(vertex.weightBones));
				}

				if (HasEyeData())
					stream.Sync(vertex.eyeData);
			}
		}
	}

	// This call of PrepareVertexMapsAndTriangles should be completely unnecessary.
	// But it doesn't hurt to be safe.
	if (stream.GetMode() == NiStreamReversible::Mode::Writing)
		PrepareVertexMapsAndTriangles();

	for (uint32_t p = 0; p < numPartitions; p++) {
		auto& partition = partitions[p];
		stream.Sync(partition.numVertices);
		stream.Sync(partition.numTriangles);
		stream.Sync(partition.numBones);
		stream.Sync(partition.numStrips);
		stream.Sync(partition.numWeightsPerVertex);

		partition.bones.resize(partition.numBones);
		stream.Sync((char*) partition.bones.data(), partition.numBones * sizeof(uint16_t));

		stream.Sync(partition.hasVertexMap);
		if (partition.hasVertexMap) {
			partition.vertexMap.resize(partition.numVertices);
			stream.Sync((char*) partition.vertexMap.data(), partition.numVertices * sizeof(uint16_t));
		}

		stream.Sync(partition.hasVertexWeights);
		if (partition.hasVertexWeights) {
			partition.vertexWeights.resize(partition.numVertices);
			stream.Sync((char*) partition.vertexWeights.data(), partition.numVertices * sizeof(VertexWeight));
		}

		partition.stripLengths.resize(partition.numStrips);
		stream.Sync((char*) partition.stripLengths.data(), partition.numStrips * sizeof(uint16_t));

		stream.Sync(partition.hasFaces);
		if (partition.hasFaces) {
			partition.strips.resize(partition.numStrips);
			for (uint32_t i = 0; i < partition.numStrips; i++) {
				partition.strips[i].resize(partition.stripLengths[i]);
				stream.Sync((char*) partition.strips[i].data(), partition.stripLengths[i] * sizeof(uint16_t));
			}
		}

		if (partition.numStrips == 0 && partition.hasFaces) {
			partition.triangles.resize(partition.numTriangles);
			stream.Sync((char*) partition.triangles.data(), partition.numTriangles * sizeof(Triangle));
		}

		stream.Sync(partition.hasBoneIndices);
		if (partition.hasBoneIndices) {
			partition.boneIndices.resize(partition.numVertices);
			stream.Sync((char*) partition.boneIndices.data(), partition.numVertices * sizeof(BoneIndices));
		}

		if (stream.GetVersion().User() >= 12) {
			stream.Sync(partition.lodLevel);
			stream.Sync(partition.globalVB);
		}

		if (stream.GetVersion().User() >= 12 && stream.GetVersion().Stream() == 100) {
			partition.vertexDesc.Sync(stream);

			partition.trueTriangles.resize(partition.numTriangles);
			stream.Sync((char*) partition.trueTriangles.data(), partition.numTriangles * sizeof(Triangle));
		}
	}
}

void NiSkinPartition::notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) {
	if (vertIndices.empty())
		return;

	NiObject::notifyVerticesDelete(vertIndices);

	// Prepare vertexMap and triangles.
	ConvertStripsToTriangles();
	PrepareVertexMapsAndTriangles();
	triParts.clear();

	// Determine maximum vertex index used so we can make a complete
	// collapse map.  (It would be nice if notifyVerticesDelete had a
	// numVertices parameter so we didn't have to calculate this.)
	uint16_t maxVertInd = 0;
	for (auto& p : partitions) {
		for (auto i : p.vertexMap)
			maxVertInd = std::max(maxVertInd, i);
		if (!bMappedIndices)
			maxVertInd = std::max(maxVertInd, CalcMaxTriangleIndex(p.triangles));
	}

	uint16_t mapSize = maxVertInd + 1;

	// Make collapse map for shape vertex indices
	std::vector<int> indexCollapse = GenerateIndexCollapseMap(vertIndices, mapSize);

	for (auto& p : partitions) {
		const size_t oldNumVertices = p.vertexMap.size();

		// Make list of deleted vertexMap indices
		std::vector<uint32_t> vertexMapDelList;
		for (uint32_t i = 0; i < static_cast<uint32_t>(p.vertexMap.size()); i++)
			if (indexCollapse[p.vertexMap[i]] == -1)
				vertexMapDelList.push_back(i);

		// Erase indices of vertexMap, vertexWeights, and boneIndices
		EraseVectorIndices(p.vertexMap, vertexMapDelList);
		if (p.hasVertexWeights)
			EraseVectorIndices(p.vertexWeights, vertexMapDelList);
		if (p.hasBoneIndices)
			EraseVectorIndices(p.boneIndices, vertexMapDelList);
		p.numVertices = static_cast<uint16_t>(p.vertexMap.size());

		// Compose vertexMap with indexCollapse to get new vertexMap
		for (uint16_t& i : p.vertexMap)
			i = static_cast<uint16_t>(indexCollapse[i]);

		if (!bMappedIndices) {
			// Apply shape vertex index collapse map to true triangles
			ApplyMapToTriangles(p.triangles, indexCollapse);
			p.trueTriangles = p.triangles;
		}
		else {
			// Generate collapse map for indices into (old) vertexMap.
			std::vector<int> mapCollapse = GenerateIndexCollapseMap(vertexMapDelList, oldNumVertices);
			// Apply vertexMap index collapse to mapped triangles
			ApplyMapToTriangles(p.triangles, mapCollapse);
			p.trueTriangles.clear();
		}
		p.numTriangles = static_cast<uint16_t>(p.triangles.size());
	}

	if (!vertData.empty()) {
		EraseVectorIndices(vertData, vertIndices);
		numVertices = static_cast<uint32_t>(vertData.size());
	}
}

void NiSkinPartition::DeletePartitions(const std::vector<uint32_t>& partInds) {
	if (partInds.empty())
		return;

	if (!triParts.empty()) {
		std::vector<int> piMap = GenerateIndexCollapseMap(partInds, numPartitions);
		const auto piMapSize = static_cast<int>(piMap.size());
		for (auto& pi : triParts) {
			if (pi >= 0 && pi < piMapSize)
				pi = piMap[pi];
		}
	}

	EraseVectorIndices(partitions, partInds);
	numPartitions = static_cast<uint32_t>(partitions.size());
}

uint32_t NiSkinPartition::RemoveEmptyPartitions(std::vector<uint32_t>& outDeletedIndices) {
	outDeletedIndices.clear();

	for (uint32_t i = 0; i < static_cast<uint32_t>(partitions.size()); ++i)
		if (partitions[i].numTriangles == 0)
			outDeletedIndices.push_back(i);

	if (!outDeletedIndices.empty())
		DeletePartitions(outDeletedIndices);

	return static_cast<uint32_t>(outDeletedIndices.size());
}

bool NiSkinPartition::PartitionBlock::ConvertStripsToTriangles() {
	if (numStrips == 0)
		return false;

	hasFaces = true;
	triangles = GenerateTrianglesFromStrips(strips);
	numTriangles = static_cast<uint16_t>(triangles.size());
	numStrips = 0;
	strips.clear();
	stripLengths.clear();
	trueTriangles.clear();
	return true;
}

bool NiSkinPartition::ConvertStripsToTriangles() {
	bool triangulated = false;
	for (PartitionBlock& p : partitions) {
		if (p.ConvertStripsToTriangles())
			triangulated = true;
	}
	return triangulated;
}

void NiSkinPartition::PartitionBlock::GenerateTrueTrianglesFromMappedTriangles() {
	if (vertexMap.empty() || triangles.empty()) {
		trueTriangles.clear();
		if (numStrips == 0)
			numTriangles = 0;
		return;
	}

	trueTriangles = triangles;
	ApplyMapToTriangles(trueTriangles, vertexMap);

	for (Triangle& t : trueTriangles)
		t.rot();

	if (triangles.size() != trueTriangles.size()) {
		triangles.clear();
		numTriangles = static_cast<uint16_t>(trueTriangles.size());
	}
}

void NiSkinPartition::PartitionBlock::GenerateMappedTrianglesFromTrueTrianglesAndVertexMap() {
	if (vertexMap.empty() || trueTriangles.empty()) {
		triangles.clear();
		if (numStrips == 0)
			numTriangles = 0;
		return;
	}

	std::vector<uint16_t> invmap(vertexMap.back() + 1);
	for (uint16_t mi = 0; mi < static_cast<uint16_t>(vertexMap.size()); ++mi) {
		if (vertexMap[mi] >= invmap.size())
			invmap.resize(vertexMap[mi] + 1);

		invmap[vertexMap[mi]] = mi;
	}

	triangles = trueTriangles;
	ApplyMapToTriangles(triangles, invmap);

	for (Triangle& t : triangles)
		t.rot();

	if (triangles.size() != trueTriangles.size()) {
		trueTriangles.clear();
		numTriangles = static_cast<uint16_t>(triangles.size());
	}
}

void NiSkinPartition::PartitionBlock::GenerateVertexMapFromTrueTriangles() {
	std::vector<bool> vertUsed(CalcMaxTriangleIndex(trueTriangles) + 1, false);
	for (auto& trueTriangle : trueTriangles) {
		vertUsed[trueTriangle.p1] = true;
		vertUsed[trueTriangle.p2] = true;
		vertUsed[trueTriangle.p3] = true;
	}

	vertexMap.clear();

	for (uint16_t i = 0; i < static_cast<uint16_t>(vertUsed.size()); ++i) {
		if (vertUsed[i])
			vertexMap.push_back(i);
	}

	numVertices = static_cast<uint16_t>(vertexMap.size());
}

void NiSkinPartition::PrepareTrueTriangles() {
	for (PartitionBlock& p : partitions) {
		if (!p.trueTriangles.empty())
			continue;

		if (p.numStrips)
			p.ConvertStripsToTriangles();

		if (bMappedIndices)
			p.GenerateTrueTrianglesFromMappedTriangles();
		else
			p.trueTriangles = p.triangles;
	}
}

void NiSkinPartition::PrepareVertexMapsAndTriangles() {
	for (PartitionBlock& p : partitions) {
		if (p.vertexMap.empty())
			p.GenerateVertexMapFromTrueTriangles();

		if (p.triangles.empty()) {
			if (bMappedIndices)
				p.GenerateMappedTrianglesFromTrueTrianglesAndVertexMap();
			else
				p.triangles = p.trueTriangles;
		}
	}
}

void NiSkinPartition::GenerateTriPartsFromTrueTriangles(const std::vector<Triangle>& shapeTris) {
	triParts.clear();
	triParts.resize(shapeTris.size());

	// Make a map from Triangles to their indices in shapeTris
	std::unordered_map<Triangle, int> shapeTriInds;

	int numTris = static_cast<int>(shapeTris.size());
	for (int triInd = 0; triInd < numTris; ++triInd) {
		Triangle t = shapeTris[triInd];
		t.rot();
		shapeTriInds[t] = triInd;
	}

	// Set triParts for each partition triangle
	int numParts = static_cast<int>(partitions.size());
	for (int partInd = 0; partInd < numParts; ++partInd) {
		for (const Triangle& pt : partitions[partInd].trueTriangles) {
			Triangle t = pt;
			t.rot();
			auto it = shapeTriInds.find(t);
			if (it != shapeTriInds.end())
				triParts[it->second] = partInd;
		}
	}
}

void NiSkinPartition::GenerateTrueTrianglesFromTriParts(const std::vector<Triangle>& shapeTris) {
	if (shapeTris.size() != triParts.size())
		return;

	for (PartitionBlock& p : partitions) {
		p.trueTriangles.clear();
		p.triangles.clear();
		p.numStrips = 0;
		p.strips.clear();
		p.stripLengths.clear();
		p.hasFaces = true;
		p.vertexMap.clear();
		p.vertexWeights.clear();
		p.boneIndices.clear();
	}

	for (size_t triInd = 0; triInd < shapeTris.size(); ++triInd) {
		const auto partitionsSize = static_cast<int>(partitions.size());
		const int partInd = triParts[triInd];
		if (partInd >= 0 && partInd < partitionsSize)
			partitions[partInd].trueTriangles.push_back(shapeTris[triInd]);
	}

	for (PartitionBlock& p : partitions)
		p.numTriangles = static_cast<uint16_t>(p.trueTriangles.size());
}

void NiSkinPartition::PrepareTriParts(const std::vector<Triangle>& shapeTris) {
	if (shapeTris.size() == triParts.size())
		return;
	PrepareTrueTriangles();
	GenerateTriPartsFromTrueTriangles(shapeTris);
}


void NiSkinInstance::Sync(NiStreamReversible& stream) {
	dataRef.Sync(stream);
	skinPartitionRef.Sync(stream);
	targetRef.Sync(stream);
	boneRefs.Sync(stream);
}

void NiSkinInstance::GetChildRefs(std::set<NiRef*>& refs) {
	NiObject::GetChildRefs(refs);

	refs.insert(&dataRef);
	refs.insert(&skinPartitionRef);
}

void NiSkinInstance::GetChildIndices(std::vector<uint32_t>& indices) {
	NiObject::GetChildIndices(indices);

	indices.push_back(dataRef.index);
	indices.push_back(skinPartitionRef.index);
}

void NiSkinInstance::GetPtrs(std::set<NiPtr*>& ptrs) {
	NiObject::GetPtrs(ptrs);

	ptrs.insert(&targetRef);
	boneRefs.GetIndexPtrs(ptrs);
}


void BSDismemberSkinInstance::Sync(NiStreamReversible& stream) {
	partitions.Sync(stream);
}

void BSDismemberSkinInstance::DeletePartitions(const std::vector<uint32_t>& partInds) {
	if (partInds.empty())
		return;

	EraseVectorIndices(partitions, partInds);
}


void BSSkinBoneData::Sync(NiStreamReversible& stream) {
	stream.Sync(nBones);
	boneXforms.resize(nBones);
	for (uint32_t i = 0; i < nBones; i++) {
		stream.Sync(boneXforms[i].bounds);
		stream.Sync(boneXforms[i].boneTransform.rotation);
		stream.Sync(boneXforms[i].boneTransform.translation);
		stream.Sync(boneXforms[i].boneTransform.scale);
	}
}


void BSSkinInstance::Sync(NiStreamReversible& stream) {
	targetRef.Sync(stream);
	dataRef.Sync(stream);
	boneRefs.Sync(stream);
	scales.Sync(stream);
}

void BSSkinInstance::GetChildRefs(std::set<NiRef*>& refs) {
	NiObject::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void BSSkinInstance::GetChildIndices(std::vector<uint32_t>& indices) {
	NiObject::GetChildIndices(indices);

	indices.push_back(dataRef.index);
}

void BSSkinInstance::GetPtrs(std::set<NiPtr*>& ptrs) {
	NiObject::GetPtrs(ptrs);

	ptrs.insert(&targetRef);
	boneRefs.GetIndexPtrs(ptrs);
}
