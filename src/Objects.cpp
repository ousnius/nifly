/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#include "Objects.hpp"
#include "Geometry.hpp"

using namespace nifly;

void NiObjectNET::Sync(NiStreamReversible& stream) {
	if (bBSLightingShaderProperty && stream.GetVersion().User() >= 12 && stream.GetVersion().Stream() <= 130)
		stream.Sync(bslspShaderType);

	name.Sync(stream);

	extraDataRefs.Sync(stream);
	controllerRef.Sync(stream);
}

void NiObjectNET::GetStringRefs(std::vector<StringRef*>& refs) {
	NiObject::GetStringRefs(refs);

	refs.emplace_back(&name);
}

void NiObjectNET::GetChildRefs(std::set<Ref*>& refs) {
	NiObject::GetChildRefs(refs);

	extraDataRefs.GetIndexPtrs(refs);
	refs.insert(&controllerRef);
}

void NiObjectNET::GetChildIndices(std::vector<int>& indices) {
	NiObject::GetChildIndices(indices);

	extraDataRefs.GetIndices(indices);
	indices.push_back(controllerRef.GetIndex());
}

std::string NiObjectNET::GetName() {
	return name.GetString();
}

void NiObjectNET::SetName(const std::string& str) {
	name.SetString(str);
}

void NiObjectNET::ClearName() {
	name.Clear();
}

int NiObjectNET::GetControllerRef() {
	return controllerRef.GetIndex();
}

void NiObjectNET::SetControllerRef(int ctlrRef) {
	controllerRef.SetIndex(ctlrRef);
}

BlockRefArray<NiExtraData>& NiObjectNET::GetExtraData() {
	return extraDataRefs;
}


void NiAVObject::Sync(NiStreamReversible& stream) {
	if (HasType<BSTriShape>()) {
		// The order of definition for BSTriShape deviates slightly from previous versions.
		// NiObjectNET -> NiAVObject (duplicated in BSTriShape) -> BSTriShape
		return;
	}

	if (stream.GetVersion().Stream() <= 26) {
		auto flagsShort = static_cast<uint16_t>(flags);
		stream.Sync(flagsShort);
		flags = flagsShort;
	}
	else
		stream.Sync(flags);

	stream.Sync(transform.translation);
	stream.Sync(transform.rotation);
	stream.Sync(transform.scale);

	if (stream.GetVersion().Stream() <= 34)
		propertyRefs.Sync(stream);

	if (stream.GetVersion().File() >= V10_0_1_0)
		collisionRef.Sync(stream);
}

void NiAVObject::GetChildRefs(std::set<Ref*>& refs) {
	NiObjectNET::GetChildRefs(refs);

	propertyRefs.GetIndexPtrs(refs);
	refs.insert(&collisionRef);
}

void NiAVObject::GetChildIndices(std::vector<int>& indices) {
	NiObjectNET::GetChildIndices(indices);

	propertyRefs.GetIndices(indices);
	indices.push_back(collisionRef.GetIndex());
}

BlockRefArray<NiProperty>& NiAVObject::GetProperties() {
	return propertyRefs;
}


void NiDefaultAVObjectPalette::Sync(NiStreamReversible& stream) {
	sceneRef.Sync(stream);

	stream.Sync(numObjects);
	objects.resize(numObjects);
	for (uint32_t i = 0; i < numObjects; i++) {
		objects[i].name.Sync(stream, 4);
		objects[i].objectRef.Sync(stream);
	}
}

void NiDefaultAVObjectPalette::GetPtrs(std::set<Ref*>& ptrs) {
	NiAVObjectPalette::GetPtrs(ptrs);

	ptrs.insert(&sceneRef);

	for (uint32_t i = 0; i < numObjects; i++)
		ptrs.insert(&objects[i].objectRef);
}


void NiCamera::Sync(NiStreamReversible& stream) {
	stream.Sync(obsoleteFlags);
	stream.Sync(frustumLeft);
	stream.Sync(frustumRight);
	stream.Sync(frustumTop);
	stream.Sync(frustomBottom);
	stream.Sync(frustumNear);
	stream.Sync(frustumFar);
	stream.Sync(useOrtho);
	stream.Sync(viewportLeft);
	stream.Sync(viewportRight);
	stream.Sync(viewportTop);
	stream.Sync(viewportBottom);
	stream.Sync(lodAdjust);

	sceneRef.Sync(stream);
	stream.Sync(numScreenPolygons);
	stream.Sync(numScreenTextures);
}

void NiCamera::GetChildRefs(std::set<Ref*>& refs) {
	NiAVObject::GetChildRefs(refs);

	refs.insert(&sceneRef);
}

void NiCamera::GetChildIndices(std::vector<int>& indices) {
	NiAVObject::GetChildIndices(indices);

	indices.push_back(sceneRef.GetIndex());
}

int NiCamera::GetSceneRef() {
	return sceneRef.GetIndex();
}

void NiCamera::SetSceneRef(int scRef) {
	sceneRef.SetIndex(scRef);
}


void NiPalette::Sync(NiStreamReversible& stream) {
	stream.Sync(hasAlpha);

	if (stream.GetMode() == NiStreamReversible::Mode::Reading) {
		// Size can only be 16 or 256
		if (numEntries != 16 && numEntries != 256) {
			if (numEntries >= 128)
				numEntries = 256;
			else
				numEntries = 16;
		}
	}

	stream.Sync(numEntries);
	palette.resize(numEntries);
	for (uint32_t i = 0; i < numEntries; i++)
		stream.Sync(palette[i]);
}


