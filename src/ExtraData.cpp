/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#include "ExtraData.hpp"

#include <fstream>

using namespace nifly;

void NiExtraData::Sync(NiStreamReversible& stream) {
	name.Sync(stream);
}

void NiExtraData::GetStringRefs(std::vector<NiStringRef*>& refs) {
	NiObject::GetStringRefs(refs);

	refs.emplace_back(&name);
}


void NiBinaryExtraData::Sync(NiStreamReversible& stream) {
	data.Sync(stream);
}


void NiFloatExtraData::Sync(NiStreamReversible& stream) {
	stream.Sync(floatData);
}


void NiFloatsExtraData::Sync(NiStreamReversible& stream) {
	floatsData.Sync(stream);
}


void NiStringsExtraData::Sync(NiStreamReversible& stream) {
	stringsData.Sync(stream);
}


void NiStringExtraData::Sync(NiStreamReversible& stream) {
	stringData.Sync(stream);
}

void NiStringExtraData::GetStringRefs(std::vector<NiStringRef*>& refs) {
	NiExtraData::GetStringRefs(refs);

	refs.emplace_back(&stringData);
}


void NiBooleanExtraData::Sync(NiStreamReversible& stream) {
	stream.Sync(booleanData);
}


void NiIntegerExtraData::Sync(NiStreamReversible& stream) {
	stream.Sync(integerData);
}


void NiIntegersExtraData::Sync(NiStreamReversible& stream) {
	integersData.Sync(stream);
}


void NiVectorExtraData::Sync(NiStreamReversible& stream) {
	stream.Sync(vectorData);
}


void NiColorExtraData::Sync(NiStreamReversible& stream) {
	stream.Sync(colorData);
}


void BSWArray::Sync(NiStreamReversible& stream) {
	data.Sync(stream);
}


void BSPositionData::Sync(NiStreamReversible& stream) {
	data.Sync(stream);
}


void BSEyeCenterExtraData::Sync(NiStreamReversible& stream) {
	data.Sync(stream);
}


