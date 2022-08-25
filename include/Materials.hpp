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
			const uint32_t size(str.length() + 1);
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
	virtual ~BgMaterial() = default;

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

	bool screenSpaceReflections = false;
	bool wetnessControlScreenSpaceReflections = false;
	bool decal = false;
	bool twoSided = false;
	bool decalNoFade = false;
	bool nonOccluder = false;

	bool refraction = false;
	bool refractionFalloff = false;
	float refractionPower = 0.0f;

	bool environmentMapping = false;
	float environmentMappingMaskScale = 1.0f;

	bool grayscaleToPaletteColor = 1.0f;
	uint8_t maskWrites = 63;
	Color3 emittanceColor;
	bool glowmap = false;
	float lumEmittance = 100.0f;

	float adaptativeEmissiveExposureOffset = 13.5f;
	float adaptativeEmissiveFinalExposureMin = 2.0f;
	float adaptativeEmissiveFinalExposureMax = 3.0f;

	std::vector<BgmString> textures;

	virtual void Sync(BgmStreamReversible& stream);

protected:
	void SyncTextures(BgmStreamReversible& stream, size_t count);
};

class BgShaderMaterial : public BgmCloneableStreamable<BgShaderMaterial, BgMaterial> {
public:
	virtual ~BgShaderMaterial() = default;

	bool enableEditorAlphaRef = false;

	bool rimLighting = false;
	float rimPower = 0.0f;
	float backlightPower = 0.0f;

	bool subsurfaceLighting = false;
	float subsurfaceLightingRolloff = 0.0f;

	bool specularEnabled = true;
	Vector3 specular;
	Color3 specularColor = {1.0f, 1.0f, 1.0f};
	float specularMult = 0.0f;

	float smoothness = 0.0f;
	float fresnelPower = 0.0f;

	float wetnessControlSpecScale = 0.0f;
	float wetnessControlSpecPowerScale = 0.0f;
	float wetnessControlSpecMinvar = 0.0f;
	float wetnessControlEnvMapScale = 0.0f;
	float wetnessControlFresnelPower = 0.0f;
	float wetnessControlMetalness = 0.0f;

	BgmString rootMaterialPath;
	bool anisoLighting = false;
	bool emitEnabled = false;
	float emittanceMult = 0.0f;
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

	bool hair = false;
	Vector3 hairColor;
	Color3 hairTintColor;

	bool tree = false;
	bool facegen = false;
	bool skinTint = false;
	bool tessellate = false;

	float displacementTextureBias = 0.0f;
	float displacementTextureScale = 0.0f;

	float tessellationPnScale = 0.0f;
	float tessellationBaseFactor = 0.0f;
	float tessellationFadeDistance = 0.0f;

	float grayscaleToPaletteScale = 0.0f;
	bool skewSpecularAlpha = false;
	bool pbr = false;

	bool translucency = false;
	bool translucencyThickObject = false;
	bool translucencyMixAlbedoWithSubsurfaceCol = false;
	Color3 translucencySubsurfaceColor;
	float translucencyTransmissiveScale = 0.0f;
	float translucencyTurbulence = 0.0f;

	bool customPorosity = false;
	float porosityValue = 0.0f;

	bool useAdaptativeEmissive = false;

	bool terrain = false;
	float terrainThresholdFalloff = 0.0f;
	float terrainTilingDistance = 0.0f;
	float terrainRotationAngle = 0.0f;

	void Sync(BgmStreamReversible& stream) override;
};

class BgEffectMaterial : public BgmCloneableStreamable<BgEffectMaterial, BgMaterial> {
public:
	virtual ~BgEffectMaterial() = default;

	bool bloodEnabled = false;
	bool effectLightingEnabled = false;

	bool falloffEnabled = false;
	bool falloffColorEnabled = false;

	bool grayscaleToPaletteAlpha = false;
	bool softEnabled = false;

	Color3 baseColor = {1.0f, 1.0f, 1.0f};
	float baseColorScale = 1.0f;

	float falloffStartAngle = 1.0f;
	float falloffStopAngle = 0.0f;
	float falloffStartOpacity = 1.0f;
	float falloffStopOpacity = 0.0f;

	float lightingInfluence = 1.0f;
	bool envmapMinLOD = false;
	float softDepth = 100.0f;
	bool effectPbrSpecular = false;

	void Sync(BgmStreamReversible& stream) override;
};
} // namespace nifly