void TextureRenderData::Sync(NiStreamReversible& stream) {
	stream.Sync(pixelFormat);
	stream.Sync(bitsPerPixel);
	stream.Sync(unkInt1);
	stream.Sync(unkInt2);
	stream.Sync(flags);
	stream.Sync(unkInt3);

	for (uint32_t i = 0; i < 4; i++) {
		stream.Sync(channels[i].type);
		stream.Sync(channels[i].convention);
		stream.Sync(channels[i].bitsPerChannel);
		stream.Sync(channels[i].unkByte1);
	}

	paletteRef.Sync(stream);

	stream.Sync(numMipmaps);
	stream.Sync(bytesPerPixel);
	mipmaps.resize(numMipmaps);
	for (uint32_t i = 0; i < numMipmaps; i++)
		stream.Sync(mipmaps[i]);
}

void TextureRenderData::GetChildRefs(std::set<Ref*>& refs) {
	NiObject::GetChildRefs(refs);

	refs.insert(&paletteRef);
}

void TextureRenderData::GetChildIndices(std::vector<int>& indices) {
	NiObject::GetChildIndices(indices);

	indices.push_back(paletteRef.GetIndex());
}

int TextureRenderData::GetPaletteRef() {
	return paletteRef.GetIndex();
}

void TextureRenderData::SetPaletteRef(int palRef) {
	paletteRef.SetIndex(palRef);
}


void NiPersistentSrcTextureRendererData::Sync(NiStreamReversible& stream) {
	stream.Sync(numPixels);
	stream.Sync(unkInt4);
	stream.Sync(numFaces);
	stream.Sync(unkInt5);

	pixelData.resize(numFaces);
	for (uint32_t f = 0; f < numFaces; f++) {
		pixelData[f].resize(numPixels);
		for (uint32_t p = 0; p < numPixels; p++)
			stream.Sync(pixelData[f][p]);
	}
}


void NiPixelData::Sync(NiStreamReversible& stream) {
	stream.Sync(numPixels);
	stream.Sync(numFaces);

	pixelData.resize(numFaces);
	for (uint32_t f = 0; f < numFaces; f++) {
		pixelData[f].resize(numPixels);
		for (uint32_t p = 0; p < numPixels; p++)
			stream.Sync(pixelData[f][p]);
	}
}


void NiSourceTexture::Sync(NiStreamReversible& stream) {
	stream.Sync(useExternal);
	fileName.Sync(stream);
	dataRef.Sync(stream);
	stream.Sync(pixelLayout);
	stream.Sync(mipMapFormat);
	stream.Sync(alphaFormat);
	stream.Sync(isStatic);

	if (stream.GetVersion().File() >= NiVersion::ToFile(10, 1, 0, 103))
		stream.Sync(directRender);
	if (stream.GetVersion().File() >= NiVersion::ToFile(20, 2, 0, 4))
		stream.Sync(persistentRenderData);
}

void NiSourceTexture::GetStringRefs(std::vector<StringRef*>& refs) {
	NiTexture::GetStringRefs(refs);

	refs.emplace_back(&fileName);
}

void NiSourceTexture::GetChildRefs(std::set<Ref*>& refs) {
	NiTexture::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void NiSourceTexture::GetChildIndices(std::vector<int>& indices) {
	NiTexture::GetChildIndices(indices);

	indices.push_back(dataRef.GetIndex());
}

int NiSourceTexture::GetDataRef() {
	return dataRef.GetIndex();
}

void NiSourceTexture::SetDataRef(int datRef) {
	dataRef.SetIndex(datRef);
}


void NiDynamicEffect::Sync(NiStreamReversible& stream) {
	if (stream.GetVersion().Stream() < 130) {
		stream.Sync(switchState);
		affectedNodes.Sync(stream);
	}
}

void NiDynamicEffect::GetChildRefs(std::set<Ref*>& refs) {
	NiAVObject::GetChildRefs(refs);

	affectedNodes.GetIndexPtrs(refs);
}

void NiDynamicEffect::GetChildIndices(std::vector<int>& indices) {
	NiAVObject::GetChildIndices(indices);

	affectedNodes.GetIndices(indices);
}

BlockRefArray<NiNode>& NiDynamicEffect::GetAffectedNodes() {
	return affectedNodes;
}


void NiTextureEffect::Sync(NiStreamReversible& stream) {
	stream.Sync(modelProjectionMatrix);
	stream.Sync(modelProjectionTranslation);
	stream.Sync(textureFiltering);
	stream.Sync(textureClamping);
	stream.Sync(textureType);
	stream.Sync(coordinateGenerationType);
	sourceTexture.Sync(stream);
	stream.Sync(clippingPlane);
	stream.Sync(unkVector);
	stream.Sync(unkFloat);
}

void NiTextureEffect::GetChildRefs(std::set<Ref*>& refs) {
	NiDynamicEffect::GetChildRefs(refs);

	refs.insert(&sourceTexture);
}

void NiTextureEffect::GetChildIndices(std::vector<int>& indices) {
	NiDynamicEffect::GetChildIndices(indices);

	indices.push_back(sourceTexture.GetIndex());
}

int NiTextureEffect::GetSourceTextureRef() {
	return sourceTexture.GetIndex();
}

void NiTextureEffect::SetSourceTextureRef(int srcTexRef) {
	sourceTexture.SetIndex(srcTexRef);
}


void NiLight::Sync(NiStreamReversible& stream) {
	stream.Sync(dimmer);
	stream.Sync(ambientColor);
	stream.Sync(diffuseColor);
	stream.Sync(specularColor);
}


void NiPointLight::Sync(NiStreamReversible& stream) {
	stream.Sync(constantAttenuation);
	stream.Sync(linearAttenuation);
	stream.Sync(quadraticAttenuation);
}


void NiSpotLight::Sync(NiStreamReversible& stream) {
	stream.Sync(cutoffAngle);
	stream.Sync(unkFloat);
	stream.Sync(exponent);
}
