/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#include "Shaders.hpp"

using namespace nifly;

void NiShadeProperty::Sync(NiStreamReversible& stream) {
	stream.Sync(flags);
}


void NiSpecularProperty::Sync(NiStreamReversible& stream) {
	stream.Sync(flags);
}


void NiTexturingProperty::Sync(NiStreamReversible& stream) {
	const NiFileVersion fileVersion = stream.GetVersion().File();

	if (fileVersion <= NiFileVersion::V10_0_1_2)
		stream.Sync(flags);

	if (fileVersion >= NiVersion::ToFile(20, 1, 0, 2))
		stream.Sync(flags); // TexturingFlags

	if (fileVersion >= NiFileVersion::V3_3_0_13 && fileVersion <= NiFileVersion::V20_1_0_1)
		stream.Sync(applyMode);

	stream.Sync(textureCount);

	stream.Sync(hasBaseTex);
	if (hasBaseTex)
		baseTex.Sync(stream);

	stream.Sync(hasDarkTex);
	if (hasDarkTex)
		darkTex.Sync(stream);

	stream.Sync(hasDetailTex);
	if (hasDetailTex)
		detailTex.Sync(stream);

	stream.Sync(hasGlossTex);
	if (hasGlossTex)
		glossTex.Sync(stream);

	stream.Sync(hasGlowTex);
	if (hasGlowTex)
		glowTex.Sync(stream);

	if (textureCount > 5 && fileVersion >= NiFileVersion::V3_3_0_13) {
		stream.Sync(hasBumpTex);
		if (hasBumpTex) {
			bumpTex.Sync(stream);
			stream.Sync(lumaScale);
			stream.Sync(lumaOffset);
			stream.Sync(bumpMatrix);
		}
	}

	if (fileVersion >= NiFileVersion::V20_2_0_5) {
		if (textureCount > 6) {
			stream.Sync(hasNormalTex);
			if (hasNormalTex)
				normalTex.Sync(stream);
		}

		if (textureCount > 7) {
			stream.Sync(hasParallaxTex);
			if (hasParallaxTex) {
				parallaxTex.Sync(stream);
				stream.Sync(parallaxOffset);
			}
		}

		if (textureCount > 8) {
			stream.Sync(hasDecalTex0);
			if (hasDecalTex0)
				decalTex0.Sync(stream);
		}

		if (textureCount > 9) {
			stream.Sync(hasDecalTex1);
			if (hasDecalTex1)
				decalTex1.Sync(stream);
		}

		if (textureCount > 10) {
			stream.Sync(hasDecalTex2);
			if (hasDecalTex2)
				decalTex2.Sync(stream);
		}

		if (textureCount > 11) {
			stream.Sync(hasDecalTex3);
			if (hasDecalTex3)
				decalTex3.Sync(stream);
		}
	}
	else {
		if (textureCount > 6) {
			stream.Sync(hasDecalTex0);
			if (hasDecalTex0)
				decalTex0.Sync(stream);
		}

		if (textureCount > 7) {
			stream.Sync(hasDecalTex1);
			if (hasDecalTex1)
				decalTex1.Sync(stream);
		}

		if (textureCount > 8) {
			stream.Sync(hasDecalTex2);
			if (hasDecalTex2)
				decalTex2.Sync(stream);
		}

		if (textureCount > 9) {
			stream.Sync(hasDecalTex3);
			if (hasDecalTex3)
				decalTex3.Sync(stream);
		}
	}

	if (fileVersion >= NiFileVersion::V10_0_1_0)
		shaderTex.Sync(stream);
}

void NiTexturingProperty::GetChildRefs(std::set<NiRef*>& refs) {
	NiProperty::GetChildRefs(refs);

	baseTex.GetChildRefs(refs);
	darkTex.GetChildRefs(refs);
	detailTex.GetChildRefs(refs);
	glossTex.GetChildRefs(refs);
	glowTex.GetChildRefs(refs);
	bumpTex.GetChildRefs(refs);
	normalTex.GetChildRefs(refs);
	parallaxTex.GetChildRefs(refs);
	decalTex0.GetChildRefs(refs);
	decalTex1.GetChildRefs(refs);
	decalTex2.GetChildRefs(refs);
	decalTex3.GetChildRefs(refs);
	shaderTex.GetChildRefs(refs);
}

