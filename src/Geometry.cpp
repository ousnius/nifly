/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#include "Geometry.hpp"
#include "Nodes.hpp"
#include "Skin.hpp"

#include "KDMatcher.hpp"
#include "NifUtil.hpp"

#include <array>

using namespace nifly;

void NiAdditionalGeometryData::Sync(NiStreamReversible& stream) {
	stream.Sync(numVertices);

	blockInfos.Sync(stream);
	blocks.Sync(stream);
}


void BSPackedAdditionalGeometryData::Sync(NiStreamReversible& stream) {
	stream.Sync(numVertices);

	blockInfos.Sync(stream);
	blocks.Sync(stream);
}


void NiGeometryData::Sync(NiStreamReversible& stream) {
	if (stream.GetVersion().File() >= NiFileVersion::V10_1_0_114)
		stream.Sync(groupID);

	stream.Sync(numVertices);

	if (stream.GetVersion().File() >= NiFileVersion::V10_1_0_0) {
		stream.Sync(keepFlags);
		stream.Sync(compressFlags);
	}

	stream.Sync(hasVertices);

	if (hasVertices && (!isPSys || stream.GetVersion().File() < V20_2_0_7)) {
		vertices.resize(numVertices);
		for (uint16_t i = 0; i < numVertices; i++)
			stream.Sync(vertices[i]);
	}

	// Disable tangent flag for OB
	if (stream.GetVersion().IsOB())
		dataFlags &= ~(1 << 12);

	if (stream.GetVersion().File() >= NiFileVersion::V10_0_1_0)
		stream.Sync(dataFlags);

	uint16_t nbtMethod = dataFlags & 0xF000;
	uint8_t numTextureSets = dataFlags & 0x3F;
	if (stream.GetVersion().Stream() >= 34)
		numTextureSets = dataFlags & 0x1;

	if (stream.GetVersion().File() == NiFileVersion::V20_2_0_7 && stream.GetVersion().Stream() > 34)
		stream.Sync(materialCRC);

	stream.Sync(hasNormals);
	if (hasNormals && (!isPSys || stream.GetVersion().File() < V20_2_0_7)) {
		normals.resize(numVertices);

		for (uint16_t i = 0; i < numVertices; i++)
			stream.Sync(normals[i]);

		if (nbtMethod) {
			tangents.resize(numVertices);
			bitangents.resize(numVertices);

			for (uint16_t i = 0; i < numVertices; i++)
				stream.Sync(tangents[i]);

			for (uint16_t i = 0; i < numVertices; i++)
				stream.Sync(bitangents[i]);
		}
	}

	stream.Sync(bounds);

	stream.Sync(hasVertexColors);
	if (hasVertexColors && (!isPSys || stream.GetVersion().File() < V20_2_0_7)) {
		vertexColors.resize(numVertices);
		for (uint16_t i = 0; i < numVertices; i++)
			stream.Sync(vertexColors[i]);
	}

	if (numTextureSets > 0 && (!isPSys || stream.GetVersion().File() < V20_2_0_7)) {
		uvSets.resize(numTextureSets);
		for (uint32_t i = 0; i < numTextureSets; i++) {
			uvSets[i].resize(numVertices);
			for (uint16_t j = 0; j < numVertices; j++)
				stream.Sync(uvSets[i][j]);
		}
	}

	stream.Sync(consistencyFlags);

	if (stream.GetVersion().File() >= NiFileVersion::V20_0_0_4)
		additionalDataRef.Sync(stream);
}

void NiGeometryData::GetChildRefs(std::set<NiRef*>& refs) {
	NiObject::GetChildRefs(refs);

	refs.insert(&additionalDataRef);
}

void NiGeometryData::GetChildIndices(std::vector<uint32_t>& indices) {
	NiObject::GetChildIndices(indices);

	indices.push_back(additionalDataRef.index);
}

uint16_t NiGeometryData::GetNumVertices() const {
	return numVertices;
}

void NiGeometryData::SetVertices(const bool enable) {
	hasVertices = enable;
	if (enable) {
		vertices.resize(numVertices);
	}
	else {
		vertices.clear();
		numVertices = 0;

		SetNormals(false);
		SetVertexColors(false);
		SetUVs(false);
		SetTangents(false);
	}
}

void NiGeometryData::SetNormals(const bool enable) {
	hasNormals = enable;
	if (enable)
		normals.resize(numVertices);
	else
		normals.clear();
}

void NiGeometryData::SetVertexColors(const bool enable) {
	hasVertexColors = enable;
	if (enable)
		vertexColors.resize(numVertices, Color4(1.0f, 1.0f, 1.0f, 1.0f));
	else
		vertexColors.clear();
}

void NiGeometryData::SetUVs(const bool enable) {
	if (enable) {
		dataFlags |= 1 << 0;
		uvSets.resize(1);
		uvSets[0].resize(numVertices);
	}
	else {
		dataFlags &= ~(1 << 0);
		uvSets.clear();
	}
}

void NiGeometryData::SetTangents(const bool enable) {
	if (enable) {
		dataFlags |= 1 << 12;
		tangents.resize(numVertices);
		bitangents.resize(numVertices);
	}
	else {
		dataFlags &= ~(1 << 12);
		tangents.clear();
		bitangents.clear();
	}
}

uint32_t NiGeometryData::GetNumTriangles() const {
	return 0;
}
bool NiGeometryData::GetTriangles(std::vector<Triangle>&) const {
	return false;
}
void NiGeometryData::SetTriangles(const std::vector<Triangle>&){};

void NiGeometryData::UpdateBounds() {
	bounds = BoundingSphere(vertices);
}

void NiGeometryData::Create(NiVersion&,
							const std::vector<Vector3>* verts,
							const std::vector<Triangle>*,
							const std::vector<Vector2>* uvs,
							const std::vector<Vector3>* norms) {
	size_t vertCount = verts->size();
	constexpr uint16_t maxIndex = std::numeric_limits<uint16_t>::max();

	if (vertCount > static_cast<size_t>(maxIndex))
		numVertices = maxIndex;
	else
		numVertices = uint16_t(vertCount);

	vertices.resize(numVertices);
	for (uint16_t v = 0; v < numVertices; v++)
		vertices[v] = (*verts)[v];

	bounds = BoundingSphere(vertices);

	if (uvs) {
		size_t uvCount = uvs->size();
		if (uvCount == numVertices) {
			SetUVs(true);

			for (size_t uv = 0; uv < uvSets[0].size(); uv++)
				uvSets[0][uv] = (*uvs)[uv];
		}
		else {
			SetUVs(false);
		}
	}
	else {
		SetUVs(false);
	}

	if (norms && norms->size() == numVertices) {
		SetNormals(true);
		normals = (*norms);
		CalcTangentSpace();
	}
	else {
		SetNormals(false);
		SetTangents(false);
	}
}

void NiGeometryData::notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) {
	EraseVectorIndices(vertices, vertIndices);
	numVertices = static_cast<uint16_t>(vertices.size());
	if (!normals.empty())
		EraseVectorIndices(normals, vertIndices);
	if (!tangents.empty())
		EraseVectorIndices(tangents, vertIndices);
	if (!bitangents.empty())
		EraseVectorIndices(bitangents, vertIndices);
	if (!vertexColors.empty())
		EraseVectorIndices(vertexColors, vertIndices);
	for (auto& uvSet : uvSets)
		EraseVectorIndices(uvSet, vertIndices);
}

void NiGeometryData::RecalcNormals(const bool, const float, std::unordered_set<uint32_t>*) {
	SetNormals(true);
}

void NiGeometryData::CalcTangentSpace() {
	SetTangents(true);
}


uint16_t NiShape::GetNumVertices() const {
	auto geomData = GetGeomData();
	if (geomData)
		return geomData->GetNumVertices();

	return 0;
}

void NiShape::SetVertices(const bool enable) {
	auto geomData = GetGeomData();
	if (geomData)
		geomData->SetVertices(enable);
};

bool NiShape::HasVertices() const {
	auto geomData = GetGeomData();
	if (geomData)
		return geomData->HasVertices();

	return false;
};

void NiShape::SetUVs(const bool enable) {
	auto geomData = GetGeomData();
	if (geomData)
		geomData->SetUVs(enable);
};

bool NiShape::HasUVs() const {
	auto geomData = GetGeomData();
	if (geomData)
		return geomData->HasUVs();

	return false;
};

void NiShape::SetNormals(const bool enable) {
	auto geomData = GetGeomData();
	if (geomData)
		geomData->SetNormals(enable);
};

bool NiShape::HasNormals() const {
	auto geomData = GetGeomData();
	if (geomData)
		return geomData->HasNormals();

	return false;
};

void NiShape::SetTangents(const bool enable) {
	auto geomData = GetGeomData();
	if (geomData)
		geomData->SetTangents(enable);
};

bool NiShape::HasTangents() const {
	auto geomData = GetGeomData();
	if (geomData)
		return geomData->HasTangents();

	return false;
};

void NiShape::SetVertexColors(const bool enable) {
	auto geomData = GetGeomData();
	if (geomData)
		geomData->SetVertexColors(enable);
};

bool NiShape::HasVertexColors() const {
	auto geomData = GetGeomData();
	if (geomData)
		return geomData->HasVertexColors();

	return false;
};

void NiShape::SetSkinned(const bool){};
bool NiShape::IsSkinned() const {
	return false;
};

