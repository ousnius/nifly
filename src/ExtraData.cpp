/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#include "ExtraData.hpp"
#include "half.hpp"

#include <fstream>

using namespace nifly;

void NiExtraData::Sync(NiStreamReversible& stream) {
	name.Sync(stream);
}

void NiExtraData::GetStringRefs(std::vector<StringRef*>& refs) {
	NiObject::GetStringRefs(refs);

	refs.emplace_back(&name);
}

std::string NiExtraData::GetName() {
	return name.GetString();
}

void NiExtraData::SetName(const std::string& extraDataName) {
	name.SetString(extraDataName);
}


void NiBinaryExtraData::Sync(NiStreamReversible& stream) {
	stream.Sync(size);
	data.resize(size);
	for (uint32_t i = 0; i < size; i++)
		stream.Sync(data[i]);
}

std::vector<uint8_t> NiBinaryExtraData::GetData() {
	return data;
}

void NiBinaryExtraData::SetData(const std::vector<uint8_t>& dat) {
	size = dat.size();
	data = dat;
}


void NiFloatExtraData::Sync(NiStreamReversible& stream) {
	stream.Sync(floatData);
}

float NiFloatExtraData::GetFloatData() {
	return floatData;
}

void NiFloatExtraData::SetFloatData(const float fltData) {
	floatData = fltData;
}


void NiFloatsExtraData::Sync(NiStreamReversible& stream) {
	stream.Sync(numFloats);
	floatsData.resize(numFloats);
	for (uint32_t i = 0; i < numFloats; i++)
		stream.Sync(floatsData[i]);
}

std::vector<float> NiFloatsExtraData::GetFloatsData() {
	return floatsData;
}

void NiFloatsExtraData::SetFloatsData(const std::vector<float>& fltsData) {
	numFloats = fltsData.size();
	floatsData = fltsData;
}


void NiStringsExtraData::Sync(NiStreamReversible& stream) {
	stream.Sync(numStrings);
	stringsData.resize(numStrings);
	for (uint32_t i = 0; i < numStrings; i++)
		stringsData[i].Sync(stream, 4);
}

std::vector<NiString> NiStringsExtraData::GetStringsData() {
	return stringsData;
}

void NiStringsExtraData::SetStringsData(const std::vector<NiString>& strsData) {
	numStrings = strsData.size();
	stringsData = strsData;
}


void NiStringExtraData::Sync(NiStreamReversible& stream) {
	stringData.Sync(stream);
}

void NiStringExtraData::GetStringRefs(std::vector<StringRef*>& refs) {
	NiExtraData::GetStringRefs(refs);

	refs.emplace_back(&stringData);
}

std::string NiStringExtraData::GetStringData() {
	return stringData.GetString();
}

void NiStringExtraData::SetStringData(const std::string& str) {
	stringData.SetString(str);
}


void NiBooleanExtraData::Sync(NiStreamReversible& stream) {
	stream.Sync(booleanData);
}

bool NiBooleanExtraData::GetBooleanData() {
	return booleanData;
}

void NiBooleanExtraData::SetBooleanData(const bool boolData) {
	booleanData = boolData;
}


void NiIntegerExtraData::Sync(NiStreamReversible& stream) {
	stream.Sync(integerData);
}

uint32_t NiIntegerExtraData::GetIntegerData() {
	return integerData;
}

void NiIntegerExtraData::SetIntegerData(const uint32_t intData) {
	integerData = intData;
}


void NiIntegersExtraData::Sync(NiStreamReversible& stream) {
	stream.Sync(numIntegers);
	integersData.resize(numIntegers);
	for (uint32_t i = 0; i < numIntegers; i++)
		stream.Sync(integersData[i]);
}

std::vector<uint32_t> NiIntegersExtraData::GetIntegersData() {
	return integersData;
}

void NiIntegersExtraData::SetIntegersData(const std::vector<uint32_t>& intData) {
	numIntegers = intData.size();
	integersData = intData;
}


void NiVectorExtraData::Sync(NiStreamReversible& stream) {
	stream.Sync(vectorData);
}

Vector4 NiVectorExtraData::GetVectorData() {
	return vectorData;
}

void NiVectorExtraData::SetVectorData(const Vector4& vecData) {
	vectorData = vecData;
}


void NiColorExtraData::Sync(NiStreamReversible& stream) {
	stream.Sync(colorData);
}

Color4 NiColorExtraData::GetColorData() {
	return colorData;
}

void NiColorExtraData::SetColorData(const Color4& colData) {
	colorData = colData;
}