void NiTexturingProperty::GetChildIndices(std::vector<uint32_t>& indices) {
	NiProperty::GetChildIndices(indices);

	baseTex.GetChildIndices(indices);
	darkTex.GetChildIndices(indices);
	detailTex.GetChildIndices(indices);
	glossTex.GetChildIndices(indices);
	glowTex.GetChildIndices(indices);
	bumpTex.GetChildIndices(indices);
	normalTex.GetChildIndices(indices);
	parallaxTex.GetChildIndices(indices);
	decalTex0.GetChildIndices(indices);
	decalTex1.GetChildIndices(indices);
	decalTex2.GetChildIndices(indices);
	decalTex3.GetChildIndices(indices);
	shaderTex.GetChildIndices(indices);
}


void NiVertexColorProperty::Sync(NiStreamReversible& stream) {
	stream.Sync(flags);

	if (stream.GetVersion().File() <= NiFileVersion::V20_0_0_5) {
		stream.Sync(vertexMode);
		stream.Sync(lightingMode);
	}
}


void NiDitherProperty::Sync(NiStreamReversible& stream) {
	stream.Sync(flags);
}


void NiFogProperty::Sync(NiStreamReversible& stream) {
	stream.Sync(flags);
	stream.Sync(fogDepth);
	stream.Sync(fogColor);
}


void NiWireframeProperty::Sync(NiStreamReversible& stream) {
	stream.Sync(flags);
}


void NiZBufferProperty::Sync(NiStreamReversible& stream) {
	stream.Sync(flags);

	if (stream.GetVersion().File() >= V4_1_0_12 && stream.GetVersion().File() <= V20_0_0_5)
		stream.Sync(testFunction);
}


void BSShaderProperty::Sync(NiStreamReversible& stream) {
	if (stream.GetVersion().User() == 12 && stream.GetVersion().Stream() > 139) {
		std::string nameStr = stream.GetHeader().GetStringById(name.GetIndex());
		if (!nameStr.empty())
			return;
	}

	if (stream.GetVersion().User() <= 11) {
		stream.Sync(shaderFlags);
		stream.Sync(shaderType);
		stream.Sync(shaderFlags1);
		stream.Sync(shaderFlags2);
		stream.Sync(environmentMapScale);
	}
	else {
		if (stream.GetVersion().Stream() < 132) {
			stream.Sync(shaderFlags1);
			stream.Sync(shaderFlags2);
			stream.Sync(uvOffset);
			stream.Sync(uvScale);
		}
	}
}

uint32_t BSShaderProperty::GetShaderType() const {
	return shaderType;
}

void BSShaderProperty::SetShaderType(uint32_t type) {
	shaderType = static_cast<BSShaderType>(type);
}

bool BSShaderProperty::IsSkinTinted() const {
	return shaderType == SHADER_SKIN;
}

bool BSShaderProperty::IsFaceTinted() const {
	return shaderType == SHADER_SKIN;
}

bool BSShaderProperty::IsSkinned() const {
	return (shaderFlags1 & (1 << 1)) != 0;
}

void BSShaderProperty::SetSkinned(const bool enable) {
	if (enable)
		shaderFlags1 |= 1 << 1;
	else
		shaderFlags1 &= ~(1 << 1);
}

bool BSShaderProperty::IsDoubleSided() const {
	return (shaderFlags2 & (1 << 4)) != 0;
}

void BSShaderProperty::SetDoubleSided(const bool enable) {
	if (enable)
		shaderFlags2 |= 1 << 4;
	else
		shaderFlags2 &= ~(1 << 4);
}

bool BSShaderProperty::IsModelSpace() const {
	return (shaderFlags1 & (1 << 12)) != 0;
}