void BSPackedGeomData::Sync(NiStreamReversible& stream) {
	stream.Sync(numVertices);

	stream.Sync(lodLevels);
	stream.Sync(triCountLod0);
	stream.Sync(triOffsetLod0);
	stream.Sync(triCountLod1);
	stream.Sync(triOffsetLod1);
	stream.Sync(triCountLod2);
	stream.Sync(triOffsetLod2);

	combined.Sync(stream);

	stream.Sync(vertexDesc);

	vertData.resize(numVertices);

	for (uint32_t i = 0; i < numVertices; i++) {
		auto& vertex = vertData[i];
		if (HasVertices()) {
			if (IsFullPrecision() || stream.GetVersion().Stream() == 100) {
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

	triangles.resize(triCountLod0 + triCountLod1 + triCountLod2);
	for (auto& t : triangles)
		stream.Sync(t);
}

void BSPackedGeomData::SetVertices(const bool enable) {
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

void BSPackedGeomData::SetUVs(const bool enable) {
	if (enable)
		vertexDesc.SetFlag(VF_UV);
	else
		vertexDesc.RemoveFlag(VF_UV);
}

void BSPackedGeomData::SetSecondUVs(const bool enable) {
	if (enable)
		vertexDesc.SetFlag(VF_UV_2);
	else
		vertexDesc.RemoveFlag(VF_UV_2);
}

void BSPackedGeomData::SetNormals(const bool enable) {
	if (enable)
		vertexDesc.SetFlag(VF_NORMAL);
	else
		vertexDesc.RemoveFlag(VF_NORMAL);
}

void BSPackedGeomData::SetTangents(const bool enable) {
	if (enable)
		vertexDesc.SetFlag(VF_TANGENT);
	else
		vertexDesc.RemoveFlag(VF_TANGENT);
}

void BSPackedGeomData::SetVertexColors(const bool enable) {
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

void BSPackedGeomData::SetSkinned(const bool enable) {
	if (enable)
		vertexDesc.SetFlag(VF_SKINNED);
	else
		vertexDesc.RemoveFlag(VF_SKINNED);
}

void BSPackedGeomData::SetEyeData(const bool enable) {
	if (enable)
		vertexDesc.SetFlag(VF_EYEDATA);
	else
		vertexDesc.RemoveFlag(VF_EYEDATA);
}

void BSPackedGeomData::SetFullPrecision(const bool enable) {
	if (!CanChangePrecision())
		return;

	if (enable)
		vertexDesc.SetFlag(VF_FULLPREC);
	else
		vertexDesc.RemoveFlag(VF_FULLPREC);
}


void BSPackedCombinedSharedGeomDataExtra::Sync(NiStreamReversible& stream) {
	vertexDesc.Sync(stream);
	stream.Sync(numVertices);
	stream.Sync(numTriangles);
	stream.Sync(unkFlags1);
	stream.Sync(unkFlags2);

	stream.Sync(numData);
	objects.resize(numData);
	data.resize(numData);

	for (uint32_t i = 0; i < numData; i++)
		stream.Sync(objects[i]);

	for (uint32_t i = 0; i < numData; i++)
		data[i].Sync(stream);
}


void BSInvMarker::Sync(NiStreamReversible& stream) {
	stream.Sync(rotationX);
	stream.Sync(rotationY);
	stream.Sync(rotationZ);
	stream.Sync(zoom);
}


void FurniturePosition::Sync(NiStreamReversible& stream) {
	stream.Sync(offset);

	if (stream.GetVersion().User() <= 11) {
		stream.Sync(orientation);
		stream.Sync(posRef1);
		stream.Sync(posRef2);
	}

	if (stream.GetVersion().User() >= 12) {
		stream.Sync(heading);
		stream.Sync(animationType);
		stream.Sync(entryPoints);
	}
}


void BSFurnitureMarker::Sync(NiStreamReversible& stream) {
	positions.Sync(stream);
}

void DecalVectorBlock::Sync(NiStreamReversible& stream) {
	points.Sync(stream);
	normals.SyncData(stream, points.size());
}


void BSDecalPlacementVectorExtraData::Sync(NiStreamReversible& stream) {
	decalVectorBlocks.Sync(stream);
}


void BSBehaviorGraphExtraData::Sync(NiStreamReversible& stream) {
	behaviorGraphFile.Sync(stream);
	stream.Sync(controlsBaseSkel);
}

void BSBehaviorGraphExtraData::GetStringRefs(std::vector<NiStringRef*>& refs) {
	NiExtraData::GetStringRefs(refs);

	refs.emplace_back(&behaviorGraphFile);
}


void BSBound::Sync(NiStreamReversible& stream) {
	stream.Sync(center);
	stream.Sync(halfExtents);
}


void BoneLOD::Sync(NiStreamReversible& stream) {
	stream.Sync(distance);
	boneName.Sync(stream);
}

void BoneLOD::GetStringRefs(std::vector<NiStringRef*>& refs) {
	refs.emplace_back(&boneName);
}


void BSBoneLODExtraData::Sync(NiStreamReversible& stream) {
	boneLODs.Sync(stream);
}

void BSBoneLODExtraData::GetStringRefs(std::vector<NiStringRef*>& refs) {
	NiExtraData::GetStringRefs(refs);

	boneLODs.GetStringRefs(refs);
}


void NiTextKeyExtraData::Sync(NiStreamReversible& stream) {
	textKeys.Sync(stream);
}

void NiTextKeyExtraData::GetStringRefs(std::vector<NiStringRef*>& refs) {
	NiExtraData::GetStringRefs(refs);

	textKeys.GetStringRefs(refs);
}


void BSDistantObjectLargeRefExtraData::Sync(NiStreamReversible& stream) {
	stream.Sync(largeRef);
}


void BSDistantObjectExtraData::Sync(NiStreamReversible& stream) {
	stream.Sync(distantObjectFlags);
}


void BSConnectPoint::Sync(NiStreamReversible& stream) {
	root.Sync(stream, 4);
	variableName.Sync(stream, 4);

	stream.Sync(rotation);
	stream.Sync(translation);
	stream.Sync(scale);
}


void BSConnectPointParents::Sync(NiStreamReversible& stream) {
	connectPoints.Sync(stream);
}


void BSConnectPointChildren::Sync(NiStreamReversible& stream) {
	stream.Sync(skinned);
	targets.Sync(stream);
}


BSClothExtraData::BSClothExtraData(const uint32_t size) {
	data.resize(size);
}

void BSClothExtraData::Sync(NiStreamReversible& stream) {
	data.SyncByteArray(stream);
}


bool BSClothExtraData::ToHKX(const std::string& fileName) {
	std::ofstream file(fileName, std::ios_base::binary);
	if (!file)
		return false;

	file.write(data.data(), data.size());
	return true;
}

bool BSClothExtraData::FromHKX(const std::string& fileName) {
	std::ifstream file(fileName, std::ios::binary | std::ios::ate);
	if (!file)
		return false;

	auto numBytes = static_cast<uint32_t>(file.tellg());
	file.seekg(0, std::ios::beg);

	data.resize(numBytes);
	file.read(data.data(), numBytes);
	return true;
}


void BSCollisionQueryProxyExtraData::Sync(NiStreamReversible& stream) {
	data.SyncByteArray(stream);
}


void SkinAttach::Sync(NiStreamReversible& stream) {
	bones.Sync(stream);
}


void BoneTranslations::Sync(NiStreamReversible& stream) {
	stream.Sync(numTranslations);
	translations.resize(numTranslations);
	for (uint32_t i = 0; i < numTranslations; i++) {
		translations[i].bone.Sync(stream, 4);
		stream.Sync(translations[i].trans);
	}
}
