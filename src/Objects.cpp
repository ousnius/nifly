/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#include "Objects.hpp"

using namespace nifly;

void NiObjectNET::Get(NiIStream& stream) {
	NiObject::Get(stream);

	if (bBSLightingShaderProperty && stream.GetVersion().User() >= 12 && stream.GetVersion().Stream() <= 130)
		stream >> bslspShaderType;

	name.Get(stream);

	extraDataRefs.Get(stream);
	controllerRef.Get(stream);
}

void NiObjectNET::Put(NiOStream& stream) {
	NiObject::Put(stream);

	if (bBSLightingShaderProperty && stream.GetVersion().User() >= 12 && stream.GetVersion().Stream() <= 130)
		stream << bslspShaderType;

	name.Put(stream);

	extraDataRefs.Put(stream);
	controllerRef.Put(stream);
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


void NiAVObject::Get(NiIStream& stream) {
	NiObjectNET::Get(stream);

	flags = 0;
	if (stream.GetVersion().Stream() <= 26)
		stream.read(reinterpret_cast<char*>(&flags), 2);
	else
		stream >> flags;

	stream >> transform.translation;
	stream >> transform.rotation;
	stream >> transform.scale;

	if (stream.GetVersion().Stream() <= 34)
		propertyRefs.Get(stream);

	if (stream.GetVersion().File() >= V10_0_1_0)
		collisionRef.Get(stream);
}

void NiAVObject::Put(NiOStream& stream) {
	NiObjectNET::Put(stream);

	if (stream.GetVersion().Stream() <= 26)
		stream.write(reinterpret_cast<char*>(&flags), 2);
	else
		stream << flags;

	stream << transform.translation;
	stream << transform.rotation;
	stream << transform.scale;

	if (stream.GetVersion().Stream() <= 34)
		propertyRefs.Put(stream);

	if (stream.GetVersion().File() >= V10_0_1_0)
		collisionRef.Put(stream);
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


void NiDefaultAVObjectPalette::Get(NiIStream& stream) {
	NiAVObjectPalette::Get(stream);

	sceneRef.Get(stream);

	stream >> numObjects;
	objects.resize(numObjects);
	for (uint32_t i = 0; i < numObjects; i++) {
		objects[i].name.Get(stream, 4);
		objects[i].objectRef.Get(stream);
	}
}

void NiDefaultAVObjectPalette::Put(NiOStream& stream) {
	NiAVObjectPalette::Put(stream);

	sceneRef.Put(stream);

	stream << numObjects;
	for (uint32_t i = 0; i < numObjects; i++) {
		objects[i].name.Put(stream, 4, false);
		objects[i].objectRef.Put(stream);
	}
}

void NiDefaultAVObjectPalette::GetPtrs(std::set<Ref*>& ptrs) {
	NiAVObjectPalette::GetPtrs(ptrs);

	ptrs.insert(&sceneRef);

	for (uint32_t i = 0; i < numObjects; i++)
		ptrs.insert(&objects[i].objectRef);
}


void NiCamera::Get(NiIStream& stream) {
	NiAVObject::Get(stream);

	stream >> obsoleteFlags;
	stream >> frustumLeft;
	stream >> frustumRight;
	stream >> frustumTop;
	stream >> frustomBottom;
	stream >> frustumNear;
	stream >> frustumFar;
	stream >> useOrtho;
	stream >> viewportLeft;
	stream >> viewportRight;
	stream >> viewportTop;
	stream >> viewportBottom;
	stream >> lodAdjust;

	sceneRef.Get(stream);
	stream >> numScreenPolygons;
	stream >> numScreenTextures;
}

void NiCamera::Put(NiOStream& stream) {
	NiAVObject::Put(stream);

	stream << obsoleteFlags;
	stream << frustumLeft;
	stream << frustumRight;
	stream << frustumTop;
	stream << frustomBottom;
	stream << frustumNear;
	stream << frustumFar;
	stream << useOrtho;
	stream << viewportLeft;
	stream << viewportRight;
	stream << viewportTop;
	stream << viewportBottom;
	stream << lodAdjust;

	sceneRef.Put(stream);
	stream << numScreenPolygons;
	stream << numScreenTextures;
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


void NiPalette::Get(NiIStream& stream) {
	NiObject::Get(stream);

	stream >> hasAlpha;
	stream >> numEntries;
	palette.resize(numEntries);
	for (uint32_t i = 0; i < numEntries; i++)
		stream >> palette[i];
}

void NiPalette::Put(NiOStream& stream) {
	NiObject::Put(stream);

	// Size can only be 16 or 256
	if (numEntries != 16 && numEntries != 256) {
		if (numEntries >= 128)
			numEntries = 256;
		else
			numEntries = 16;

		palette.resize(numEntries);
	}

	stream << hasAlpha;
	stream << numEntries;
	for (uint32_t i = 0; i < numEntries; i++)
		stream << palette[i];
}


void TextureRenderData::Get(NiIStream& stream) {
	NiObject::Get(stream);

	stream >> pixelFormat;
	stream >> bitsPerPixel;
	stream >> unkInt1;
	stream >> unkInt2;
	stream >> flags;
	stream >> unkInt3;

	for (uint32_t i = 0; i < 4; i++) {
		stream >> channels[i].type;
		stream >> channels[i].convention;
		stream >> channels[i].bitsPerChannel;
		stream >> channels[i].unkByte1;
	}

	paletteRef.Get(stream);

	stream >> numMipmaps;
	stream >> bytesPerPixel;
	mipmaps.resize(numMipmaps);
	for (uint32_t i = 0; i < numMipmaps; i++)
		stream >> mipmaps[i];
}

void TextureRenderData::Put(NiOStream& stream) {
	NiObject::Put(stream);

	stream << pixelFormat;
	stream << bitsPerPixel;
	stream << unkInt1;
	stream << unkInt2;
	stream << flags;
	stream << unkInt3;

	for (uint32_t i = 0; i < 4; i++) {
		stream << channels[i].type;
		stream << channels[i].convention;
		stream << channels[i].bitsPerChannel;
		stream << channels[i].unkByte1;
	}

	paletteRef.Put(stream);

	stream << numMipmaps;
	stream << bytesPerPixel;
	for (uint32_t i = 0; i < numMipmaps; i++)
		stream << mipmaps[i];
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


void NiPersistentSrcTextureRendererData::Get(NiIStream& stream) {
	TextureRenderData::Get(stream);

	stream >> numPixels;
	stream >> unkInt4;
	stream >> numFaces;
	stream >> unkInt5;

	pixelData.resize(numFaces);
	for (uint32_t f = 0; f < numFaces; f++) {
		pixelData[f].resize(numPixels);
		for (uint32_t p = 0; p < numPixels; p++)
			stream >> pixelData[f][p];
	}
}

void NiPersistentSrcTextureRendererData::Put(NiOStream& stream) {
	TextureRenderData::Put(stream);

	stream << numPixels;
	stream << unkInt4;
	stream << numFaces;
	stream << unkInt5;

	for (uint32_t f = 0; f < numFaces; f++)
		for (uint32_t p = 0; p < numPixels; p++)
			stream << pixelData[f][p];
}


void NiPixelData::Get(NiIStream& stream) {
	TextureRenderData::Get(stream);

	stream >> numPixels;
	stream >> numFaces;

	pixelData.resize(numFaces);
	for (uint32_t f = 0; f < numFaces; f++) {
		pixelData[f].resize(numPixels);
		for (uint32_t p = 0; p < numPixels; p++)
			stream >> pixelData[f][p];
	}
}

void NiPixelData::Put(NiOStream& stream) {
	TextureRenderData::Put(stream);

	stream << numPixels;
	stream << numFaces;

	for (uint32_t f = 0; f < numFaces; f++)
		for (uint32_t p = 0; p < numPixels; p++)
			stream << pixelData[f][p];
}


void NiSourceTexture::Get(NiIStream& stream) {
	NiTexture::Get(stream);

	stream >> useExternal;
	fileName.Get(stream);
	dataRef.Get(stream);
	stream >> pixelLayout;
	stream >> mipMapFormat;
	stream >> alphaFormat;
	stream >> isStatic;

	if (stream.GetVersion().File() >= NiVersion::ToFile(10, 1, 0, 103))
		stream >> directRender;
	if (stream.GetVersion().File() >= NiVersion::ToFile(20, 2, 0, 4))
		stream >> persistentRenderData;
}

void NiSourceTexture::Put(NiOStream& stream) {
	NiTexture::Put(stream);

	stream << useExternal;
	fileName.Put(stream);
	dataRef.Put(stream);
	stream << pixelLayout;
	stream << mipMapFormat;
	stream << alphaFormat;
	stream << isStatic;

	if (stream.GetVersion().File() >= NiVersion::ToFile(10, 1, 0, 103))
		stream << directRender;
	if (stream.GetVersion().File() >= NiVersion::ToFile(20, 2, 0, 4))
		stream << persistentRenderData;
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


void NiDynamicEffect::Get(NiIStream& stream) {
	NiAVObject::Get(stream);

	if (stream.GetVersion().Stream() < 130) {
		stream >> switchState;
		affectedNodes.Get(stream);
	}
}

void NiDynamicEffect::Put(NiOStream& stream) {
	NiAVObject::Put(stream);

	if (stream.GetVersion().Stream() < 130) {
		stream << switchState;
		affectedNodes.Put(stream);
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


void NiTextureEffect::Get(NiIStream& stream) {
	NiDynamicEffect::Get(stream);

	stream >> modelProjectionMatrix;
	stream >> modelProjectionTranslation;
	stream >> textureFiltering;
	stream >> textureClamping;
	stream >> textureType;
	stream >> coordinateGenerationType;
	sourceTexture.Get(stream);
	stream >> clippingPlane;
	stream >> unkVector;
	stream >> unkFloat;
}

void NiTextureEffect::Put(NiOStream& stream) {
	NiDynamicEffect::Put(stream);

	stream << modelProjectionMatrix;
	stream << modelProjectionTranslation;
	stream << textureFiltering;
	stream << textureClamping;
	stream << textureType;
	stream << coordinateGenerationType;
	sourceTexture.Put(stream);
	stream << clippingPlane;
	stream << unkVector;
	stream << unkFloat;
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


void NiLight::Get(NiIStream& stream) {
	NiDynamicEffect::Get(stream);

	stream >> dimmer;
	stream >> ambientColor;
	stream >> diffuseColor;
	stream >> specularColor;
}

void NiLight::Put(NiOStream& stream) {
	NiDynamicEffect::Put(stream);

	stream << dimmer;
	stream << ambientColor;
	stream << diffuseColor;
	stream << specularColor;
}


void NiPointLight::Get(NiIStream& stream) {
	NiLight::Get(stream);

	stream >> constantAttenuation;
	stream >> linearAttenuation;
	stream >> quadraticAttenuation;
}

void NiPointLight::Put(NiOStream& stream) {
	NiLight::Put(stream);

	stream << constantAttenuation;
	stream << linearAttenuation;
	stream << quadraticAttenuation;
}


void NiSpotLight::Get(NiIStream& stream) {
	NiPointLight::Get(stream);

	stream >> cutoffAngle;
	stream >> unkFloat;
	stream >> exponent;
}

void NiSpotLight::Put(NiOStream& stream) {
	NiPointLight::Put(stream);

	stream << cutoffAngle;
	stream << unkFloat;
	stream << exponent;
}