bool BSShaderProperty::IsEmissive() const {
	return (shaderFlags1 & (1 << 22)) != 0;
}

bool BSShaderProperty::HasSpecular() const {
	return (shaderFlags1 & (1 << 0)) != 0;
}

bool BSShaderProperty::HasVertexColors() const {
	return (shaderFlags2 & (1 << 5)) != 0;
}

void BSShaderProperty::SetVertexColors(const bool enable) {
	if (enable)
		shaderFlags2 |= 1 << 5;
	else
		shaderFlags2 &= ~(1 << 5);
}

bool BSShaderProperty::HasVertexAlpha() const {
	return (shaderFlags1 & (1 << 3)) != 0;
}

void BSShaderProperty::SetVertexAlpha(const bool enable) {
	if (enable)
		shaderFlags1 |= 1 << 3;
	else
		shaderFlags1 &= ~(1 << 3);
}

bool BSShaderProperty::HasBacklight() const {
	// Skyrim
	return (shaderFlags2 & (1 << 27)) != 0;
}

bool BSShaderProperty::HasRimlight() const {
	// Skyrim
	return (shaderFlags2 & (1 << 26)) != 0;
}

bool BSShaderProperty::HasSoftlight() const {
	// Skyrim
	return (shaderFlags2 & (1 << 25)) != 0;
}

bool BSShaderProperty::HasGlowmap() const {
	return (shaderFlags2 & (1 << 6)) != 0;
}

bool BSShaderProperty::HasGreyscaleColor() const {
	return (shaderFlags1 & (1 << 3)) != 0;
}

bool BSShaderProperty::HasEnvironmentMapping() const {
	return (shaderFlags1 & (1 << 7)) != 0;
}

void BSShaderProperty::SetEnvironmentMapping(const bool enable) {
	if (enable)
		shaderFlags1 |= 1 << 7;
	else
		shaderFlags1 &= ~(1 << 7);
}

float BSShaderProperty::GetEnvironmentMapScale() const {
	return environmentMapScale;
}

Vector2 BSShaderProperty::GetUVOffset() const {
	return uvOffset;
}

Vector2 BSShaderProperty::GetUVScale() const {
	return uvScale;
}


void TallGrassShaderProperty::Sync(NiStreamReversible& stream) {
	fileName.Sync(stream, 4);
}


void SkyShaderProperty::Sync(NiStreamReversible& stream) {
	fileName.Sync(stream, 4);
	stream.Sync(skyObjectType);
}


void TileShaderProperty::Sync(NiStreamReversible& stream) {
	fileName.Sync(stream, 4);
}


BSShaderTextureSet::BSShaderTextureSet(NiVersion& version) {
	if (version.User() == 12 && version.Stream() == 155)
		textures.resize(13);
	else if (version.User() == 12 && version.Stream() == 130)
		textures.resize(10);
	else if (version.User() == 12)
		textures.resize(9);
	else
		textures.resize(6);
}

void BSShaderTextureSet::Sync(NiStreamReversible& stream) {
	textures.Sync(stream);
}

BSLightingShaderProperty::BSLightingShaderProperty() {
	NiObjectNET::bBSLightingShaderProperty = true;

	shaderFlags1 = 0x80400203;
	shaderFlags2 = 0x00000081;
}

BSLightingShaderProperty::BSLightingShaderProperty(NiVersion& version)
	: BSLightingShaderProperty() {
	if (version.User() == 12 && version.Stream() >= 120) {
		shaderFlags1 = 0x80400203;
		shaderFlags2 = 0x00000081;
	}
	else {
		shaderFlags1 = 0x82400303;
		shaderFlags2 = 0x00008001;
	}

	if (version.User() == 12 && version.Stream() >= 120)
		glossiness = 1.0f;
	else
		glossiness = 20.0f;
}

