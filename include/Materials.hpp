/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#pragma once

#include "BasicTypes.hpp"

namespace nifly {
class BgmString {
private:
	std::string str;

public:
	BgmString() = default;
	BgmString(const std::string& s)
		: str(s) {}

	std::string& get() { return str; }
	const std::string& get() const { return str; }

	size_t length() const { return str.length(); }

	void clear() { str.clear(); }

	void Sync(BgmStreamReversible& stream) {
		if (auto istream = stream.asRead()) {
			uint32_t size;
			*istream >> size;

			std::vector<char> buffer(size, 0);
			istream->read(buffer.data(), size);
			str = std::string(buffer.data(), size - 1);
		}
		else if (auto ostream = stream.asWrite()) {
			const auto size = static_cast<uint32_t>(str.length()) + 1;
			*ostream << size;
			ostream->write(str.c_str(), size);
		}
	}

	bool operator==(const BgmString& rhs) const { return str == rhs.str; }
	bool operator!=(const BgmString& rhs) const { return !operator==(rhs); }

	bool operator==(const std::string& rhs) const { return str == rhs; }
	bool operator!=(const std::string& rhs) const { return !operator==(rhs); }
};

class BgMaterial : public BgmCloneableStreamable<BgMaterial, BgmObject> {
public:
	~BgMaterial() override = default;

	bool tileU = false;
	bool tileV = false;
	Vector2 uvOffset = {0.0f, 0.0f};
	Vector2 uvScale = {1.0f, 1.0f};

	float alpha = 1.0f;
	bool alphaBlend = false;
	uint32_t alphaSrc = 0;
	uint32_t alphaDst = 0;
	uint8_t alphaTestRef = 255;
	bool alphaTest = false;

	bool zBufferWrite = true;
	bool zBufferTest = true;

	bool useSSR = false;
	bool wetnessUseSSR = false;
	bool decal = false;
	bool doubleSided = false;
	bool decalNoFade = false;
	bool nonOccluder = false;

	bool useRefraction = false;
	bool refractionFalloff = false;
	float refractionStrength = 0.0f;

	bool environmentMapping = false;
	float envMapScale = 1.0f;

	bool grayscaleToPaletteColor = 1.0f;
	uint8_t maskWrites = 63;
	Vector3 emissiveColor;
	bool glowmap = false;
	float lumEmittance = 100.0f;

	float adaptiveEmissiveExposureOffset = 13.5f;
	float adaptiveEmissiveFinalExposureMin = 2.0f;
	float adaptiveEmissiveFinalExposureMax = 3.0f;

	std::vector<BgmString> textures;

	void Sync(BgmStreamReversible& stream);

protected:
	void SyncTextures(BgmStreamReversible& stream, size_t count);
};

class BgShaderMaterial : public BgmCloneableStreamable<BgShaderMaterial, BgMaterial> {
public:
	~BgShaderMaterial() override = default;

	bool enableEditorAlphaRef = false;

	bool useRimLighting = false;
	float rimlightPower = 0.0f;
	float backlightPower = 0.0f;

	bool useSubsurfaceLighting = false;
	float subsurfaceRolloff = 0.0f;

	bool useSpecular = true;
	Vector3 specular;
	Vector3 specularColor = {1.0f, 1.0f, 1.0f};
	float specularStrength = 0.0f;

	float glossiness = 0.0f;
	float fresnelPower = 0.0f;

	float wetnessSpecScale = 0.0f;
	float wetnessSpecPower = 0.0f;
	float wetnessMinVar = 0.0f;
	float wetnessEnvmapScale = 0.0f;
	float wetnessFresnelPower = 0.0f;
	float wetnessMetalness = 0.0f;

	BgmString rootMaterialName;
	bool anisoLighting = false;
	bool emmissive = false;
	float emissiveMultiple = 0.0f;
	bool modelSpaceNormals = false;
	bool externalEmittance = false;
	bool backLighting = false;
	bool receiveShadows = true;
	bool hideSecret = false;
	bool castShadows = true;
	bool dissolveFade = false;
	bool assumeShadowmask = false;

	bool environmentMappingWindow = false;
	bool environmentMappingEye = false;

	bool isHair = false;
	Vector3 hairColor;
	Vector3 hairTintColor;

	bool isTree = false;
	bool isFace = false;
	bool isSkinTint = false;
	bool tessellate = false;

	float displacementTextureBias = 0.0f;
	float displacementTextureScale = 0.0f;

	float tessellationPnScale = 0.0f;
	float tessellationBaseFactor = 0.0f;
	float tessellationFadeDistance = 0.0f;

	float grayscaleToPaletteScale = 0.0f;
	bool skewSpecularAlpha = false;
	bool isPbr = false;

	bool useTranslucency = false;
	bool translucencyThickObject = false;
	bool translucencyMixAlbedoWithSubsurfaceColor = false;
	Vector3 translucencySubsurfaceColor;
	float translucencyTransmissiveScale = 0.0f;
	float translucencyTurbulence = 0.0f;

	bool useCustomPorosity = false;
	float porosityValue = 0.0f;

	bool useAdaptiveEmissive = false;

	bool isTerrain = false;
	float terrainThresholdFalloff = 0.0f;
	float terrainTilingDistance = 0.0f;
	float terrainRotationAngle = 0.0f;

	void Sync(BgmStreamReversible& stream);
};

class BgEffectMaterial : public BgmCloneableStreamable<BgEffectMaterial, BgMaterial> {
public:
	~BgEffectMaterial() override = default;

	bool bloodEnabled = false;
	bool effectLightingEnabled = false;

	bool falloffEnabled = false;
	bool falloffColorEnabled = false;

	bool grayscaleToPaletteAlpha = false;
	bool softEnabled = false;

	Vector3 baseColor = {1.0f, 1.0f, 1.0f};
	float baseColorScale = 1.0f;

	float falloffStartAngle = 1.0f;
	float falloffStopAngle = 0.0f;
	float falloffStartOpacity = 1.0f;
	float falloffStopOpacity = 0.0f;

	float lightingInfluence = 1.0f;
	bool envmapMinLOD = false;
	float softDepth = 100.0f;
	bool effectPbrSpecular = false;

	void Sync(BgmStreamReversible& stream);
};
} // namespace nifly