uint32_t NiShape::GetNumTriangles() const {
	auto geomData = GetGeomData();
	if (geomData)
		return geomData->GetNumTriangles();

	return 0;
}

bool NiShape::GetTriangles(std::vector<Triangle>& tris) const {
	auto geomData = GetGeomData();
	if (geomData)
		return geomData->GetTriangles(tris);

	return false;
};

void NiShape::SetTriangles(const std::vector<Triangle>& tris) {
	auto geomData = GetGeomData();
	if (geomData)
		geomData->SetTriangles(tris);
};

void NiShape::SetBounds(const BoundingSphere& bounds) {
	auto geomData = GetGeomData();
	if (geomData)
		geomData->SetBounds(bounds);
}

BoundingSphere NiShape::GetBounds() const {
	auto geomData = GetGeomData();
	if (geomData)
		return geomData->GetBounds();

	return BoundingSphere();
}

void NiShape::UpdateBounds() {
	auto geomData = GetGeomData();
	if (geomData)
		geomData->UpdateBounds();
}

int NiShape::GetBoneID(const NiHeader& hdr, const std::string& boneName) const {
	auto boneCont = hdr.GetBlock(SkinInstanceRef());
	if (boneCont) {
		int i = 0;
		for (auto& bone : boneCont->boneRefs) {
			auto node = hdr.GetBlock(bone);
			if (node && node->name == boneName)
				return i;
			++i;
		}
	}

	return NIF_NPOS;
}

bool NiShape::ReorderTriangles(const std::vector<uint32_t>& triInds) {
	std::vector<Triangle> trisOrdered;
	std::vector<Triangle> tris;
	if (!GetTriangles(tris))
		return false;

	if (tris.size() != triInds.size())
		return false;

	for (uint32_t id : triInds)
		if (id < tris.size())
			trisOrdered.push_back(tris[id]);

	if (trisOrdered.size() != tris.size())
		return false;

	SetTriangles(trisOrdered);
	return true;
}


BSTriShape::BSTriShape() {
	flags = 14;
	vertexDesc.SetFlag(VF_VERTEX);
	vertexDesc.SetFlag(VF_UV);
	vertexDesc.SetFlag(VF_NORMAL);
	vertexDesc.SetFlag(VF_TANGENT);
	vertexDesc.SetFlag(VF_SKINNED);
}