void BSLightingShaderProperty::Sync(NiStreamReversible& stream) {
	if (stream.GetVersion().User() == 12 && stream.GetVersion().Stream() > 139) {
		std::string nameStr = stream.GetHeader().GetStringById(name.GetIndex());
		if (!nameStr.empty())
			return;
	}

	if (stream.GetVersion().Stream() > 139) {
		stream.Sync(bslspShaderType);

		// Adjust shader type to old value internally due to removed Height/Parallax enum value (3)
		if (stream.GetMode() == NiStreamReversible::Mode::Reading) {
			if (bslspShaderType > 3)
				bslspShaderType += 1;
		}
		else {
			if (bslspShaderType >= 3)
				bslspShaderType -= 1;
		}
	}

	if (stream.GetVersion().Stream() >= 132) {
		stream.Sync(numSF1);
		SF1.resize(numSF1);
	}

	if (stream.GetVersion().Stream() >= 152) {
		stream.Sync(numSF2);
		SF2.resize(numSF2);
	}

	if (stream.GetVersion().Stream() >= 132) {
		for (uint32_t i = 0; i < numSF1; i++)
			stream.Sync(SF1[i]);
	}

	if (stream.GetVersion().Stream() >= 152) {
		for (uint32_t i = 0; i < numSF2; i++)
			stream.Sync(SF2[i]);
	}

	if (stream.GetVersion().Stream() >= 132) {
		stream.Sync(uvOffset);
		stream.Sync(uvScale);
	}

	textureSetRef.Sync(stream);

	stream.Sync(emissiveColor);
	stream.Sync(emissiveMultiple);

	if (stream.GetVersion().User() == 12 && stream.GetVersion().Stream() >= 130)
		rootMaterialName.Sync(stream);

	if (stream.GetVersion().User() == 12 && stream.GetVersion().Stream() >= 172)
		stream.Sync(unkFloat);

	stream.Sync(textureClampMode);
	stream.Sync(alpha);
	stream.Sync(refractionStrength);
	stream.Sync(glossiness);
	stream.Sync(specularColor);
	stream.Sync(specularStrength);

	if (stream.GetVersion().User() <= 12 && stream.GetVersion().Stream() < 130) {
		stream.Sync(softlighting);
		stream.Sync(rimlightPower);
	}

	if (stream.GetVersion().IsFO4()) {
		stream.Sync(subsurfaceRolloff);
		stream.Sync(rimlightPower2);

		if (rimlightPower2 >= NiFloatMax && rimlightPower2 < NiFloatInf)
			stream.Sync(backlightPower);
	}

	if (stream.GetVersion().User() == 12 && stream.GetVersion().Stream() >= 130) {
		stream.Sync(grayscaleToPaletteScale);
		stream.Sync(fresnelPower);
		stream.Sync(wetnessSpecScale);
		stream.Sync(wetnessSpecPower);
		stream.Sync(wetnessMinVar);

		if (stream.GetVersion().Stream() == 130)
			stream.Sync(wetnessEnvmapScale);

		stream.Sync(wetnessFresnelPower);
		stream.Sync(wetnessMetalness);

		if (stream.GetVersion().Stream() > 130)
			stream.Sync(wetnessUnknown1);
		if (stream.GetVersion().Stream() >= 155)
			stream.Sync(wetnessUnknown2);
	}

	if (stream.GetVersion().User() == 12 && stream.GetVersion().Stream() > 139) {
		stream.Sync(lumEmittance);
		stream.Sync(exposureOffset);
		stream.Sync(finalExposureMin);
		stream.Sync(finalExposureMax);

		if (stream.GetVersion().Stream() < 172) {
			stream.Sync(doTranslucency);
			if (doTranslucency) {
				stream.Sync(subsurfaceColor);
				stream.Sync(transmissiveScale);
				stream.Sync(turbulence);
				stream.Sync(thickObject);
				stream.Sync(mixAlbedo);
			}

			stream.Sync(hasTextureArrays);

			if (hasTextureArrays) {
				stream.Sync(numTextureArrays);

				textureArrays.resize(numTextureArrays);

				for (uint32_t i = 0; i < numTextureArrays; i++)
					textureArrays[i].Sync(stream);
			}
		}
		else {
			stream.Sync(unkFloat1);
			stream.Sync(unkFloat2);
			stream.Sync(unkShort1);
		}
	}

	switch (bslspShaderType) {
		case 1:
			stream.Sync(environmentMapScale);

			if (stream.GetVersion().IsFO4()) {
				stream.Sync(useSSR);
				stream.Sync(wetnessUseSSR);
			}
			break;
		case 5:
			stream.Sync(skinTintColor);

			if (stream.GetVersion().User() == 12 && stream.GetVersion().Stream() >= 130)
				stream.Sync(skinTintAlpha);
			break;
		case 6:
			stream.Sync(hairTintColor);
			break;
		case 7:
			stream.Sync(maxPasses);
			stream.Sync(scale);
			break;
		case 11:
			stream.Sync(parallaxInnerLayerThickness);
			stream.Sync(parallaxRefractionScale);
			stream.Sync(parallaxInnerLayerTextureScale);
			stream.Sync(parallaxEnvmapStrength);
			break;
		case 14: stream.Sync(sparkleParameters); break;
		case 16:
			stream.Sync(eyeCubemapScale);
			stream.Sync(eyeLeftReflectionCenter);
			stream.Sync(eyeRightReflectionCenter);
			break;
	}
}

