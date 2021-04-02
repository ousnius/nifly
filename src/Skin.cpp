/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#include "Skin.hpp"
#include "NifUtil.hpp"
#include "half.hpp"

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

		for (int j = 0; j < numVerts; j++) {
			stream.Sync(boneData.vertexWeights[j].index);
			stream.Sync(boneData.vertexWeights[j].weight);
		}
	}
}

void NiSkinData::notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) {
	uint16_t highestRemoved = vertIndices.back();
	std::vector<int> indexCollapse = GenerateIndexCollapseMap(vertIndices, highestRemoved + 1);

	NiObject::notifyVerticesDelete(vertIndices);

	uint16_t ival = 0;
	for (auto& b : bones) {
		for (int i = b.numVertices - 1; i >= 0; i--) {
			ival = b.vertexWeights[i].index;
			if (b.vertexWeights[i].index > highestRemoved) {
				b.vertexWeights[i].index -= vertIndices.size();
			}
			else if (indexCollapse[ival] == -1) {
				b.vertexWeights.erase(b.vertexWeights.begin() + i);
				b.numVertices--;
			}
			else
				b.vertexWeights[i].index = indexCollapse[ival];
		}
	}
}


void NiSkinPartition::Sync(NiStreamReversible& stream) {
	stream.Sync(numPartitions);
	partitions.resize(numPartitions);

	if (stream.GetVersion().User() >= 12 && stream.GetVersion().Stream() == 100) {
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

			half_float::half halfData;
			for (uint32_t i = 0; i < numVertices; i++) {
				auto& vertex = vertData[i];
				if (HasVertices()) {
					if (IsFullPrecision()) {
						// Full precision
						stream.Sync(vertex.vert);
						stream.Sync(vertex.bitangentX);
					}
					else {
						// Half precision
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
					for (uint8_t& j : vertex.normal)
						stream.Sync(j);

					stream.Sync(vertex.bitangentY);

					if (HasTangents()) {
						for (uint8_t& j : vertex.tangent)
							stream.Sync(j);

						stream.Sync(vertex.bitangentZ);
					}
				}

				if (HasVertexColors())
					for (uint8_t& j : vertex.colorData)
						stream.Sync(j);

				if (IsSkinned()) {
					for (float& weight : vertex.weights)
						stream.SyncHalf(weight);

					for (uint8_t& weightBone : vertex.weightBones)
						stream.Sync(weightBone);
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
		for (uint32_t i = 0; i < partition.numBones; i++)
			stream.Sync(partition.bones[i]);

		stream.Sync(partition.hasVertexMap);
		if (partition.hasVertexMap) {
			partition.vertexMap.resize(partition.numVertices);
			for (uint32_t i = 0; i < partition.numVertices; i++)
				stream.Sync(partition.vertexMap[i]);
		}

		stream.Sync(partition.hasVertexWeights);
		if (partition.hasVertexWeights) {
			partition.vertexWeights.resize(partition.numVertices);
			for (uint32_t i = 0; i < partition.numVertices; i++)
				stream.Sync(partition.vertexWeights[i]);
		}

		partition.stripLengths.resize(partition.numStrips);
		for (uint32_t i = 0; i < partition.numStrips; i++)
			stream.Sync(partition.stripLengths[i]);

		stream.Sync(partition.hasFaces);
		if (partition.hasFaces) {
			partition.strips.resize(partition.numStrips);
			for (uint32_t i = 0; i < partition.numStrips; i++) {
				partition.strips[i].resize(partition.stripLengths[i]);
				for (int j = 0; j < partition.stripLengths[i]; j++)
					stream.Sync(partition.strips[i][j]);
			}
		}

		if (partition.numStrips == 0 && partition.hasFaces) {
			partition.triangles.resize(partition.numTriangles);
			for (uint32_t i = 0; i < partition.numTriangles; i++)
				stream.Sync(partition.triangles[i]);
		}

		stream.Sync(partition.hasBoneIndices);
		if (partition.hasBoneIndices) {
			partition.boneIndices.resize(partition.numVertices);
			for (uint32_t i = 0; i < partition.numVertices; i++)
				stream.Sync(partition.boneIndices[i]);
		}

		if (stream.GetVersion().User() >= 12) {
			stream.Sync(partition.lodLevel);
			stream.Sync(partition.globalVB);
		}

		if (stream.GetVersion().User() >= 12 && stream.GetVersion().Stream() == 100) {
			partition.vertexDesc.Sync(stream);

			partition.trueTriangles.resize(partition.numTriangles);
			for (uint32_t i = 0; i < partition.numTriangles; i++)
				stream.Sync(partition.trueTriangles[i]);
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

	// Make collapse map for shape vertex indices
	std::vector<int> indexCollapse = GenerateIndexCollapseMap(vertIndices, maxVertInd + 1);

	for (auto& p : partitions) {
		int oldNumVertices = p.vertexMap.size();

		// Make list of deleted vertexMap indices
		std::vector<int> vertexMapDelList;
		for (size_t i = 0; i < p.vertexMap.size(); i++)
			if (indexCollapse[p.vertexMap[i]] == -1)
				vertexMapDelList.push_back(i);

		// Erase indices of vertexMap, vertexWeights, and boneIndices
		EraseVectorIndices(p.vertexMap, vertexMapDelList);
		if (p.hasVertexWeights)
			EraseVectorIndices(p.vertexWeights, vertexMapDelList);
		if (p.hasBoneIndices)
			EraseVectorIndices(p.boneIndices, vertexMapDelList);
		p.numVertices = p.vertexMap.size();

		// Compose vertexMap with indexCollapse to get new vertexMap
		for (uint16_t& i : p.vertexMap)
			i = indexCollapse[i];

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
		p.numTriangles = p.triangles.size();
	}

	if (!vertData.empty()) {
		EraseVectorIndices(vertData, vertIndices);
		numVertices = vertData.size();
	}
}

void NiSkinPartition::DeletePartitions(const std::vector<int>& partInds) {
	if (partInds.empty())
		return;
	if (!triParts.empty()) {
		std::vector<int> piMap = GenerateIndexCollapseMap(partInds, numPartitions);
		for (int& pi : triParts) {
			if (pi >= 0 && pi < piMap.size())
				pi = piMap[pi];
		}
	}
	EraseVectorIndices(partitions, partInds);
	numPartitions = partitions.size();
}

int NiSkinPartition::RemoveEmptyPartitions(std::vector<int>& outDeletedIndices) {
	outDeletedIndices.clear();
	for (size_t i = 0; i < partitions.size(); ++i)
		if (partitions[i].numTriangles == 0)
			outDeletedIndices.push_back(i);
	if (!outDeletedIndices.empty())
		DeletePartitions(outDeletedIndices);
	return outDeletedIndices.size();
}

bool NiSkinPartition::PartitionBlock::ConvertStripsToTriangles() {
	if (numStrips == 0)
		return false;
	hasFaces = true;
	triangles = GenerateTrianglesFromStrips(strips);
	numTriangles = triangles.size();
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
		numTriangles = trueTriangles.size();
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
	for (unsigned int mi = 0; mi < vertexMap.size(); ++mi) {
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
		numTriangles = triangles.size();
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

	for (unsigned int i = 0; i < vertUsed.size(); ++i) {
		if (vertUsed[i])
			vertexMap.push_back(i);
	}

	numVertices = vertexMap.size();
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
	for (size_t triInd = 0; triInd < shapeTris.size(); ++triInd) {
		Triangle t = shapeTris[triInd];
		t.rot();
		shapeTriInds[t] = triInd;
	}

	// Set triParts for each partition triangle
	for (size_t partInd = 0; partInd < partitions.size(); ++partInd) {
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
		int partInd = triParts[triInd];
		if (partInd >= 0 && partInd < partitions.size())
			partitions[partInd].trueTriangles.push_back(shapeTris[triInd]);
	}
	for (PartitionBlock& p : partitions)
		p.numTriangles = p.trueTriangles.size();
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

void NiSkinInstance::GetChildIndices(std::vector<int>& indices) {
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
	stream.Sync(numPartitions);
	partitions.resize(numPartitions);

	for (int i = 0; i < numPartitions; i++)
		stream.Sync(partitions[i]);
}

void BSDismemberSkinInstance::AddPartition(const BSDismemberSkinInstance::PartitionInfo& part) {
	partitions.push_back(part);
	numPartitions++;
}

void BSDismemberSkinInstance::DeletePartitions(const std::vector<int>& partInds) {
	if (partInds.empty())
		return;
	EraseVectorIndices(partitions, partInds);
	numPartitions = partitions.size();
}

void BSDismemberSkinInstance::RemovePartition(const int id) {
	if (id >= 0 && id < numPartitions) {
		partitions.erase(partitions.begin() + id);
		numPartitions--;
	}
}

void BSDismemberSkinInstance::ClearPartitions() {
	partitions.clear();
	numPartitions = 0;
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

void BSSkinInstance::GetChildIndices(std::vector<int>& indices) {
	NiObject::GetChildIndices(indices);

	indices.push_back(dataRef.index);
}

void BSSkinInstance::GetPtrs(std::set<NiPtr*>& ptrs) {
	NiObject::GetPtrs(ptrs);

	ptrs.insert(&targetRef);
	boneRefs.GetIndexPtrs(ptrs);
}
