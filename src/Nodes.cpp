/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#include "Nodes.hpp"

using namespace nifly;

void NiNode::Sync(NiStreamReversible& stream) {
	childRefs.Sync(stream);

	if (stream.GetVersion().User() <= 12 && stream.GetVersion().Stream() < 130)
		effectRefs.Sync(stream);
}

void NiNode::GetChildRefs(std::set<NiRef*>& refs) {
	NiAVObject::GetChildRefs(refs);

	childRefs.GetIndexPtrs(refs);
	effectRefs.GetIndexPtrs(refs);
}

void NiNode::GetChildIndices(std::vector<uint32_t>& indices) {
	NiAVObject::GetChildIndices(indices);

	childRefs.GetIndices(indices);
	effectRefs.GetIndices(indices);
}


void BSValueNode::Sync(NiStreamReversible& stream) {
	stream.Sync(value);
	stream.Sync(valueFlags);
}


void BSTreeNode::Sync(NiStreamReversible& stream) {
	bones1.Sync(stream);
	bones2.Sync(stream);
}

void BSTreeNode::GetChildRefs(std::set<NiRef*>& refs) {
	NiNode::GetChildRefs(refs);

	bones1.GetIndexPtrs(refs);
	bones2.GetIndexPtrs(refs);
}

void BSTreeNode::GetChildIndices(std::vector<uint32_t>& indices) {
	NiNode::GetChildIndices(indices);

	bones1.GetIndices(indices);
	bones2.GetIndices(indices);
}


void BSOrderedNode::Sync(NiStreamReversible& stream) {
	stream.Sync(alphaSortBound);
	stream.Sync(isStaticBound);
}


void BSMultiBoundOBB::Sync(NiStreamReversible& stream) {
	stream.Sync(center);
	stream.Sync(size);
	stream.Sync(rotation);
}


void BSMultiBoundAABB::Sync(NiStreamReversible& stream) {
	stream.Sync(center);
	stream.Sync(halfExtent);
}


void BSMultiBoundSphere::Sync(NiStreamReversible& stream) {
	stream.Sync(center);
	stream.Sync(radius);
}


void BSMultiBound::Sync(NiStreamReversible& stream) {
	dataRef.Sync(stream);
}

void BSMultiBound::GetChildRefs(std::set<NiRef*>& refs) {
	NiObject::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void BSMultiBound::GetChildIndices(std::vector<uint32_t>& indices) {
	NiObject::GetChildIndices(indices);

	indices.push_back(dataRef.index);
}


void BSMultiBoundNode::Sync(NiStreamReversible& stream) {
	multiBoundRef.Sync(stream);

	if (stream.GetVersion().User() >= 12)
		stream.Sync(cullingMode);
}

void BSMultiBoundNode::GetChildRefs(std::set<NiRef*>& refs) {
	NiNode::GetChildRefs(refs);

	refs.insert(&multiBoundRef);
}

void BSMultiBoundNode::GetChildIndices(std::vector<uint32_t>& indices) {
	NiNode::GetChildIndices(indices);

	indices.push_back(multiBoundRef.index);
}


void BSDistantObjectInstancedNode::Sync(NiStreamReversible& stream) {
	instances.Sync(stream);

	for (int i = 0; i < 3; i++)
		textureArrays[i].Sync(stream);
}


void BSRangeNode::Sync(NiStreamReversible& stream) {
	stream.Sync(min);
	stream.Sync(max);
	stream.Sync(current);
}


void UnkMaterialStruct::Sync(NiStreamReversible& stream) {
	stream.Sync(biomeFormID);
	stream.Sync(dirHash);
	stream.Sync(fileHash);
	stream.SyncString(mat);
}

void BSWaterReferenceStruct::Sync(NiStreamReversible& stream) {
	stream.Sync(transform);
	stream.Sync(resourceID);
	stream.Sync(unkInt1);
	material.Sync(stream, 4);
}

void BSWeakReference::Sync(NiStreamReversible& stream) {
	if (stream.GetVersion().Stream() >= 173)
		stream.Sync(formID);

	stream.Sync(resourceID);
	stream.Sync(numTransforms);
	transforms.resize(numTransforms);
	for (uint32_t i = 0; i < numTransforms; i++)
		stream.Sync(transforms[i]);

	stream.Sync(numMaterials);
	unkMaterials.resize(numMaterials);
	for (uint32_t i = 0; i < numMaterials; i++)
		unkMaterials[i].Sync(stream);
}

void BSWeakReferenceNode::Sync(NiStreamReversible& stream) {
	stream.Sync(numWeakRefs);
	weakRefs.resize(numWeakRefs);
	for (uint32_t i = 0; i < numWeakRefs; i++)
		weakRefs[i].Sync(stream);

	stream.Sync(unkInt1);
	stream.Sync(numWaterRefs);
	waterRefs.resize(numWaterRefs);
	for (uint32_t i = 0; i < numWaterRefs; i++)
		waterRefs[i].Sync(stream);
}


void BSFaceGenNiNode::Sync(NiStreamReversible& stream) {
	stream.Sync(unkShort);
}


void NiBillboardNode::Sync(NiStreamReversible& stream) {
	stream.Sync(billboardMode);
}


void NiSwitchNode::Sync(NiStreamReversible& stream) {
	stream.Sync(flags);
	stream.Sync(index);
}


void NiRangeLODData::Sync(NiStreamReversible& stream) {
	stream.Sync(lodCenter);
	lodLevels.Sync(stream);
}


void NiScreenLODData::Sync(NiStreamReversible& stream) {
	stream.Sync(boundCenter);
	stream.Sync(boundRadius);
	stream.Sync(worldCenter);
	stream.Sync(worldRadius);
	proportionLevels.Sync(stream);
}


void NiLODNode::Sync(NiStreamReversible& stream) {
	lodLevelData.Sync(stream);
}

void NiLODNode::GetChildRefs(std::set<NiRef*>& refs) {
	NiSwitchNode::GetChildRefs(refs);

	refs.insert(&lodLevelData);
}

void NiLODNode::GetChildIndices(std::vector<uint32_t>& indices) {
	NiSwitchNode::GetChildIndices(indices);

	indices.push_back(lodLevelData.index);
}


void NiSortAdjustNode::Sync(NiStreamReversible& stream) {
	stream.Sync(sortingMode);
}