void BSLightingShaderProperty::GetStringRefs(std::vector<NiStringRef*>& refs) {
	BSShaderProperty::GetStringRefs(refs);

	refs.emplace_back(&rootMaterialName);
}

void BSLightingShaderProperty::GetChildRefs(std::set<NiRef*>& refs) {
	BSShaderProperty::GetChildRefs(refs);

	refs.insert(&textureSetRef);
}

void BSLightingShaderProperty::GetChildIndices(std::vector<uint32_t>& indices) {
	BSShaderProperty::GetChildIndices(indices);

	indices.push_back(textureSetRef.index);
}

bool BSLightingShaderProperty::IsSkinTinted() const {
	return bslspShaderType == BSLSP_SKINTINT;
}

bool BSLightingShaderProperty::IsFaceTinted() const {
	return bslspShaderType == BSLSP_FACE;
}

bool BSLightingShaderProperty::HasGlowmap() const {
	return bslspShaderType == BSLSP_GLOWMAP && BSShaderProperty::HasGlowmap();
}

bool BSLightingShaderProperty::HasEnvironmentMapping() const {
	return bslspShaderType == BSLSP_ENVMAP && BSShaderProperty::HasEnvironmentMapping();
}

uint32_t BSLightingShaderProperty::GetShaderType() const {
	return bslspShaderType;
}

void BSLightingShaderProperty::SetShaderType(const uint32_t type) {
	bslspShaderType = type;
}

Vector3 BSLightingShaderProperty::GetSpecularColor() const {
	return specularColor;
}

void BSLightingShaderProperty::SetSpecularColor(const Vector3& color) {
	specularColor = color;
}

float BSLightingShaderProperty::GetSpecularStrength() const {
	return specularStrength;
}

void BSLightingShaderProperty::SetSpecularStrength(const float strength) {
	specularStrength = strength;
}

float BSLightingShaderProperty::GetGlossiness() const {
	return glossiness;
}

void BSLightingShaderProperty::SetGlossiness(const float gloss) {
	glossiness = gloss;
}

Color4 BSLightingShaderProperty::GetEmissiveColor() const {
	Color4 color;
	color.r = emissiveColor.x;
	color.g = emissiveColor.y;
	color.b = emissiveColor.z;
	return color;
}

void BSLightingShaderProperty::SetEmissiveColor(const Color4& color) {
	emissiveColor.x = color.r;
	emissiveColor.y = color.g;
	emissiveColor.z = color.b;
}

float BSLightingShaderProperty::GetEmissiveMultiple() const {
	return emissiveMultiple;
}

void BSLightingShaderProperty::SetEmissiveMultiple(const float emissive) {
	emissiveMultiple = emissive;
}