void BSWArray::Sync(NiStreamReversible& stream) {
	stream.Sync(numData);
	data.resize(numData);
	for (uint32_t i = 0; i < numData; i++)
		stream.Sync(data[i]);
}

std::vector<uint32_t> BSWArray::GetData() {
	return data;
}

void BSWArray::SetData(const std::vector<uint32_t>& dat) {
	numData = dat.size();
	data = dat;
}


void BSPositionData::Sync(NiStreamReversible& stream) {
	stream.Sync(numData);
	data.resize(numData);
	for (uint32_t i = 0; i < numData; i++)
		stream.Sync(data[i]);
}

std::vector<half_float::half> BSPositionData::GetData() {
	return data;
}

void BSPositionData::SetData(const std::vector<half_float::half>& dat) {
	numData = dat.size();
	data = dat;
}


void BSEyeCenterExtraData::Sync(NiStreamReversible& stream) {
	stream.Sync(numData);
	data.resize(numData);
	for (uint32_t i = 0; i < numData; i++)
		stream.Sync(data[i]);
}

std::vector<float> BSEyeCenterExtraData::GetData() {
	return data;
}

void BSEyeCenterExtraData::SetData(const std::vector<float>& dat) {
	numData = dat.size();
	data = dat;
}


void BSPackedGeomData::Sync(NiStreamReversible& stream) {
	stream.Sync(numVerts);

	stream.Sync(lodLevels);
	lod.resize(lodLevels);
	for (uint32_t i = 0; i < lodLevels; i++)
		stream.Sync(lod[i]);

	stream.Sync(numCombined);
	combined.resize(numCombined);
	for (uint32_t i = 0; i < numCombined; i++)
		stream.Sync(combined[i]);

	stream.Sync(unkInt1);
	stream.Sync(unkInt2);
}


