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

void NiNode::GetChildRefs(std::set<Ref*>& refs) {
	NiAVObject::GetChildRefs(refs);

	childRefs.GetIndexPtrs(refs);
	effectRefs.GetIndexPtrs(refs);
}

void NiNode::GetChildIndices(std::vector<int>& indices) {
	NiAVObject::GetChildIndices(indices);

	childRefs.GetIndices(indices);
	effectRefs.GetIndices(indices);
}

BlockRefArray<NiAVObject>& NiNode::GetChildren() {
	return childRefs;
}

BlockRefArray<NiDynamicEffect>& NiNode::GetEffects() {
	return effectRefs;
}


void BSValueNode::Sync(NiStreamReversible& stream) {
	stream.Sync(value);
	stream.Sync(valueFlags);
}


void BSTreeNode::Sync(NiStreamReversible& stream) {
	bones1.Sync(stream);
	bones2.Sync(stream);
}

void BSTreeNode::GetChildRefs(std::set<Ref*>& refs) {
	NiNode::GetChildRefs(refs);

	bones1.GetIndexPtrs(refs);
	bones2.GetIndexPtrs(refs);
}

void BSTreeNode::GetChildIndices(std::vector<int>& indices) {
	NiNode::GetChildIndices(indices);

	bones1.GetIndices(indices);
	bones2.GetIndices(indices);
}

BlockRefArray<NiNode>& BSTreeNode::GetBones1() {
	return bones1;
}

BlockRefArray<NiNode>& BSTreeNode::GetBones2() {
	return bones2;
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

void BSMultiBound::GetChildRefs(std::set<Ref*>& refs) {
	NiObject::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void BSMultiBound::GetChildIndices(std::vector<int>& indices) {
	NiObject::GetChildIndices(indices);

	indices.push_back(dataRef.GetIndex());
}

int BSMultiBound::GetDataRef() {
	return dataRef.GetIndex();
}

void BSMultiBound::SetDataRef(int datRef) {
	dataRef.SetIndex(datRef);
}


void BSMultiBoundNode::Sync(NiStreamReversible& stream) {
	multiBoundRef.Sync(stream);

	if (stream.GetVersion().User() >= 12)
		stream.Sync(cullingMode);
}

void BSMultiBoundNode::GetChildRefs(std::set<Ref*>& refs) {
	NiNode::GetChildRefs(refs);

	refs.insert(&multiBoundRef);
}

void BSMultiBoundNode::GetChildIndices(std::vector<int>& indices) {
	NiNode::GetChildIndices(indices);

	indices.push_back(multiBoundRef.GetIndex());
}


int BSMultiBoundNode::GetMultiBoundRef() {
	return multiBoundRef.GetIndex();
}

void BSMultiBoundNode::SetMultiBoundRef(int multBoundRef) {
	multiBoundRef.SetIndex(multBoundRef);
}


void BSRangeNode::Sync(NiStreamReversible& stream) {
	stream.Sync(min);
	stream.Sync(max);
	stream.Sync(current);
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
	stream.Sync(numLODLevels);

	lodLevels.resize(numLODLevels);
	for (uint32_t i = 0; i < numLODLevels; i++)
		stream.Sync(lodLevels[i]);
}


void NiScreenLODData::Sync(NiStreamReversible& stream) {
	stream.Sync(boundCenter);
	stream.Sync(boundRadius);
	stream.Sync(worldCenter);
	stream.Sync(worldRadius);
	stream.Sync(numProportions);

	proportionLevels.resize(numProportions);
	for (uint32_t i = 0; i < numProportions; i++)
		stream.Sync(proportionLevels[i]);
}


void NiLODNode::Sync(NiStreamReversible& stream) {
	lodLevelData.Sync(stream);
}

void NiLODNode::GetChildRefs(std::set<Ref*>& refs) {
	NiSwitchNode::GetChildRefs(refs);

	refs.insert(&lodLevelData);
}

void NiLODNode::GetChildIndices(std::vector<int>& indices) {
	NiSwitchNode::GetChildIndices(indices);

	indices.push_back(lodLevelData.GetIndex());
}

int NiLODNode::GetLodLevelDataRef() {
	return lodLevelData.GetIndex();
}

void NiLODNode::SetLodLevelDataRef(int dataRef) {
	lodLevelData.SetIndex(dataRef);
}


void NiSortAdjustNode::Sync(NiStreamReversible& stream) {
	stream.Sync(sortingMode);
}