float BSLightingShaderProperty::GetAlpha() const {
	return alpha;
}

void BSLightingShaderProperty::SetAlpha(const float alphaValue) {
	alpha = alphaValue;
}

float BSLightingShaderProperty::GetBacklightPower() const {
	return backlightPower;
}

float BSLightingShaderProperty::GetRimlightPower() const {
	return rimlightPower;
}

float BSLightingShaderProperty::GetSoftlight() const {
	return softlighting;
}

float BSLightingShaderProperty::GetSubsurfaceRolloff() const {
	return subsurfaceRolloff;
}

float BSLightingShaderProperty::GetGrayscaleToPaletteScale() const {
	return grayscaleToPaletteScale;
}

float BSLightingShaderProperty::GetFresnelPower() const {
	return fresnelPower;
}

std::string BSLightingShaderProperty::GetWetMaterialName() const {
	return rootMaterialName.get();
}

void BSLightingShaderProperty::SetWetMaterialName(const std::string& matName) {
	rootMaterialName.get() = matName;
}


void BSEffectShaderProperty::Sync(NiStreamReversible& stream) {
	if (stream.GetVersion().User() == 12 && stream.GetVersion().Stream() > 130) {
		std::string nameStr = stream.GetHeader().GetStringById(name.GetIndex());
		if (!nameStr.empty())
			return;
	}

	if (stream.GetVersion().Stream() >= 132) {
		stream.Sync(numSF1);
		SF1.resize(numSF1);
	}

	if (stream.GetVersion().Stream() >= 152) {
		stream.Sync(numSF2);
		SF2.resize(numSF2);
	}

	if (stream.GetVersion().Stream() >= 132) {
		for (uint32_t i = 0; i < numSF1; i++)
			stream.Sync(SF1[i]);
	}

	if (stream.GetVersion().Stream() >= 152) {
		for (uint32_t i = 0; i < numSF2; i++)
			stream.Sync(SF2[i]);
	}

	if (stream.GetVersion().Stream() >= 132) {
		stream.Sync(uvOffset);
		stream.Sync(uvScale);
	}

	sourceTexture.Sync(stream, 4);

	if (stream.GetVersion().Stream() >= 172)
		stream.Sync(unkFloat);

	stream.Sync(textureClampMode);

	stream.Sync(falloffStartAngle);
	stream.Sync(falloffStopAngle);
	stream.Sync(falloffStartOpacity);
	stream.Sync(falloffStopOpacity);

	if (stream.GetVersion().User() == 12 && stream.GetVersion().Stream() > 139 && stream.GetVersion().Stream() < 172)
		stream.Sync(refractionPower);

	stream.Sync(baseColor);
	stream.Sync(baseColorScale);
	stream.Sync(softFalloffDepth);
	greyscaleTexture.Sync(stream, 4);

	if (stream.GetVersion().User() == 12 && stream.GetVersion().Stream() >= 130) {
		envMapTexture.Sync(stream, 4);
		normalTexture.Sync(stream, 4);
		envMaskTexture.Sync(stream, 4);
		stream.Sync(envMapScale);
	}

	if (stream.GetVersion().User() == 12 && stream.GetVersion().Stream() > 139) {
		reflectanceTexture.Sync(stream, 4);
		lightingTexture.Sync(stream, 4);
		stream.Sync(emittanceColor);
		emitGradientTexture.Sync(stream, 4);

		stream.Sync(lumEmittance);
		stream.Sync(exposureOffset);
		stream.Sync(finalExposureMin);
		stream.Sync(finalExposureMax);
	}

	if (stream.GetVersion().User() == 12 && stream.GetVersion().Stream() >= 172) {
		for (uint8_t& b : unkBytes)
			stream.Sync(b);

		for (float& f : unkFloats)
			stream.Sync(f);

		stream.Sync(unkByte1);
	}
}

float BSEffectShaderProperty::GetEnvironmentMapScale() const {
	return envMapScale;
}