void BSPackedCombinedSharedGeomDataExtra::Sync(NiStreamReversible& stream) {
	vertDesc.Sync(stream);
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


void BSFurnitureMarker::Sync(NiStreamReversible& stream) {
	stream.Sync(numPositions);
	positions.resize(numPositions);

	for (uint32_t i = 0; i < numPositions; i++) {
		stream.Sync(positions[i].offset);

		if (stream.GetVersion().User() <= 11) {
			stream.Sync(positions[i].orientation);
			stream.Sync(positions[i].posRef1);
			stream.Sync(positions[i].posRef2);
		}

		if (stream.GetVersion().User() >= 12) {
			stream.Sync(positions[i].heading);
			stream.Sync(positions[i].animationType);
			stream.Sync(positions[i].entryPoints);
		}
	}
}

std::vector<FurniturePosition> BSFurnitureMarker::GetPositions() {
	return positions;
}

void BSFurnitureMarker::SetPositions(const std::vector<FurniturePosition>& pos) {
	numPositions = pos.size();
	positions = pos;
}


void BSDecalPlacementVectorExtraData::Sync(NiStreamReversible& stream) {
	stream.Sync(numVectorBlocks);
	decalVectorBlocks.resize(numVectorBlocks);

	for (uint32_t i = 0; i < numVectorBlocks; i++) {
		stream.Sync(decalVectorBlocks[i].numVectors);

		decalVectorBlocks[i].points.resize(decalVectorBlocks[i].numVectors);
		for (int j = 0; j < decalVectorBlocks[i].numVectors; j++)
			stream.Sync(decalVectorBlocks[i].points[j]);

		decalVectorBlocks[i].normals.resize(decalVectorBlocks[i].numVectors);
		for (int j = 0; j < decalVectorBlocks[i].numVectors; j++)
			stream.Sync(decalVectorBlocks[i].normals[j]);
	}
}

std::vector<DecalVectorBlock> BSDecalPlacementVectorExtraData::GetDecalVectorBlocks() {
	return decalVectorBlocks;
}

void BSDecalPlacementVectorExtraData::SetDecalVectorBlocks(const std::vector<DecalVectorBlock>& vectorBlocks) {
	numVectorBlocks = vectorBlocks.size();
	decalVectorBlocks = vectorBlocks;
}


void BSBehaviorGraphExtraData::Sync(NiStreamReversible& stream) {
	behaviorGraphFile.Sync(stream);
	stream.Sync(controlsBaseSkel);
}

void BSBehaviorGraphExtraData::GetStringRefs(std::vector<StringRef*>& refs) {
	NiExtraData::GetStringRefs(refs);

	refs.emplace_back(&behaviorGraphFile);
}


void BSBound::Sync(NiStreamReversible& stream) {
	stream.Sync(center);
	stream.Sync(halfExtents);
}


void BSBoneLODExtraData::Sync(NiStreamReversible& stream) {
	stream.Sync(numBoneLODs);
	boneLODs.resize(numBoneLODs);
	for (uint32_t i = 0; i < numBoneLODs; i++) {
		stream.Sync(boneLODs[i].distance);
		boneLODs[i].boneName.Sync(stream);
	}
}

void BSBoneLODExtraData::GetStringRefs(std::vector<StringRef*>& refs) {
	NiExtraData::GetStringRefs(refs);

	for (uint32_t i = 0; i < numBoneLODs; i++)
		refs.emplace_back(&boneLODs[i].boneName);
}

std::vector<BoneLOD> BSBoneLODExtraData::GetBoneLODs() {
	return boneLODs;
}

void BSBoneLODExtraData::SetBoneLODs(const std::vector<BoneLOD>& lods) {
	numBoneLODs = lods.size();
	boneLODs = lods;
}


void NiTextKeyExtraData::Sync(NiStreamReversible& stream) {
	stream.Sync(numTextKeys);
	textKeys.resize(numTextKeys);
	for (uint32_t i = 0; i < numTextKeys; i++) {
		stream.Sync(textKeys[i].time);
		textKeys[i].value.Sync(stream);
	}
}

void NiTextKeyExtraData::GetStringRefs(std::vector<StringRef*>& refs) {
	NiExtraData::GetStringRefs(refs);

	for (uint32_t i = 0; i < numTextKeys; i++)
		refs.emplace_back(&textKeys[i].value);
}

std::vector<Key<StringRef>> NiTextKeyExtraData::GetTextKeys() {
	return textKeys;
}

void NiTextKeyExtraData::SetTextKeys(const std::vector<Key<StringRef>>& keys) {
	numTextKeys = keys.size();
	textKeys = keys;
}


void BSDistantObjectLargeRefExtraData::Sync(NiStreamReversible& stream) {
	stream.Sync(largeRef);
}


void BSConnectPoint::Sync(NiStreamReversible& stream) {
	root.Sync(stream, 4);
	variableName.Sync(stream, 4);

	stream.Sync(rotation);
	stream.Sync(translation);
	stream.Sync(scale);
}


void BSConnectPointParents::Sync(NiStreamReversible& stream) {
	stream.Sync(numConnectPoints);
	connectPoints.resize(numConnectPoints);

	for (uint32_t i = 0; i < numConnectPoints; i++)
		connectPoints[i].Sync(stream);
}

std::vector<BSConnectPoint> BSConnectPointParents::GetConnectPoints() {
	return connectPoints;
}

void BSConnectPointParents::SetConnectPoints(const std::vector<BSConnectPoint>& cps) {
	numConnectPoints = cps.size();
	connectPoints = cps;
}


void BSConnectPointChildren::Sync(NiStreamReversible& stream) {
	stream.Sync(unkByte);
	stream.Sync(numTargets);

	targets.resize(numTargets);
	for (uint32_t i = 0; i < numTargets; i++)
		targets[i].Sync(stream, 4);
}

std::vector<NiString> BSConnectPointChildren::GetTargets() {
	return targets;
}

void BSConnectPointChildren::SetTargets(const std::vector<NiString>& targ) {
	numTargets = targ.size();
	targets = targ;
}


BSClothExtraData::BSClothExtraData(const uint32_t size) {
	numBytes = size;
	data.resize(size);
}

void BSClothExtraData::Sync(NiStreamReversible& stream) {
	stream.Sync(numBytes);

	data.resize(numBytes);
	if (data.empty())
		return;

	stream.Sync(&data[0], numBytes);
}

std::vector<char> BSClothExtraData::GetData() {
	return data;
}

void BSClothExtraData::SetData(const std::vector<char>& dat) {
	numBytes = dat.size();
	data = dat;
}


bool BSClothExtraData::ToHKX(const std::string& fileName) {
	std::ofstream file(fileName, std::ios_base::binary);
	if (!file)
		return false;

	file.write(data.data(), numBytes);
	return true;
}

bool BSClothExtraData::FromHKX(const std::string& fileName) {
	std::ifstream file(fileName, std::ios::binary | std::ios::ate);
	if (!file)
		return false;

	numBytes = file.tellg();
	file.seekg(0, std::ios::beg);

	data.resize(numBytes);
	file.read(data.data(), numBytes);
	return true;
}