void BSTriShape::Sync(NiStreamReversible& stream) {
	stream.Sync(flags);
	stream.Sync(transform.translation);
	stream.Sync(transform.rotation);
	stream.Sync(transform.scale);

	collisionRef.Sync(stream);

	stream.Sync(bounds);

	if (stream.GetVersion().Stream() > 139)
		for (float& i : boundMinMax)
			stream.Sync(i);

	skinInstanceRef.Sync(stream);
	shaderPropertyRef.Sync(stream);
	alphaPropertyRef.Sync(stream);

	vertexDesc.Sync(stream);

	bool syncVertexData = true;

	if (stream.GetMode() == NiStreamReversible::Mode::Reading) {
		if (stream.GetVersion().User() >= 12 && stream.GetVersion().Stream() < 130) {
			uint16_t numTris = 0;
			stream.Sync(numTris);
			numTriangles = numTris;
		}
		else
			stream.Sync(numTriangles);
	}
	else {
		if (stream.GetVersion().User() >= 12 && stream.GetVersion().Stream() < 130) {
			if (IsSkinned()) {
				// Triangle and vertex data is in partition instead
				uint16_t numUShort = 0;
				uint32_t numUInt = 0;
				stream.Sync(numUShort);

				if (HasType<BSDynamicTriShape>())
					stream.Sync(numVertices);
				else
					stream.Sync(numUShort);

				stream.Sync(numUInt);
				syncVertexData = false;
			}
			else {
				auto numTris = static_cast<uint16_t>(numTriangles);
				stream.Sync(numTris);
			}
		}
		else
			stream.Sync(numTriangles);
	}

	if (syncVertexData) {
		stream.Sync(numVertices);
		stream.Sync(dataSize);

		vertData.resize(numVertices);

		if (dataSize > 0) {
			uint32_t vertexMainSize = vertexDesc.GetVertexMainSize();

			for (uint16_t i = 0; i < numVertices; i++) {
				auto& vertex = vertData[i];
				if (HasVertices() && vertexMainSize <= 16) {
					if (IsFullPrecision() || stream.GetVersion().Stream() == 100) {
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
				else if (vertexMainSize > 16) {
					// Full precision (vert = 12 bytes)
					stream.Sync((char*) &vertex.vert, sizeof(vertex.vert));

					// Variable length extra float elements
					uint32_t vertexExtraCount = (vertexMainSize - 16) / 4;
					if (vertexExtraCount > 0) {
						vertex.extra.resize(vertexExtraCount);
						for (uint32_t e = 0; e < vertexExtraCount; e++)
							stream.Sync(vertex.extra[e]);
					}

					// BitangentX after extra floats (bitangentX = 4 bytes)
					stream.Sync(vertex.bitangentX);
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

		triangles.resize(numTriangles);

		if (dataSize > 0) {
			for (uint32_t i = 0; i < numTriangles; i++)
				stream.Sync(triangles[i]);
		}
	}

	if (stream.GetVersion().User() == 12 && stream.GetVersion().Stream() == 100) {
		stream.Sync(particleDataSize);

		if (particleDataSize > 0) {
			particleVerts.resize(numVertices);
			particleNorms.resize(numVertices);
			particleTris.resize(numTriangles);

			for (uint16_t i = 0; i < numVertices; i++) {
				stream.SyncHalf(particleVerts[i].x);
				stream.SyncHalf(particleVerts[i].y);
				stream.SyncHalf(particleVerts[i].z);
			}

			for (uint16_t i = 0; i < numVertices; i++) {
				stream.SyncHalf(particleNorms[i].x);
				stream.SyncHalf(particleNorms[i].y);
				stream.SyncHalf(particleNorms[i].z);
			}

			for (uint32_t i = 0; i < numTriangles; i++)
				stream.Sync(particleTris[i]);
		}
	}
}

void BSTriShape::notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) {
	deletedTris.clear();

	std::vector<int> indexCollapse = GenerateIndexCollapseMap(vertIndices, vertData.size());

	EraseVectorIndices(vertData, vertIndices);
	numVertices = static_cast<uint16_t>(vertData.size());

	ApplyMapToTriangles(triangles, indexCollapse, &deletedTris);
	numTriangles = static_cast<uint32_t>(triangles.size());

	std::sort(deletedTris.begin(), deletedTris.end(), std::greater<>());
}

void BSTriShape::GetChildRefs(std::set<NiRef*>& refs) {
	NiAVObject::GetChildRefs(refs);

	refs.insert(&skinInstanceRef);
	refs.insert(&shaderPropertyRef);
	refs.insert(&alphaPropertyRef);
}

void BSTriShape::GetChildIndices(std::vector<uint32_t>& indices) {
	NiAVObject::GetChildIndices(indices);

	indices.push_back(skinInstanceRef.index);
	indices.push_back(shaderPropertyRef.index);
	indices.push_back(alphaPropertyRef.index);
}

std::vector<Vector3>& BSTriShape::UpdateRawVertices() {
	rawVertices.resize(numVertices);

	for (uint16_t i = 0; i < numVertices; i++)
		rawVertices[i] = vertData[i].vert;

	return rawVertices;
}

std::vector<Vector3>& BSTriShape::UpdateRawNormals() {
	if (!HasNormals()) {
		rawNormals.clear();
		return rawNormals;
	}

	rawNormals.resize(numVertices);

	for (uint16_t i = 0; i < numVertices; i++) {
		rawNormals[i].x = ((static_cast<float>(vertData[i].normal[0])) / 255.0f) * 2.0f - 1.0f;
		rawNormals[i].y = ((static_cast<float>(vertData[i].normal[1])) / 255.0f) * 2.0f - 1.0f;
		rawNormals[i].z = ((static_cast<float>(vertData[i].normal[2])) / 255.0f) * 2.0f - 1.0f;
	}

	return rawNormals;
}

std::vector<Vector3>& BSTriShape::UpdateRawTangents() {
	if (!HasTangents()) {
		rawTangents.clear();
		return rawTangents;
	}

	rawTangents.resize(numVertices);
	for (uint16_t i = 0; i < numVertices; i++) {
		rawTangents[i].x = ((static_cast<float>(vertData[i].tangent[0])) / 255.0f) * 2.0f - 1.0f;
		rawTangents[i].y = ((static_cast<float>(vertData[i].tangent[1])) / 255.0f) * 2.0f - 1.0f;
		rawTangents[i].z = ((static_cast<float>(vertData[i].tangent[2])) / 255.0f) * 2.0f - 1.0f;
	}

	return rawTangents;
}

std::vector<Vector3>& BSTriShape::UpdateRawBitangents() {
	if (!HasTangents()) {
		rawBitangents.clear();
		return rawBitangents;
	}

	rawBitangents.resize(numVertices);
	for (uint16_t i = 0; i < numVertices; i++) {
		rawBitangents[i].x = vertData[i].bitangentX;
		rawBitangents[i].y = ((static_cast<float>(vertData[i].bitangentY)) / 255.0f) * 2.0f - 1.0f;
		rawBitangents[i].z = ((static_cast<float>(vertData[i].bitangentZ)) / 255.0f) * 2.0f - 1.0f;
	}

	return rawBitangents;
}

std::vector<Vector2>& BSTriShape::UpdateRawUvs() {
	if (!HasUVs()) {
		rawUvs.clear();
		return rawUvs;
	}

	rawUvs.resize(numVertices);

	for (uint16_t i = 0; i < numVertices; i++)
		rawUvs[i] = vertData[i].uv;

	return rawUvs;
}

std::vector<Color4>& BSTriShape::UpdateRawColors() {
	if (!HasVertexColors()) {
		rawColors.clear();
		return rawColors;
	}

	rawColors.resize(numVertices);

	for (uint16_t i = 0; i < numVertices; i++) {
		rawColors[i].r = vertData[i].colorData[0] / 255.0f;
		rawColors[i].g = vertData[i].colorData[1] / 255.0f;
		rawColors[i].b = vertData[i].colorData[2] / 255.0f;
		rawColors[i].a = vertData[i].colorData[3] / 255.0f;
	}

	return rawColors;
}

std::vector<float>& BSTriShape::UpdateRawEyeData() {
	if (!HasEyeData()) {
		rawEyeData.clear();
		return rawEyeData;
	}

	rawEyeData.resize(numVertices);

	for (uint16_t i = 0; i < numVertices; ++i)
		rawEyeData[i] = vertData[i].eyeData;

	return rawEyeData;
}

uint16_t BSTriShape::GetNumVertices() const {
	return numVertices;
}

void BSTriShape::SetVertices(const bool enable) {
	if (enable) {
		vertexDesc.SetFlag(VF_VERTEX);
		vertData.resize(numVertices);
	}
	else {
		vertexDesc.RemoveFlag(VF_VERTEX);
		vertData.clear();
		numVertices = 0;

		SetUVs(false);
		SetNormals(false);
		SetTangents(false);
		SetVertexColors(false);
		SetSkinned(false);
	}
}

void BSTriShape::SetUVs(const bool enable) {
	if (enable)
		vertexDesc.SetFlag(VF_UV);
	else
		vertexDesc.RemoveFlag(VF_UV);
}

void BSTriShape::SetSecondUVs(const bool enable) {
	if (enable)
		vertexDesc.SetFlag(VF_UV_2);
	else
		vertexDesc.RemoveFlag(VF_UV_2);
}

void BSTriShape::SetNormals(const bool enable) {
	if (enable)
		vertexDesc.SetFlag(VF_NORMAL);
	else
		vertexDesc.RemoveFlag(VF_NORMAL);
}

void BSTriShape::SetTangents(const bool enable) {
	if (enable)
		vertexDesc.SetFlag(VF_TANGENT);
	else
		vertexDesc.RemoveFlag(VF_TANGENT);
}

void BSTriShape::SetVertexColors(const bool enable) {
	if (enable) {
		if (!vertexDesc.HasFlag(VF_COLORS)) {
			for (auto& v : vertData) {
				v.colorData[0] = 255;
				v.colorData[1] = 255;
				v.colorData[2] = 255;
				v.colorData[3] = 255;
			}
		}

		vertexDesc.SetFlag(VF_COLORS);
	}
	else
		vertexDesc.RemoveFlag(VF_COLORS);
}

void BSTriShape::SetSkinned(const bool enable) {
	if (enable)
		vertexDesc.SetFlag(VF_SKINNED);
	else
		vertexDesc.RemoveFlag(VF_SKINNED);
}

void BSTriShape::SetEyeData(const bool enable) {
	if (enable)
		vertexDesc.SetFlag(VF_EYEDATA);
	else
		vertexDesc.RemoveFlag(VF_EYEDATA);
}

void BSTriShape::SetFullPrecision(const bool enable) {
	if (!CanChangePrecision())
		return;

	if (enable)
		vertexDesc.SetFlag(VF_FULLPREC);
	else
		vertexDesc.RemoveFlag(VF_FULLPREC);
}

uint32_t BSTriShape::GetNumTriangles() const {
	return numTriangles;
}

bool BSTriShape::GetTriangles(std::vector<Triangle>& tris) const {
	tris = triangles;
	return true;
}

void BSTriShape::SetTriangles(const std::vector<Triangle>& tris) {
	triangles = tris;
	numTriangles = static_cast<uint32_t>(triangles.size());
}

void BSTriShape::UpdateBounds() {
	UpdateRawVertices();
	bounds = BoundingSphere(rawVertices);
}

void BSTriShape::SetVertexData(const std::vector<BSVertexData>& bsVertData) {
	vertData = bsVertData;
	numVertices = static_cast<uint16_t>(vertData.size());
}

void BSTriShape::SetNormals(const std::vector<Vector3>& inNorms) {
	SetNormals(true);

	rawNormals.resize(numVertices);
	for (uint16_t i = 0; i < numVertices; i++) {
		rawNormals[i] = inNorms[i];
		vertData[i].normal[0] = static_cast<uint8_t>(std::round((((inNorms[i].x + 1.0f) / 2.0f) * 255.0f)));
		vertData[i].normal[1] = static_cast<uint8_t>(std::round((((inNorms[i].y + 1.0f) / 2.0f) * 255.0f)));
		vertData[i].normal[2] = static_cast<uint8_t>(std::round((((inNorms[i].z + 1.0f) / 2.0f) * 255.0f)));
	}
}

void BSTriShape::SetTangentData(const std::vector<Vector3>& in) {
	SetTangents(true);

	for (uint16_t i = 0; i < numVertices; i++) {
		vertData[i].tangent[0] = static_cast<uint8_t>(std::round((((in[i].x + 1.0f) / 2.0f) * 255.0f)));
		vertData[i].tangent[1] = static_cast<uint8_t>(std::round((((in[i].y + 1.0f) / 2.0f) * 255.0f)));
		vertData[i].tangent[2] = static_cast<uint8_t>(std::round((((in[i].z + 1.0f) / 2.0f) * 255.0f)));
	}
}

void BSTriShape::SetBitangentData(const std::vector<Vector3>& in) {
	SetTangents(true);

	for (uint16_t i = 0; i < numVertices; i++) {
		vertData[i].bitangentX = in[i].x;
		vertData[i].bitangentY = static_cast<uint8_t>(std::round((((in[i].y + 1.0f) / 2.0f) * 255.0f)));
		vertData[i].bitangentZ = static_cast<uint8_t>(std::round((((in[i].z + 1.0f) / 2.0f) * 255.0f)));
	}
}

void BSTriShape::SetEyeData(const std::vector<float>& in) {
	SetEyeData(true);

	for (uint16_t i = 0; i < numVertices; i++)
		vertData[i].eyeData = in[i];
}

static void CalculateNormals(const std::vector<Vector3>& verts,
							 const std::vector<Triangle>& tris,
							 std::vector<Vector3>& outNorms,
							 const bool smooth,
							 float smoothThresh,
							 std::unordered_set<uint32_t>* lockedIndices = nullptr) {
	std::vector<Vector3> norms;
	norms.resize(verts.size());

	// Face normals
	for (const Triangle& t : tris) {
		Vector3 tn = t.trinormal(verts);
		norms[t.p1] += tn;
		norms[t.p2] += tn;
		norms[t.p3] += tn;
	}

	for (Vector3& n : norms)
		n.Normalize();

	// Smooth normals
	if (smooth) {
		smoothThresh *= DEG2RAD;
		std::vector<Vector3> seamNorms;
		SortingMatcher matcher(verts.data(), static_cast<uint16_t>(verts.size()));
		for (const auto& matchset : matcher.matches) {
			seamNorms.resize(matchset.size());
			for (size_t j = 0; j < matchset.size(); ++j) {
				const Vector3& n = norms[matchset[j]];
				Vector3 sn = n;
				for (size_t k = 0; k < matchset.size(); ++k) {
					if (j == k)
						continue;
					const Vector3& mn = norms[matchset[k]];
					if (n.angle(mn) >= smoothThresh)
						continue;
					sn += mn;
				}
				sn.Normalize();
				seamNorms[j] = sn;
			}
			for (size_t j = 0; j < matchset.size(); ++j)
				norms[matchset[j]] = seamNorms[j];
		}
	}

	if (lockedIndices) {
		outNorms.resize(norms.size());

		// Move normals of indices that aren't locked only
		for (uint32_t i = 0; i < static_cast<uint32_t>(norms.size()); i++) {
			if (lockedIndices->find(i) == lockedIndices->end())
				outNorms[i] = std::move(norms[i]);
		}
	}
	else
		outNorms = std::move(norms);
}

void BSTriShape::RecalcNormals(const bool smooth,
							   const float smoothThresh,
							   std::unordered_set<uint32_t>* lockedIndices) {
	UpdateRawVertices();
	SetNormals(true);

	CalculateNormals(rawVertices, triangles, rawNormals, smooth, smoothThresh, lockedIndices);

	for (uint16_t i = 0; i < numVertices; i++) {
		if (lockedIndices) {
			// Skip locked indices (keep current normal)
			if (lockedIndices->find(i) != lockedIndices->end())
				continue;
		}

		vertData[i].normal[0] = static_cast<uint8_t>(std::round((((rawNormals[i].x + 1.0f) / 2.0f) * 255.0f)));
		vertData[i].normal[1] = static_cast<uint8_t>(std::round((((rawNormals[i].y + 1.0f) / 2.0f) * 255.0f)));
		vertData[i].normal[2] = static_cast<uint8_t>(std::round((((rawNormals[i].z + 1.0f) / 2.0f) * 255.0f)));
	}
}

void BSTriShape::CalcTangentSpace() {
	if (!HasNormals() || !HasUVs())
		return;

	UpdateRawNormals();
	SetTangents(true);

	std::vector<Vector3> tan1;
	std::vector<Vector3> tan2;
	tan1.resize(numVertices);
	tan2.resize(numVertices);

	for (auto& triangle : triangles) {
		int i1 = triangle.p1;
		int i2 = triangle.p2;
		int i3 = triangle.p3;

		if (i1 >= numVertices || i2 >= numVertices || i3 >= numVertices)
			continue;

		Vector3 v1 = vertData[i1].vert;
		Vector3 v2 = vertData[i2].vert;
		Vector3 v3 = vertData[i3].vert;

		Vector2 w1 = vertData[i1].uv;
		Vector2 w2 = vertData[i2].uv;
		Vector2 w3 = vertData[i3].uv;

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.u - w1.u;
		float s2 = w3.u - w1.u;
		float t1 = w2.v - w1.v;
		float t2 = w3.v - w1.v;

		float r = (s1 * t2 - s2 * t1);
		r = (r >= 0.0f ? +1.0f : -1.0f);

		Vector3 sdir = Vector3((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
		Vector3 tdir = Vector3((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

		sdir.Normalize();
		tdir.Normalize();

		tan1[i1] += tdir;
		tan1[i2] += tdir;
		tan1[i3] += tdir;

		tan2[i1] += sdir;
		tan2[i2] += sdir;
		tan2[i3] += sdir;
	}

	rawBitangents.resize(numVertices);
	rawTangents.resize(numVertices);

	for (uint16_t i = 0; i < numVertices; i++) {
		rawTangents[i] = tan1[i];
		rawBitangents[i] = tan2[i];

		if (rawTangents[i].IsZero() || rawBitangents[i].IsZero()) {
			rawTangents[i].x = rawNormals[i].y;
			rawTangents[i].y = rawNormals[i].z;
			rawTangents[i].z = rawNormals[i].x;
			rawBitangents[i] = rawNormals[i].cross(rawTangents[i]);
		}
		else {
			rawTangents[i].Normalize();
			rawTangents[i] = (rawTangents[i] - rawNormals[i] * rawNormals[i].dot(rawTangents[i]));
			rawTangents[i].Normalize();

			rawBitangents[i].Normalize();

			rawBitangents[i] = (rawBitangents[i] - rawNormals[i] * rawNormals[i].dot(rawBitangents[i]));
			rawBitangents[i] = (rawBitangents[i] - rawTangents[i] * rawTangents[i].dot(rawBitangents[i]));

			rawBitangents[i].Normalize();
		}

		vertData[i].tangent[0] = static_cast<uint8_t>(
			std::round((((rawTangents[i].x + 1.0f) / 2.0f) * 255.0f)));
		vertData[i].tangent[1] = static_cast<uint8_t>(
			std::round((((rawTangents[i].y + 1.0f) / 2.0f) * 255.0f)));
		vertData[i].tangent[2] = static_cast<uint8_t>(
			std::round((((rawTangents[i].z + 1.0f) / 2.0f) * 255.0f)));

		vertData[i].bitangentX = rawBitangents[i].x;
		vertData[i].bitangentY = static_cast<uint8_t>(
			std::round((((rawBitangents[i].y + 1.0f) / 2.0f) * 255.0f)));
		vertData[i].bitangentZ = static_cast<uint8_t>(
			std::round((((rawBitangents[i].z + 1.0f) / 2.0f) * 255.0f)));
	}
}

int BSTriShape::CalcDataSizes(NiVersion& version) {
	vertexSize = 0;
	dataSize = 0;

	VertexFlags vf = vertexDesc.GetFlags();
	vertexDesc.ClearAttributeOffsets();

	std::array<uint32_t, VA_COUNT> attributeSizes{};
	if (HasVertices()) {
		if (IsFullPrecision() || version.Stream() == 100)
			attributeSizes[VA_POSITION] = 4;
		else
			attributeSizes[VA_POSITION] = 2;
	}

	if (!vertData.empty() && !vertData.front().extra.empty()) {
		// Add extra float elements to vertex size
		uint8_t extraCount = static_cast<uint8_t>(vertData.front().extra.size());
		if (extraCount > 0)
			attributeSizes[VA_POSITION] += extraCount;
	}

	if (HasUVs())
		attributeSizes[VA_TEXCOORD0] = 1;

	if (HasSecondUVs())
		attributeSizes[VA_TEXCOORD1] = 1;

	if (HasNormals()) {
		attributeSizes[VA_NORMAL] = 1;

		if (HasTangents())
			attributeSizes[VA_BINORMAL] = 1;
	}

	if (HasVertexColors())
		attributeSizes[VA_COLOR] = 1;

	if (IsSkinned())
		attributeSizes[VA_SKINNING] = 3;

	if (HasEyeData())
		attributeSizes[VA_EYEDATA] = 1;

	for (int va = 0; va < VA_COUNT; va++) {
		if (attributeSizes[va] != 0) {
			vertexDesc.SetAttributeOffset(VertexAttribute(va), vertexSize);
			vertexSize += attributeSizes[va] * 4;
		}
	}

	vertexDesc.SetSize(vertexSize);
	vertexDesc.SetFlags(vf);

	if (HasType<BSDynamicTriShape>())
		vertexDesc.MakeDynamic();

	dataSize = vertexSize * numVertices + 6 * numTriangles;

	return dataSize;
}

void BSTriShape::Create(NiVersion& version,
						const std::vector<Vector3>* verts,
						const std::vector<Triangle>* tris,
						const std::vector<Vector2>* uvs,
						const std::vector<Vector3>* normals) {
	constexpr uint16_t maxVertIndex = std::numeric_limits<uint16_t>::max();
	size_t vertCount = verts->size();
	if (vertCount > static_cast<size_t>(maxVertIndex))
		numVertices = maxVertIndex;
	else
		numVertices = uint16_t(vertCount);

	uint32_t maxTriIndex = std::numeric_limits<uint32_t>::max();
	if (version.User() >= 12 && version.Stream() < 130)
		maxTriIndex = std::numeric_limits<uint16_t>::max();

	size_t triCount = tris ? tris->size() : 0;
	if (numVertices == 0)
		numTriangles = 0;
	else if (triCount > static_cast<size_t>(maxTriIndex))
		numTriangles = maxTriIndex;
	else
		numTriangles = uint32_t(triCount);

	vertData.resize(numVertices);

	if (uvs && uvs->size() != numVertices)
		SetUVs(false);

	for (uint16_t i = 0; i < numVertices; i++) {
		auto& vertex = vertData[i];
		vertex.vert = (*verts)[i];

		if (uvs && uvs->size() == numVertices)
			vertex.uv = (*uvs)[i];

		vertex.bitangentX = 0.0f;
		vertex.bitangentY = 0;
		vertex.bitangentZ = 0;
		vertex.normal[0] = vertex.normal[1] = vertex.normal[2] = 0;
		std::memset(vertex.colorData, 255, 4);
		std::memset(vertex.weights, 0, sizeof(float) * 4);
		std::memset(vertex.weightBones, 0, 4);
		vertex.eyeData = 0.0f;
	}

	triangles.resize(numTriangles);
	for (uint32_t i = 0; i < numTriangles; i++)
		triangles[i] = (*tris)[i];

	UpdateRawVertices();
	bounds = BoundingSphere(rawVertices);

	if (normals && normals->size() == numVertices) {
		SetNormals(*normals);
		CalcTangentSpace();
	}
	else {
		SetNormals(false);
		SetTangents(false);
	}
}


void BSSubIndexTriShape::Sync(NiStreamReversible& stream) {
	if (stream.GetVersion().Stream() >= 130 && dataSize > 0) {
		stream.Sync(segmentation.numPrimitives);
		stream.Sync(segmentation.numSegments);
		stream.Sync(segmentation.numTotalSegments);

		segmentation.segments.resize(segmentation.numSegments);
		for (auto& segment : segmentation.segments) {
			stream.Sync(segment.startIndex);
			stream.Sync(segment.numPrimitives);
			stream.Sync(segment.parentArrayIndex);
			stream.Sync(segment.numSubSegments);

			segment.subSegments.resize(segment.numSubSegments);
			for (auto& subSegment : segment.subSegments) {
				stream.Sync(subSegment.startIndex);
				stream.Sync(subSegment.numPrimitives);
				stream.Sync(subSegment.arrayIndex);
				stream.Sync(subSegment.unkInt1);
			}
		}

		if (segmentation.numSegments < segmentation.numTotalSegments) {
			stream.Sync(segmentation.subSegmentData.numSegments);
			stream.Sync(segmentation.subSegmentData.numTotalSegments);

			segmentation.subSegmentData.arrayIndices.resize(segmentation.numSegments);
			for (auto& arrayIndex : segmentation.subSegmentData.arrayIndices)
				stream.Sync(arrayIndex);

			segmentation.subSegmentData.dataRecords.resize(segmentation.numTotalSegments);
			for (auto& dataRecord : segmentation.subSegmentData.dataRecords) {
				stream.Sync(dataRecord.userSlotID);
				stream.Sync(dataRecord.material);
				stream.Sync(dataRecord.numData);

				dataRecord.extraData.resize(dataRecord.numData);
				for (auto& data : dataRecord.extraData)
					stream.Sync(data);
			}

			segmentation.subSegmentData.ssfFile.Sync(stream, 2);
		}
	}
	else if (stream.GetVersion().Stream() == 100) {
		stream.Sync(numSegments);
		segments.resize(numSegments);

		for (auto& segment : segments)
			segment.Sync(stream);
	}
}

void BSSubIndexTriShape::notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) {
	BSTriShape::notifyVerticesDelete(vertIndices);

	//Remove triangles from segments and re-fit lists
	segmentation.numPrimitives -= static_cast<uint32_t>(deletedTris.size());
	for (auto& segment : segmentation.segments) {
		// Delete primitives
		for (auto& id : deletedTris)
			if (segment.numPrimitives > 0 && id >= segment.startIndex / 3
				&& id < segment.startIndex / 3 + segment.numPrimitives)
				segment.numPrimitives--;

		// Align sub segments
		for (auto& subSegment : segment.subSegments)
			for (auto& id : deletedTris)
				if (subSegment.numPrimitives > 0 && id >= subSegment.startIndex / 3
					&& id < subSegment.startIndex / 3 + subSegment.numPrimitives)
					subSegment.numPrimitives--;
	}

	// Align segments
	size_t i = 0;
	for (auto& segment : segmentation.segments) {
		// Align sub segments
		size_t j = 0;
		for (auto& subSegment : segment.subSegments) {
			if (j == 0)
				subSegment.startIndex = segment.startIndex;

			if (j + 1 >= segment.numSubSegments)
				continue;

			BSSITSSubSegment& nextSubSegment = segment.subSegments[j + 1];
			nextSubSegment.startIndex = subSegment.startIndex + subSegment.numPrimitives * 3;
			j++;
		}

		if (i + 1 >= segmentation.numSegments)
			continue;

		BSSITSSegment& nextSegment = segmentation.segments[i + 1];
		nextSegment.startIndex = segment.startIndex + segment.numPrimitives * 3;

		i++;
	}

	// Remove triangles from SSE segments
	for (auto& segment : segments) {
		for (auto& id : deletedTris)
			if (segment.numTris > 0 && id >= segment.index / 3 && id < segment.index / 3 + segment.numTris)
				segment.numTris--;
	}

	// Align SSE segments
	i = 0;
	for (auto& segment : segments) {
		if (i + 1 >= numSegments)
			continue;

		BSGeometrySegmentData& nextSegment = segments[i + 1];
		nextSegment.index = segment.index + segment.numTris * 3;

		i++;
	}
}

void BSSubIndexTriShape::SetDefaultSegments() {
	segmentation.numPrimitives = numTriangles;
	segmentation.numSegments = 4;
	segmentation.numTotalSegments = 4;

	segmentation.subSegmentData.numSegments = 0;
	segmentation.subSegmentData.numTotalSegments = 0;

	segmentation.subSegmentData.arrayIndices.clear();
	segmentation.subSegmentData.dataRecords.clear();
	segmentation.subSegmentData.ssfFile.clear();

	segmentation.segments.resize(4);
	for (uint32_t i = 0; i < 3; i++) {
		segmentation.segments[i].startIndex = 0;
		segmentation.segments[i].numPrimitives = 0;
		segmentation.segments[i].parentArrayIndex = 0xFFFFFFFF;
		segmentation.segments[i].numSubSegments = 0;
	}

	segmentation.segments[3].startIndex = 0;
	segmentation.segments[3].numPrimitives = numTriangles;
	segmentation.segments[3].parentArrayIndex = 0xFFFFFFFF;
	segmentation.segments[3].numSubSegments = 0;

	numSegments = 0;
	segments.clear();
}

void BSSubIndexTriShape::Create(NiVersion& version,
								const std::vector<Vector3>* verts,
								const std::vector<Triangle>* tris,
								const std::vector<Vector2>* uvs,
								const std::vector<Vector3>* normals) {
	BSTriShape::Create(version, verts, tris, uvs, normals);

	// Skinned most of the time
	SetSkinned(true);
	SetDefaultSegments();
}

std::vector<BSGeometrySegmentData> BSSubIndexTriShape::GetSegments() const {
	return segments;
}

void BSSubIndexTriShape::SetSegments(const std::vector<BSGeometrySegmentData>& sd) {
	segments = sd;
	numSegments = static_cast<uint32_t>(segments.size());
}

void BSSubIndexTriShape::GetSegmentation(NifSegmentationInfo& inf, std::vector<int>& triParts) const {
	inf.segs.clear();
	inf.ssfFile = segmentation.subSegmentData.ssfFile.get();
	inf.segs.resize(segmentation.segments.size());
	triParts.clear();

	uint32_t numTris = GetNumTriangles();
	triParts.resize(numTris, -1);

	int partID = 0;
	int arrayIndex = 0;

	for (size_t i = 0; i < segmentation.segments.size(); ++i) {
		const BSSITSSegment& seg = segmentation.segments[i];
		uint32_t startIndex = seg.startIndex / 3;
		uint32_t endIndex = std::min(numTris, startIndex + seg.numPrimitives);

		for (uint32_t id = startIndex; id < endIndex; id++)
			triParts[id] = partID;

		inf.segs[i].partID = partID++;
		inf.segs[i].subs.resize(seg.subSegments.size());

		for (size_t j = 0; j < seg.subSegments.size(); ++j) {
			const BSSITSSubSegment& sub = seg.subSegments[j];
			startIndex = sub.startIndex / 3;

			endIndex = std::min(numTris, startIndex + sub.numPrimitives);
			for (uint32_t id = startIndex; id < endIndex; id++)
				triParts[id] = partID;

			inf.segs[i].subs[j].partID = partID++;
			arrayIndex++;

			const BSSITSSubSegmentDataRecord& rec = segmentation.subSegmentData.dataRecords[arrayIndex];
			inf.segs[i].subs[j].userSlotID = rec.userSlotID < 30 ? 0 : rec.userSlotID;
			inf.segs[i].subs[j].material = rec.material;
			inf.segs[i].subs[j].extraData = rec.extraData;
		}
		arrayIndex++;
	}
}

void BSSubIndexTriShape::SetSegmentation(const NifSegmentationInfo& inf, const std::vector<int>& inTriParts) {
	uint32_t numTris = GetNumTriangles();
	if (inTriParts.size() != numTris)
		return;

	// Renumber partitions so that the partition IDs are increasing.
	int newPartID = 0;
	std::vector<int> oldToNewPartIDs;
	for (const NifSegmentInfo& seg : inf.segs) {
		if (seg.partID >= static_cast<int>(oldToNewPartIDs.size()))
			oldToNewPartIDs.resize(seg.partID + 1);
		oldToNewPartIDs[seg.partID] = newPartID++;

		for (const NifSubSegmentInfo& sub : seg.subs) {
			if (sub.partID >= static_cast<int>(oldToNewPartIDs.size()))
				oldToNewPartIDs.resize(sub.partID + 1);
			oldToNewPartIDs[sub.partID] = newPartID++;
		}
	}

	std::vector<int> triParts(numTris);
	for (uint32_t i = 0; i < numTris; ++i)
		if (inTriParts[i] >= 0)
			triParts[i] = oldToNewPartIDs[inTriParts[i]];

	// Sort triangles (via index) by partition ID
	std::vector<uint32_t> triInds(numTris);
	for (uint32_t i = 0; i < numTris; ++i)
		triInds[i] = i;

	std::stable_sort(triInds.begin(), triInds.end(), [&triParts](int i, int j) {
		return triParts[i] < triParts[j];
	});

	ReorderTriangles(triInds);
	// Note that triPart's indexing no longer matches triangle indexing.
	// triParts uses the old indexing.  triInds maps from new indexing to old.
	// So triParts[triInds[i]] is now the partition number of triangle i.

	// Find the index of the first triangle of each partition: partTriInds.
	// If p is the partition number, then partTriInds[p] will be the index
	// in tris of the first triangle of partition p.
	// The number of triangles in partition p will be
	// partTriInds[p + 1] - partTriInds[p].
	std::vector<uint32_t> partTriInds(newPartID + 1);
	int nextPartID = 0;
	for (uint32_t i = 0; i < numTris; ++i)
		while (triParts[triInds[i]] >= nextPartID)
			partTriInds[nextPartID++] = i;
	while (nextPartID < static_cast<int>(partTriInds.size()))
		partTriInds[nextPartID++] = numTris;

	segmentation = BSSITSSegmentation();
	uint32_t parentArrayIndex = 0;
	uint32_t segmentIndex = 0;
	int partID = 0;

	for (const NifSegmentInfo& seg : inf.segs) {
		// Create new segment
		segmentation.segments.emplace_back();
		BSSITSSegment& segment = segmentation.segments.back();
		uint32_t childCount = static_cast<uint32_t>(seg.subs.size());
		segment.numPrimitives = partTriInds[partID + childCount + 1] - partTriInds[partID];
		segment.startIndex = partTriInds[partID] * 3;
		segment.numSubSegments = childCount;
		++partID;

		// Create new segment data record
		BSSITSSubSegmentDataRecord segmentDataRecord;
		segmentDataRecord.userSlotID = segmentIndex;
		segmentation.subSegmentData.arrayIndices.push_back(parentArrayIndex);
		segmentation.subSegmentData.dataRecords.push_back(segmentDataRecord);

		uint32_t subSegmentNumber = 1;
		for (const NifSubSegmentInfo& sub : seg.subs) {
			// Create new subsegment
			segment.subSegments.emplace_back();
			BSSITSSubSegment& subSegment = segment.subSegments.back();
			subSegment.arrayIndex = parentArrayIndex;
			subSegment.numPrimitives = partTriInds[partID + 1] - partTriInds[partID];
			subSegment.startIndex = partTriInds[partID] * 3;
			++partID;

			// Create new subsegment data record
			BSSITSSubSegmentDataRecord subSegmentDataRecord;
			if (sub.userSlotID < 30)
				subSegmentDataRecord.userSlotID = subSegmentNumber++;
			else
				subSegmentDataRecord.userSlotID = sub.userSlotID;

			subSegmentDataRecord.material = sub.material;
			subSegmentDataRecord.numData = static_cast<uint32_t>(sub.extraData.size());
			subSegmentDataRecord.extraData = sub.extraData;
			segmentation.subSegmentData.dataRecords.push_back(subSegmentDataRecord);
		}

		parentArrayIndex += childCount + 1;
		++segmentIndex;
	}

	segmentation.numPrimitives = numTris;
	segmentation.numSegments = segmentIndex;
	segmentation.numTotalSegments = parentArrayIndex;
	segmentation.subSegmentData.numSegments = segmentIndex;
	segmentation.subSegmentData.numTotalSegments = parentArrayIndex;
	segmentation.subSegmentData.ssfFile.get() = inf.ssfFile;
}


void BSMeshLODTriShape::Sync(NiStreamReversible& stream) {
	stream.Sync(lodSize0);
	stream.Sync(lodSize1);
	stream.Sync(lodSize2);
}

void BSMeshLODTriShape::notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) {
	BSTriShape::notifyVerticesDelete(vertIndices);

	// Force full LOD (workaround)
	lodSize0 = 0;
	lodSize1 = 0;
	lodSize2 = numTriangles;
}


BSDynamicTriShape::BSDynamicTriShape() {
	vertexDesc.RemoveFlag(VF_VERTEX);
	vertexDesc.SetFlag(VF_FULLPREC);

	dynamicDataSize = 0;
}

void BSDynamicTriShape::Sync(NiStreamReversible& stream) {
	stream.Sync(dynamicDataSize);

	dynamicData.resize(numVertices);
	for (uint16_t i = 0; i < numVertices; i++)
		stream.Sync(dynamicData[i]);
}

void BSDynamicTriShape::notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) {
	BSTriShape::notifyVerticesDelete(vertIndices);

	EraseVectorIndices(dynamicData, vertIndices);
	dynamicDataSize = static_cast<uint32_t>(dynamicData.size());
}

void BSDynamicTriShape::CalcDynamicData() {
	dynamicDataSize = numVertices * 16;

	dynamicData.resize(numVertices);
	for (uint16_t i = 0; i < numVertices; i++) {
		auto& vertex = vertData[i];
		dynamicData[i].x = vertex.vert.x;
		dynamicData[i].y = vertex.vert.y;
		dynamicData[i].z = vertex.vert.z;
		dynamicData[i].w = vertex.bitangentX;

		if (dynamicData[i].x > 0.0f)
			vertex.eyeData = 1.0f;
		else
			vertex.eyeData = 0.0f;
	}
}

void BSDynamicTriShape::Create(NiVersion& version,
							   const std::vector<Vector3>* verts,
							   const std::vector<Triangle>* tris,
							   const std::vector<Vector2>* uvs,
							   const std::vector<Vector3>* normals) {
	BSTriShape::Create(version, verts, tris, uvs, normals);

	constexpr uint32_t maxIndex = std::numeric_limits<uint32_t>::max();
	size_t vertCount = verts->size();
	if (vertCount > static_cast<size_t>(maxIndex))
		dynamicDataSize = maxIndex;
	else
		dynamicDataSize = uint32_t(vertCount);

	dynamicData.resize(dynamicDataSize);
	for (uint32_t i = 0; i < dynamicDataSize; i++) {
		dynamicData[i].x = (*verts)[i].x;
		dynamicData[i].y = (*verts)[i].y;
		dynamicData[i].z = (*verts)[i].z;
		dynamicData[i].w = 0.0f;
	}
}

void BSGeometryMeshData::Sync(NiStreamReversible& stream) {
	// verts, normals, vertcolors are always present, though it's possible the counts are 0
	SetVertices(true);
	SetNormals(true);
	SetTangents(true);
	SetVertexColors(true);

	stream.Sync(version);
	if (version > 2)
		return;

	stream.Sync(nTriIndices);
	tris.resize(nTriIndices / 3);
	for (uint32_t t = 0; t < nTriIndices / 3; t++)
		stream.Sync(tris[t]);

	stream.Sync(scale);
	if (scale <= 0.0f)
		return;

	stream.Sync(nWeightsPerVert);

	stream.Sync(nVertices);
	// maybe not a good idea to do the below, in case some meshes have over 65k verts, however since
	// triangles still use 16 bit indices, the total count must still fit under that limit ...
	numVertices = (uint16_t) nVertices;
	vertices.resize(nVertices);
	for (uint32_t v = 0; v < nVertices; v++) {
		if (stream.GetMode() == NiStreamReversible::Mode::Reading) {
			auto unpack = [&](const float posScale) -> float {
				int16_t val;
				stream.Sync(val);
				if (val < 0)
					return static_cast<float>((val / 32768.0) * scale * posScale);
				else
					return static_cast<float>((val / 32767.0) * scale * posScale);
			};

			vertices[v].x = unpack(havokScale);
			vertices[v].y = unpack(havokScale);
			vertices[v].z = unpack(havokScale);
		}
		else {
			auto pack = [&](float component, float posScale) {
				uint16_t factor;
				if (component < 0)
					factor = 32768;
				else
					factor = 32767;

				uint16_t val = (uint16_t) ((component / (scale * posScale)) * factor);
				stream.Sync(val);
			};

			pack(vertices[v].x, havokScale);
			pack(vertices[v].y, havokScale);
			pack(vertices[v].z, havokScale);
		}
	}

	stream.Sync(nUV1);
	if (nUV1 > 0)
		SetUVs(true);

	uvSets.resize(2);

	uvSets[0].resize(nUV1);
	for (uint32_t uv = 0; uv < nUV1; uv++) {
		stream.SyncHalf(uvSets[0][uv].u);
		stream.SyncHalf(uvSets[0][uv].v);
	}

	stream.Sync(nUV2);
	uvSets[1].resize(nUV2);
	for (uint32_t uv = 0; uv < nUV2; uv++) {
		stream.SyncHalf(uvSets[1][uv].u);
		stream.SyncHalf(uvSets[1][uv].v);
	}

	stream.Sync(nColors);
	vColors.resize(nColors);
	for (uint32_t c = 0; c < nColors; c++)
		stream.Sync(vColors[c]);

	stream.Sync(nNormals);
	normals.resize(nNormals);
	for (uint32_t n = 0; n < nNormals; n++)
		stream.SyncUDEC3(normals[n]);

	stream.Sync(nTangents);
	tangents.resize(nTangents);
	for (uint32_t t = 0; t < nTangents; t++) {
		stream.SyncUDEC3(tangents[t]);
		// need to calculate tangent basis and bitangents on read?
	}

	/*
	FIXME: Normal and tangent data is in a 10:10:10:2 bits packed X,Y,Z,W format.
	Each channel should be normalized from unsigned integers to -1.0 to 1.0.
	The bitangent can be calculated with this formula:
	bitangent.xyz = normal.xyz x tangent.xyz * tangent.w (the 'x' means cross product).
	*/

	stream.Sync(nTotalWeights);
	if (nWeightsPerVert > 0)
		skinWeights.resize(nTotalWeights / nWeightsPerVert);

	for (auto& vw : skinWeights) {
		vw.resize(nWeightsPerVert);
		for (auto& bw : vw)
			stream.Sync(bw);
	}

	stream.Sync(nLODS);
	lods.resize(nLODS);
	for (auto& lod : lods) {
		uint32_t nLodTriIndices = static_cast<uint32_t>(lod.size() * 3);
		stream.Sync(nLodTriIndices);

		lod.resize(nLodTriIndices / 3);
		for (auto& lodTri : lod)
			stream.Sync(lodTri);
	}

	stream.Sync(nMeshlets);
	meshletList.resize(nMeshlets);
	for (auto& meshlet : meshletList) {
		stream.Sync(meshlet.vertCount);
		stream.Sync(meshlet.vertOffset);
		stream.Sync(meshlet.primCount);
		stream.Sync(meshlet.primOffset);
	}

	stream.Sync(nCullData);
	cullDataList.resize(nCullData);
	for (auto& cullData : cullDataList) {
		stream.Sync(cullData.center);
		stream.Sync(cullData.expand);
	}
}

void BSGeometryMesh::Sync(NiStreamReversible& stream) {
	stream.Sync(triSize);
	stream.Sync(numVerts);
	stream.Sync(flags);
	meshName.Sync(stream, 4);
}

void BSGeometry::Sync(NiStreamReversible& stream) {
	stream.Sync(bounds);

	for (float& i : boundMinMax)
		stream.Sync(i);

	skinInstanceRef.Sync(stream);
	shaderPropertyRef.Sync(stream);
	alphaPropertyRef.Sync(stream);

	if (stream.GetMode() == NiStreamReversible::Mode::Reading)
		meshes.clear();

	size_t meshCount = meshes.size();
	for (uint32_t i = 0; i < 4; i++) {
		uint8_t testByte = i < meshCount;
		stream.Sync(testByte);
		if (testByte) {
			if (stream.GetMode() == NiStreamReversible::Mode::Reading) {
				BSGeometryMesh mesh{};
				meshes.push_back(mesh);
			}
			meshes[i].Sync(stream);
		}
	}
}

void BSGeometry::GetChildRefs(std::set<NiRef*>& refs) {
	NiAVObject::GetChildRefs(refs);

	refs.insert(&skinInstanceRef);
	refs.insert(&shaderPropertyRef);
	refs.insert(&alphaPropertyRef);
}

void BSGeometry::GetChildIndices(std::vector<uint32_t>& indices) {
	NiAVObject::GetChildIndices(indices);

	indices.push_back(skinInstanceRef.index);
	indices.push_back(shaderPropertyRef.index);
	indices.push_back(alphaPropertyRef.index);
}


NiGeometryData* BSGeometry::GetGeomData() const {
	if (meshes.size() > selectedMesh) {
		// Breaking const correctness here to cast to the desired level of the class heirarchy.
		//   Perhaps NiShape GetGeomData should return a const* or it shouldn't be a const function? 
		return dynamic_cast<NiGeometryData*>(const_cast<BSGeometryMeshData*>(&meshes[selectedMesh].meshData));
	}
	return nullptr;
}


bool BSGeometry::GetTriangles(std::vector<Triangle>& tris) const {
	if (meshes.size() > selectedMesh) {
		tris = meshes[selectedMesh].meshData.tris;
		return true;
	}

	return false;
}

void BSGeometry::SetTriangles(const std::vector<Triangle>& tris) {
	if (meshes.size() > selectedMesh) {
		meshes[selectedMesh].meshData.tris = tris;
	}
}


void NiGeometry::Sync(NiStreamReversible& stream) {
	dataRef.Sync(stream);
	skinInstanceRef.Sync(stream);

	if (stream.GetVersion().File() >= V20_2_0_5) {
		uint32_t numMaterials = materialNames.Sync(stream);
		materialExtraData.SyncData(stream, numMaterials);

		stream.Sync(activeMaterial);
	}
	else {
		stream.Sync(shader);

		if (shader) {
			shaderName.Sync(stream);
			stream.Sync(implementation);
		}
	}

	if (stream.GetVersion().File() >= V20_2_0_7)
		stream.Sync(defaultMatNeedsUpdateFlag);

	if (stream.GetVersion().Stream() > 34) {
		shaderPropertyRef.Sync(stream);
		alphaPropertyRef.Sync(stream);
	}
}

void NiGeometry::GetStringRefs(std::vector<NiStringRef*>& refs) {
	NiAVObject::GetStringRefs(refs);

	for (auto& mn : materialNames)
		refs.emplace_back(&mn);
}

void NiGeometry::GetChildRefs(std::set<NiRef*>& refs) {
	NiAVObject::GetChildRefs(refs);

	refs.insert(&dataRef);
	refs.insert(&skinInstanceRef);
	refs.insert(&shaderPropertyRef);
	refs.insert(&alphaPropertyRef);
}

void NiGeometry::GetChildIndices(std::vector<uint32_t>& indices) {
	NiAVObject::GetChildIndices(indices);

	indices.push_back(dataRef.index);
	indices.push_back(skinInstanceRef.index);
	indices.push_back(shaderPropertyRef.index);
	indices.push_back(alphaPropertyRef.index);
}

bool NiGeometry::IsSkinned() const {
	return !skinInstanceRef.IsEmpty();
}


void NiTriBasedGeomData::Sync(NiStreamReversible& stream) {
	stream.Sync(numTriangles);
}

void NiTriBasedGeomData::Create(NiVersion& version,
								const std::vector<Vector3>* verts,
								const std::vector<Triangle>* inTris,
								const std::vector<Vector2>* uvs,
								const std::vector<Vector3>* norms) {
	NiGeometryData::Create(version, verts, inTris, uvs, norms);

	if (inTris) {
		constexpr uint16_t maxIndex = std::numeric_limits<uint16_t>::max();
		size_t triCount = inTris ? inTris->size() : 0;

		if (numVertices == 0)
			numTriangles = 0;
		else if (triCount > static_cast<size_t>(maxIndex))
			numTriangles = maxIndex;
		else
			numTriangles = uint16_t(triCount);
	}
}


void NiTriShapeData::Sync(NiStreamReversible& stream) {
	stream.Sync(numTrianglePoints);
	stream.Sync(hasTriangles);

	if (hasTriangles) {
		triangles.resize(numTriangles);
		for (uint32_t i = 0; i < numTriangles; i++)
			stream.Sync(triangles[i]);
	}

	stream.Sync(numMatchGroups);
	matchGroups.resize(numMatchGroups);

	for (uint32_t i = 0; i < numMatchGroups; i++) {
		auto& mg = matchGroups[i];

		stream.Sync(mg.count);
		mg.matches.resize(mg.count);

		for (uint32_t j = 0; j < mg.count; j++)
			stream.Sync(mg.matches[j]);
	}

	// Not supported yet, so clear it again after reading
	matchGroups.clear();
	numMatchGroups = 0;
}

void NiTriShapeData::Create(NiVersion& version,
							const std::vector<Vector3>* verts,
							const std::vector<Triangle>* inTris,
							const std::vector<Vector2>* uvs,
							const std::vector<Vector3>* norms) {
	NiTriBasedGeomData::Create(version, verts, inTris, uvs, norms);

	if (numTriangles > 0) {
		numTrianglePoints = numTriangles * 3;
		hasTriangles = true;
	}
	else {
		numTrianglePoints = 0;
		hasTriangles = false;
	}

	if (inTris) {
		triangles.resize(numTriangles);
		for (uint16_t t = 0; t < numTriangles; t++)
			triangles[t] = (*inTris)[t];
	}

	numMatchGroups = 0;

	// Calculate again, now with triangles
	CalcTangentSpace();
}

void NiTriShapeData::notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) {
	std::vector<int> indexCollapse = GenerateIndexCollapseMap(vertIndices, vertices.size());
	ApplyMapToTriangles(triangles, indexCollapse);
	numTriangles = static_cast<uint16_t>(triangles.size());
	numTrianglePoints = 3 * numTriangles;

	NiTriBasedGeomData::notifyVerticesDelete(vertIndices);
}

std::vector<MatchGroup> NiTriShapeData::GetMatchGroups() const {
	return matchGroups;
}

void NiTriShapeData::SetMatchGroups(const std::vector<MatchGroup>& mg) {
	matchGroups = mg;
	numMatchGroups = static_cast<uint16_t>(matchGroups.size());
}

uint32_t NiTriShapeData::GetNumTriangles() const {
	return numTriangles;
}

bool NiTriShapeData::GetTriangles(std::vector<Triangle>& tris) const {
	tris = triangles;
	return hasTriangles;
}

void NiTriShapeData::SetTriangles(const std::vector<Triangle>& tris) {
	hasTriangles = true;
	triangles = tris;
	numTriangles = static_cast<uint16_t>(triangles.size());
	numTrianglePoints = numTriangles * 3;
}

void NiTriShapeData::RecalcNormals(const bool smooth,
								   const float smoothThresh,
								   std::unordered_set<uint32_t>* lockedIndices) {
	if (!HasNormals())
		return;

	NiTriBasedGeomData::RecalcNormals();

	CalculateNormals(vertices, triangles, normals, smooth, smoothThresh, lockedIndices);
}

void NiTriShapeData::CalcTangentSpace() {
	if (!HasNormals() || !HasUVs())
		return;

	NiTriBasedGeomData::CalcTangentSpace();

	std::vector<Vector3> tan1;
	std::vector<Vector3> tan2;
	tan1.resize(numVertices);
	tan2.resize(numVertices);

	for (uint32_t i = 0; i < numTriangles; i++) {
		int i1 = triangles[i].p1;
		int i2 = triangles[i].p2;
		int i3 = triangles[i].p3;

		if (i1 >= numVertices || i2 >= numVertices || i3 >= numVertices)
			continue;

		Vector3 v1 = vertices[i1];
		Vector3 v2 = vertices[i2];
		Vector3 v3 = vertices[i3];

		Vector2 w1 = uvSets[0][i1];
		Vector2 w2 = uvSets[0][i2];
		Vector2 w3 = uvSets[0][i3];

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.u - w1.u;
		float s2 = w3.u - w1.u;
		float t1 = w2.v - w1.v;
		float t2 = w3.v - w1.v;

		float r = (s1 * t2 - s2 * t1);
		r = (r >= 0.0f ? +1.0f : -1.0f);

		Vector3 sdir = Vector3((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
		Vector3 tdir = Vector3((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

		sdir.Normalize();
		tdir.Normalize();

		tan1[i1] += sdir;
		tan1[i2] += sdir;
		tan1[i3] += sdir;

		tan2[i1] += tdir;
		tan2[i2] += tdir;
		tan2[i3] += tdir;
	}

	for (uint16_t i = 0; i < numVertices; i++) {
		bitangents[i] = tan1[i];
		tangents[i] = tan2[i];

		if (tangents[i].IsZero() || bitangents[i].IsZero()) {
			tangents[i].x = normals[i].y;
			tangents[i].y = normals[i].z;
			tangents[i].z = normals[i].x;
			bitangents[i] = normals[i].cross(tangents[i]);
		}
		else {
			tangents[i].Normalize();
			tangents[i] = (tangents[i] - normals[i] * normals[i].dot(tangents[i]));
			tangents[i].Normalize();

			bitangents[i].Normalize();

			bitangents[i] = (bitangents[i] - normals[i] * normals[i].dot(bitangents[i]));
			bitangents[i] = (bitangents[i] - tangents[i] * tangents[i].dot(bitangents[i]));

			bitangents[i].Normalize();
		}
	}
}


NiGeometryData* NiTriShape::GetGeomData() const {
	return shapeData;
};

void NiTriShape::SetGeomData(NiGeometryData* geomDataPtr) {
	auto geomData = dynamic_cast<NiTriShapeData*>(geomDataPtr);
	if (geomData)
		shapeData = geomData;
}


void StripsInfo::Sync(NiStreamReversible& stream) {
	stripLengths.Sync(stream);

	if (stream.GetVersion().File() >= NiFileVersion::V10_0_1_3)
		stream.Sync(hasPoints);
	else
		hasPoints = true;

	if (hasPoints) {
		points.resize(stripLengths.size());
		for (uint16_t i = 0; i < stripLengths.size(); i++) {
			points[i].resize(stripLengths[i]);
			for (uint16_t j = 0; j < stripLengths[i]; j++)
				stream.Sync(points[i][j]);
		}
	}
}


void NiTriStripsData::Sync(NiStreamReversible& stream) {
	stripsInfo.Sync(stream);
}

void NiTriStripsData::notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) {
	std::vector<int> indexCollapse = GenerateIndexCollapseMap(vertIndices, vertices.size());

	NiTriBasedGeomData::notifyVerticesDelete(vertIndices);

	// This is not a healthy way to delete strip data. Probably need to restrip the shape.
	for (uint16_t i = 0; i < stripsInfo.stripLengths.size(); i++) {
		for (uint16_t j = 0; j < stripsInfo.stripLengths[i]; j++) {
			if (indexCollapse[stripsInfo.points[i][j]] == -1) {
				stripsInfo.points[i].erase(stripsInfo.points[i].begin() + j);
				stripsInfo.stripLengths[i]--;
				--j;
			}
			else
				stripsInfo.points[i][j] = static_cast<uint16_t>(indexCollapse[stripsInfo.points[i][j]]);
		}
	}

	numTriangles = 0;
	for (auto len : stripsInfo.stripLengths)
		if (len - 2 > 0)
			numTriangles += len - 2;
}

uint32_t NiTriStripsData::GetNumTriangles() const {
	return static_cast<uint32_t>(StripsToTris().size());
}

bool NiTriStripsData::GetTriangles(std::vector<Triangle>& tris) const {
	tris = StripsToTris();
	return stripsInfo.hasPoints;
}

void NiTriStripsData::SetTriangles(const std::vector<Triangle>& /*tris*/) {
	// Not implemented, stripify here
}

std::vector<Triangle> NiTriStripsData::StripsToTris() const {
	return GenerateTrianglesFromStrips(stripsInfo.points);
}

void NiTriStripsData::RecalcNormals(const bool smooth,
									const float smoothThresh,
									std::unordered_set<uint32_t>* lockedIndices) {
	if (!HasNormals())
		return;

	NiTriBasedGeomData::RecalcNormals();

	std::vector<Triangle> tris = StripsToTris();

	CalculateNormals(vertices, tris, normals, smooth, smoothThresh, lockedIndices);
}

void NiTriStripsData::CalcTangentSpace() {
	if (!HasNormals() || !HasUVs())
		return;

	NiTriBasedGeomData::CalcTangentSpace();

	std::vector<Vector3> tan1;
	std::vector<Vector3> tan2;
	tan1.resize(numVertices);
	tan2.resize(numVertices);

	std::vector<Triangle> tris = StripsToTris();

	for (auto& tri : tris) {
		int i1 = tri.p1;
		int i2 = tri.p2;
		int i3 = tri.p3;

		if (i1 >= numVertices || i2 >= numVertices || i3 >= numVertices)
			continue;

		Vector3 v1 = vertices[i1];
		Vector3 v2 = vertices[i2];
		Vector3 v3 = vertices[i3];

		Vector2 w1 = uvSets[0][i1];
		Vector2 w2 = uvSets[0][i2];
		Vector2 w3 = uvSets[0][i3];

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.u - w1.u;
		float s2 = w3.u - w1.u;
		float t1 = w2.v - w1.v;
		float t2 = w3.v - w1.v;

		float r = (s1 * t2 - s2 * t1);
		r = (r >= 0.0f ? +1.0f : -1.0f);

		Vector3 sdir = Vector3((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
		Vector3 tdir = Vector3((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

		sdir.Normalize();
		tdir.Normalize();

		tan1[i1] += sdir;
		tan1[i2] += sdir;
		tan1[i3] += sdir;

		tan2[i1] += tdir;
		tan2[i2] += tdir;
		tan2[i3] += tdir;
	}

	for (uint16_t i = 0; i < numVertices; i++) {
		bitangents[i] = tan1[i];
		tangents[i] = tan2[i];

		if (tangents[i].IsZero() || bitangents[i].IsZero()) {
			tangents[i].x = normals[i].y;
			tangents[i].y = normals[i].z;
			tangents[i].z = normals[i].x;
			bitangents[i] = normals[i].cross(tangents[i]);
		}
		else {
			tangents[i].Normalize();
			tangents[i] = (tangents[i] - normals[i] * normals[i].dot(tangents[i]));
			tangents[i].Normalize();

			bitangents[i].Normalize();

			bitangents[i] = (bitangents[i] - normals[i] * normals[i].dot(bitangents[i]));
			bitangents[i] = (bitangents[i] - tangents[i] * tangents[i].dot(bitangents[i]));

			bitangents[i].Normalize();
		}
	}
}


NiGeometryData* NiTriStrips::GetGeomData() const {
	return stripsData;
};

void NiTriStrips::SetGeomData(NiGeometryData* geomDataPtr) {
	auto geomData = dynamic_cast<NiTriStripsData*>(geomDataPtr);
	if (geomData)
		stripsData = geomData;
}


void NiLinesData::Sync(NiStreamReversible& stream) {
	lineFlags.resize(numVertices);
	for (uint16_t i = 0; i < numVertices; i++)
		stream.Sync(lineFlags[i]);
}

void NiLinesData::notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) {
	NiGeometryData::notifyVerticesDelete(vertIndices);

	EraseVectorIndices(lineFlags, vertIndices);
}


NiGeometryData* NiLines::GetGeomData() const {
	return linesData;
}

void NiLines::SetGeomData(NiGeometryData* geomDataPtr) {
	auto geomData = dynamic_cast<NiLinesData*>(geomDataPtr);
	if (geomData)
		linesData = geomData;
}


void NiScreenElementsData::Sync(NiStreamReversible& stream) {
	stream.Sync(maxPolygons);
	polygons.resize(maxPolygons);
	for (uint32_t i = 0; i < maxPolygons; i++)
		stream.Sync(polygons[i]);

	polygonIndices.resize(maxPolygons);
	for (uint32_t i = 0; i < maxPolygons; i++)
		stream.Sync(polygonIndices[i]);

	stream.Sync(polygonGrowBy);
	stream.Sync(numPolygons);
	stream.Sync(maxVertices);
	stream.Sync(verticesGrowBy);
	stream.Sync(maxIndices);
	stream.Sync(indicesGrowBy);
}

void NiScreenElementsData::notifyVerticesDelete(const std::vector<uint16_t>& vertIndices) {
	NiTriShapeData::notifyVerticesDelete(vertIndices);

	// Clearing as workaround
	maxPolygons = 0;
	polygons.clear();
	polygonIndices.clear();
	numPolygons = 0;
	maxVertices = 0;
	maxIndices = 0;
}


NiGeometryData* NiScreenElements::GetGeomData() const {
	return elemData;
}

void NiScreenElements::SetGeomData(NiGeometryData* geomDataPtr) {
	auto geomData = dynamic_cast<NiScreenElementsData*>(geomDataPtr);
	if (geomData)
		elemData = geomData;
}


void BSLODTriShape::Sync(NiStreamReversible& stream) {
	stream.Sync(level0);
	stream.Sync(level1);
	stream.Sync(level2);
}

NiGeometryData* BSLODTriShape::GetGeomData() const {
	return shapeData;
}

void BSLODTriShape::SetGeomData(NiGeometryData* geomDataPtr) {
	auto geomData = dynamic_cast<NiTriShapeData*>(geomDataPtr);
	if (geomData)
		shapeData = geomData;
}


void BSGeometrySegmentData::Sync(NiStreamReversible& stream) {
	stream.Sync(flags);
	stream.Sync(index);
	stream.Sync(numTris);
}


void BSSegmentedTriShape::Sync(NiStreamReversible& stream) {
	stream.Sync(numSegments);
	segments.resize(numSegments);

	for (auto& segment : segments)
		segment.Sync(stream);
}

std::vector<BSGeometrySegmentData> BSSegmentedTriShape::GetSegments() const {
	return segments;
}

void BSSegmentedTriShape::SetSegments(const std::vector<BSGeometrySegmentData>& sd) {
	segments = sd;
	numSegments = static_cast<uint32_t>(segments.size());
}