Color4 BSEffectShaderProperty::GetEmissiveColor() const {
	return baseColor;
}

void BSEffectShaderProperty::SetEmissiveColor(const Color4& color) {
	baseColor = color;
}

float BSEffectShaderProperty::GetEmissiveMultiple() const {
	return baseColorScale;
}

void BSEffectShaderProperty::SetEmissiveMultiple(const float emissive) {
	baseColorScale = emissive;
}


void BSWaterShaderProperty::Sync(NiStreamReversible& stream) {
	if (stream.GetVersion().User() == 12 && stream.GetVersion().Stream() > 139) {
		std::string nameStr = stream.GetHeader().GetStringById(name.GetIndex());
		if (!nameStr.empty())
			return;
	}

	if (stream.GetVersion().Stream() >= 132) {
		stream.Sync(numSF1);
		SF1.resize(numSF1);
	}

	if (stream.GetVersion().Stream() >= 152) {
		stream.Sync(numSF2);
		SF2.resize(numSF2);
	}

	if (stream.GetVersion().Stream() >= 132) {
		for (uint32_t i = 0; i < numSF1; i++)
			stream.Sync(SF1[i]);
	}

	if (stream.GetVersion().Stream() >= 152) {
		for (uint32_t i = 0; i < numSF2; i++)
			stream.Sync(SF2[i]);
	}

	if (stream.GetVersion().Stream() >= 132) {
		stream.Sync(uvOffset);
		stream.Sync(uvScale);
	}

	stream.Sync(waterFlags);
}


void BSSkyShaderProperty::Sync(NiStreamReversible& stream) {
	if (stream.GetVersion().User() == 12 && stream.GetVersion().Stream() > 139) {
		std::string nameStr = stream.GetHeader().GetStringById(name.GetIndex());
		if (!nameStr.empty())
			return;
	}

	if (stream.GetVersion().Stream() >= 132) {
		stream.Sync(numSF1);
		SF1.resize(numSF1);
	}

	if (stream.GetVersion().Stream() >= 152) {
		stream.Sync(numSF2);
		SF2.resize(numSF2);
	}

	if (stream.GetVersion().Stream() >= 132) {
		for (uint32_t i = 0; i < numSF1; i++)
			stream.Sync(SF1[i]);
	}

	if (stream.GetVersion().Stream() >= 152) {
		for (uint32_t i = 0; i < numSF2; i++)
			stream.Sync(SF2[i]);
	}

	if (stream.GetVersion().Stream() >= 132) {
		stream.Sync(uvOffset);
		stream.Sync(uvScale);
	}

	baseTexture.Sync(stream, 4);
	stream.Sync(skyFlags);
}


void BSShaderLightingProperty::Sync(NiStreamReversible& stream) {
	if (stream.GetVersion().User() <= 11)
		stream.Sync(textureClampMode);
}


void BSShaderPPLightingProperty::Sync(NiStreamReversible& stream) {
	textureSetRef.Sync(stream);

	if (stream.GetVersion().User() == 11 && stream.GetVersion().Stream() > 14) {
		stream.Sync(refractionStrength);
		stream.Sync(refractionFirePeriod);
	}

	if (stream.GetVersion().User() == 11 && stream.GetVersion().Stream() > 24) {
		stream.Sync(parallaxMaxPasses);
		stream.Sync(parallaxScale);
	}

	if (stream.GetVersion().User() >= 12)
		stream.Sync(emissiveColor);
}

void BSShaderPPLightingProperty::GetChildRefs(std::set<NiRef*>& refs) {
	BSShaderLightingProperty::GetChildRefs(refs);

	refs.insert(&textureSetRef);
}

void BSShaderPPLightingProperty::GetChildIndices(std::vector<uint32_t>& indices) {
	BSShaderLightingProperty::GetChildIndices(indices);

	indices.push_back(textureSetRef.index);
}

bool BSShaderPPLightingProperty::IsSkinned() const {
	return (shaderFlags1 & (1 << 1)) != 0;
}

