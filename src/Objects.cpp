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

void NiObjectNET::GetStringRefs(std::vector<NiStringRef*>& refs) {
	NiObject::GetStringRefs(refs);

	refs.emplace_back(&name);
}

void NiObjectNET::GetChildRefs(std::set<NiRef*>& refs) {
	NiObject::GetChildRefs(refs);

	extraDataRefs.GetIndexPtrs(refs);
	refs.insert(&controllerRef);
}

void NiObjectNET::GetChildIndices(std::vector<uint32_t>& indices) {
	NiObject::GetChildIndices(indices);

	extraDataRefs.GetIndices(indices);
	indices.push_back(controllerRef.index);
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

void NiAVObject::GetChildRefs(std::set<NiRef*>& refs) {
	NiObjectNET::GetChildRefs(refs);

	propertyRefs.GetIndexPtrs(refs);
	refs.insert(&collisionRef);
}

void NiAVObject::GetChildIndices(std::vector<uint32_t>& indices) {
	NiObjectNET::GetChildIndices(indices);

	propertyRefs.GetIndices(indices);
	indices.push_back(collisionRef.index);
}


void NiDefaultAVObjectPalette::Sync(NiStreamReversible& stream) {
	sceneRef.Sync(stream);
	objects.Sync(stream);
}

void NiDefaultAVObjectPalette::GetPtrs(std::set<NiPtr*>& ptrs) {
	NiAVObjectPalette::GetPtrs(ptrs);

	ptrs.insert(&sceneRef);
	objects.GetPtrs(ptrs);
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

void NiCamera::GetChildRefs(std::set<NiRef*>& refs) {
	NiAVObject::GetChildRefs(refs);

	refs.insert(&sceneRef);
}

void NiCamera::GetChildIndices(std::vector<uint32_t>& indices) {
	NiAVObject::GetChildIndices(indices);

	indices.push_back(sceneRef.index);
}


void NiPalette::Sync(NiStreamReversible& stream) {
	stream.Sync(hasAlpha);

	if (stream.GetMode() == NiStreamReversible::Mode::Writing) {
		// Size can only be 16 or 256
		auto numEntries = palette.size();
		if (numEntries != 16 && numEntries != 256) {
			if (numEntries >= 128)
				palette.resize(256);
			else
				palette.resize(16);
		}
	}

	palette.Sync(stream);
}


void TextureRenderData::Sync(NiStreamReversible& stream) {
	stream.Sync(pixelFormat);
	stream.Sync(bitsPerPixel);
	stream.Sync(rendererHint);
	stream.Sync(extraData);
	stream.Sync(flags);
	stream.Sync(pixelTiling);

	for (auto& channel : channels) {
		stream.Sync(channel.type);
		stream.Sync(channel.convention);
		stream.Sync(channel.bitsPerChannel);
		stream.Sync(channel.isSigned);
	}

	paletteRef.Sync(stream);

	uint32_t sz = mipmaps.SyncSize(stream);
	stream.Sync(bytesPerPixel);

	mipmaps.SyncData(stream, sz);
}

void TextureRenderData::GetChildRefs(std::set<NiRef*>& refs) {
	NiObject::GetChildRefs(refs);

	refs.insert(&paletteRef);
}

void TextureRenderData::GetChildIndices(std::vector<uint32_t>& indices) {
	NiObject::GetChildIndices(indices);

	indices.push_back(paletteRef.index);
}


void NiPersistentSrcTextureRendererData::Sync(NiStreamReversible& stream) {
	stream.Sync(numPixels);
	stream.Sync(padNumPixels);
	stream.Sync(numFaces);
	stream.Sync(platform);

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
	const NiFileVersion fileVersion = stream.GetVersion().File();

	stream.Sync(useExternal);

	if (fileVersion <= NiFileVersion::V10_0_1_3)
		if (!useExternal)
			stream.Sync(useInternal);

	if (useExternal || fileVersion >= NiFileVersion::V10_1_0_0)
		fileName.Sync(stream);

	if (useExternal) {
		if (fileVersion >= NiFileVersion::V10_1_0_0)
			dataRef.Sync(stream);
	}
	else if (useInternal) {
		if (fileVersion <= NiFileVersion::V10_0_1_3)
			dataRef.Sync(stream);
	}
	else {
		if (fileVersion > NiFileVersion::V10_0_1_3)
			dataRef.Sync(stream);
	}

	stream.Sync(pixelLayout);
	stream.Sync(mipMapFormat);
	stream.Sync(alphaFormat);

	stream.Sync(isStatic);

	if (fileVersion >= NiVersion::ToFile(10, 1, 0, 103))
		stream.Sync(directRender);
	if (fileVersion >= NiVersion::ToFile(20, 2, 0, 4))
		stream.Sync(persistentRenderData);
}

void NiSourceTexture::GetStringRefs(std::vector<NiStringRef*>& refs) {
	NiTexture::GetStringRefs(refs);

	refs.emplace_back(&fileName);
}

void NiSourceTexture::GetChildRefs(std::set<NiRef*>& refs) {
	NiTexture::GetChildRefs(refs);

	refs.insert(&dataRef);
}

void NiSourceTexture::GetChildIndices(std::vector<uint32_t>& indices) {
	NiTexture::GetChildIndices(indices);

	indices.push_back(dataRef.index);
}


void NiDynamicEffect::Sync(NiStreamReversible& stream) {
	if (stream.GetVersion().Stream() < 130) {
		if (stream.GetVersion().File() > NiFileVersion::V10_1_0_101)
			stream.Sync(switchState);

		if (stream.GetVersion().File() <= NiFileVersion::V4_0_0_2 && stream.GetVersion().File() >= NiFileVersion::V10_1_0_0)
			affectedNodes.Sync(stream);
	}
}

void NiDynamicEffect::GetPtrs(std::set<NiPtr*>& ptrs) {
	NiAVObject::GetPtrs(ptrs);

	affectedNodes.GetIndexPtrs(ptrs);
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
	stream.Sync(plane);
}

void NiTextureEffect::GetChildRefs(std::set<NiRef*>& refs) {
	NiDynamicEffect::GetChildRefs(refs);

	refs.insert(&sourceTexture);
}

void NiTextureEffect::GetChildIndices(std::vector<uint32_t>& indices) {
	NiDynamicEffect::GetChildIndices(indices);

	indices.push_back(sourceTexture.index);
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
	stream.Sync(outerSpotAngle);
	stream.Sync(innerSpotAngle);
	stream.Sync(exponent);
}
