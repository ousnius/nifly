/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#include "Nodes.hpp"

using namespace nifly;

void NiNode::Get(NiIStream& stream) {
	NiAVObject::Get(stream);

	childRefs.Get(stream);

	if (stream.GetVersion().User() <= 12 && stream.GetVersion().Stream() < 130)
		effectRefs.Get(stream);
}

void NiNode::Put(NiOStream& stream) {
	NiAVObject::Put(stream);

	childRefs.Put(stream);

	if (stream.GetVersion().User() <= 12 && stream.GetVersion().Stream() < 130)
		effectRefs.Put(stream);
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


void BSValueNode::Get(NiIStream& stream) {
	NiNode::Get(stream);

	stream >> value;
	stream >> valueFlags;
}

void BSValueNode::Put(NiOStream& stream) {
	NiNode::Put(stream);

	stream << value;
	stream << valueFlags;
}


void BSTreeNode::Get(NiIStream& stream) {
	NiNode::Get(stream);

	bones1.Get(stream);
	bones2.Get(stream);
}

void BSTreeNode::Put(NiOStream& stream) {
	NiNode::Put(stream);

	bones1.Put(stream);
	bones2.Put(stream);
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


void BSOrderedNode::Get(NiIStream& stream) {
	NiNode::Get(stream);

	stream >> alphaSortBound;
	stream >> isStaticBound;
}

void BSOrderedNode::Put(NiOStream& stream) {
	NiNode::Put(stream);

	stream << alphaSortBound;
	stream << isStaticBound;
}


void BSMultiBoundOBB::Get(NiIStream& stream) {
	BSMultiBoundData::Get(stream);

	stream >> center;
	stream >> size;
	stream >> rotation;
}

void BSMultiBoundOBB::Put(NiOStream& stream) {
	BSMultiBoundData::Put(stream);

	stream << center;
	stream << size;
	stream << rotation;
}


void BSMultiBoundAABB::Get(NiIStream& stream) {
	BSMultiBoundData::Get(stream);

	stream >> center;
	stream >> halfExtent;
}

void BSMultiBoundAABB::Put(NiOStream& stream) {
	BSMultiBoundData::Put(stream);

	stream << center;
	stream << halfExtent;
}


void BSMultiBoundSphere::Get(NiIStream& stream) {
	BSMultiBoundData::Get(stream);

	stream >> center;
	stream >> radius;
}

void BSMultiBoundSphere::Put(NiOStream& stream) {
	BSMultiBoundData::Put(stream);

	stream << center;
	stream << radius;
}


void BSMultiBound::Get(NiIStream& stream) {
	NiObject::Get(stream);

	dataRef.Get(stream);
}

void BSMultiBound::Put(NiOStream& stream) {
	NiObject::Put(stream);

	dataRef.Put(stream);
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


void BSMultiBoundNode::Get(NiIStream& stream) {
	NiNode::Get(stream);

	multiBoundRef.Get(stream);

	if (stream.GetVersion().User() >= 12)
		stream >> cullingMode;
}

void BSMultiBoundNode::Put(NiOStream& stream) {
	NiNode::Put(stream);

	multiBoundRef.Put(stream);

	if (stream.GetVersion().User() >= 12)
		stream << cullingMode;
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


void BSRangeNode::Get(NiIStream& stream) {
	NiNode::Get(stream);

	stream >> min;
	stream >> max;
	stream >> current;
}

void BSRangeNode::Put(NiOStream& stream) {
	NiNode::Put(stream);

	stream << min;
	stream << max;
	stream << current;
}


void NiBillboardNode::Get(NiIStream& stream) {
	NiNode::Get(stream);

	stream >> billboardMode;
}

void NiBillboardNode::Put(NiOStream& stream) {
	NiNode::Put(stream);

	stream << billboardMode;
}


void NiSwitchNode::Get(NiIStream& stream) {
	NiNode::Get(stream);

	stream >> flags;
	stream >> index;
}

void NiSwitchNode::Put(NiOStream& stream) {
	NiNode::Put(stream);

	stream << flags;
	stream << index;
}


void NiRangeLODData::Get(NiIStream& stream) {
	NiLODData::Get(stream);

	stream >> lodCenter;
	stream >> numLODLevels;

	lodLevels.resize(numLODLevels);
	for (uint32_t i = 0; i < numLODLevels; i++) {
		stream >> lodLevels[i].nearExtent;
		stream >> lodLevels[i].farExtent;
	}
}

void NiRangeLODData::Put(NiOStream& stream) {
	NiLODData::Put(stream);

	stream << lodCenter;
	stream << numLODLevels;

	for (uint32_t i = 0; i < numLODLevels; i++) {
		stream << lodLevels[i].nearExtent;
		stream << lodLevels[i].farExtent;
	}
}


void NiScreenLODData::Get(NiIStream& stream) {
	NiLODData::Get(stream);

	stream >> boundCenter;
	stream >> boundRadius;
	stream >> worldCenter;
	stream >> worldRadius;
	stream >> numProportions;

	proportionLevels.resize(numProportions);
	for (uint32_t i = 0; i < numProportions; i++)
		stream >> proportionLevels[i];
}

void NiScreenLODData::Put(NiOStream& stream) {
	NiLODData::Put(stream);

	stream << boundCenter;
	stream << boundRadius;
	stream << worldCenter;
	stream << worldRadius;
	stream << numProportions;

	for (uint32_t i = 0; i < numProportions; i++)
		stream << proportionLevels[i];
}


void NiLODNode::Get(NiIStream& stream) {
	NiSwitchNode::Get(stream);

	lodLevelData.Get(stream);
}

void NiLODNode::Put(NiOStream& stream) {
	NiSwitchNode::Put(stream);

	lodLevelData.Put(stream);
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


void NiSortAdjustNode::Get(NiIStream& stream) {
	NiNode::Get(stream);

	stream >> sortingMode;
}

void NiSortAdjustNode::Put(NiOStream& stream) {
	NiNode::Put(stream);

	stream << sortingMode;
}