void BSShaderPPLightingProperty::SetSkinned(const bool enable) {
	if (enable)
		shaderFlags1 |= 1 << 1;
	else
		shaderFlags1 &= ~(1 << 1);
}


void BSShaderNoLightingProperty::Sync(NiStreamReversible& stream) {
	baseTexture.Sync(stream, 4);

	if (stream.GetVersion().Stream() > 26) {
		stream.Sync(falloffStartAngle);
		stream.Sync(falloffStopAngle);
		stream.Sync(falloffStartOpacity);
		stream.Sync(falloffStopOpacity);
	}
}

bool BSShaderNoLightingProperty::IsSkinned() const {
	return (shaderFlags1 & (1 << 1)) != 0;
}

void BSShaderNoLightingProperty::SetSkinned(const bool enable) {
	if (enable)
		shaderFlags1 |= 1 << 1;
	else
		shaderFlags1 &= ~(1 << 1);
}


void NiAlphaProperty::Sync(NiStreamReversible& stream) {
	stream.Sync(flags);
	stream.Sync(threshold);
}


void NiMaterialProperty::Sync(NiStreamReversible& stream) {
	const NiFileVersion fileVersion = stream.GetVersion().File();

	if (fileVersion >= NiFileVersion::V3_0 && fileVersion <= NiFileVersion::V10_0_1_2)
		stream.Sync(legacyFlags);

	if (stream.GetVersion().Stream() < 26) {
		stream.Sync(colorAmbient);
		stream.Sync(colorDiffuse);
	}

	stream.Sync(colorSpecular);
	stream.Sync(colorEmissive);
	stream.Sync(glossiness);
	stream.Sync(alpha);

	if (stream.GetVersion().Stream() > 21)
		stream.Sync(emitMulti);
}

bool NiMaterialProperty::IsEmissive() const {
	return !colorEmissive.IsZero();
}

bool NiMaterialProperty::HasSpecular() const {
	return !colorSpecular.IsZero();
}

void NiMaterialProperty::SetSpecularColor(const Vector3& color) {
	colorSpecular = color;
}

Vector3 NiMaterialProperty::GetSpecularColor() const {
	return colorSpecular;
}

float NiMaterialProperty::GetGlossiness() const {
	return glossiness;
}

void NiMaterialProperty::SetGlossiness(const float gloss) {
	glossiness = gloss;
}

Color4 NiMaterialProperty::GetEmissiveColor() const {
	Color4 color;
	color.r = colorEmissive.x;
	color.g = colorEmissive.y;
	color.b = colorEmissive.z;
	return color;
}

void NiMaterialProperty::SetEmissiveColor(const Color4& color) {
	colorEmissive.x = color.r;
	colorEmissive.y = color.g;
	colorEmissive.z = color.b;
}

float NiMaterialProperty::GetEmissiveMultiple() const {
	return emitMulti;
}

void NiMaterialProperty::SetEmissiveMultiple(const float emissive) {
	emitMulti = emissive;
}

float NiMaterialProperty::GetAlpha() const {
	return alpha;
}

void NiMaterialProperty::SetAlpha(const float alphaValue) {
	alpha = alphaValue;
}


void NiStencilProperty::Sync(NiStreamReversible& stream) {
	const NiFileVersion fileVersion = stream.GetVersion().File();

	if (fileVersion >= NiFileVersion::V3_0 && fileVersion <= NiFileVersion::V10_0_1_2)
		stream.Sync(legacyFlags);

	if (fileVersion <= NiFileVersion::V20_0_0_5) {
		stream.Sync(stencilEnabled);
		stream.Sync(stencilFunction);
		stream.Sync(stencilRef);
		stream.Sync(stencilMask);
		stream.Sync(failAction);
		stream.Sync(zFailAction);
		stream.Sync(passAction);
		stream.Sync(drawMode);
	}
	else if (fileVersion >= NiFileVersion::V20_1_0_3) {
		stream.Sync(flags);
		stream.Sync(stencilRef);
		stream.Sync(stencilMask);
	}
}
