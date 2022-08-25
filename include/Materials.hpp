/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#pragma once

#include "BasicTypes.hpp"

namespace nifly {
class BgString {
private:
	std::string str;

public:
	BgString() = default;
	BgString(const std::string& s)
		: str(s) {}

	std::string& get() { return str; }
	const std::string& get() const { return str; }

	size_t length() const { return str.length(); }

	void clear() { str.clear(); }

	void Sync(StreamReversible& stream) {
		if (auto istream = stream.asRead()) {
			uint32_t size;
			*istream >> size;

			std::vector<char> buffer(size, 0);
			istream->read(buffer.data(), size);
			str = std::string(buffer.data(), size - 1);
		}
		else if (auto ostream = stream.asWrite()) {
			const auto size = str.length() + 1;
			*ostream << size;
			ostream->write(str.c_str(), size);
		}
	}

	bool operator==(const BgString& rhs) const { return str == rhs.str; }
	bool operator!=(const BgString& rhs) const { return !operator==(rhs); }

	bool operator==(const std::string& rhs) const { return str == rhs; }
	bool operator!=(const std::string& rhs) const { return !operator==(rhs); }
};

enum BgMaterialType : uint32_t {
	BGSM = 0x4D454742,
	BGEM = 0x4D534742,
};

enum BgMaterialVersion : uint32_t {
	V1 = 0x01,
	V2 = 0x02,
	V3 = 0x03,
	V6 = 0x06,
	V7 = 0x08,
	V8 = 0x08,
	V9 = 0x09,
	V10 = 0x0A,
	V12 = 0x0C,
	V13 = 0x0D,
	V15 = 0x0F,
	V16 = 0x10,
	V17 = 0x11,
	V20 = 0x14,
};

class BgMaterial {
public:
	BgMaterial(BgMaterialType t)
		: type(t) {}

	virtual ~BgMaterial() = default;

	BgMaterialType type = BgMaterialType::BGSM;
	BgMaterialVersion version = BgMaterialVersion::V1;
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
	std::vector<BgString> textures;

	virtual void Sync(StreamReversible& stream);

protected:
	void SyncTextures(StreamReversible& stream, size_t count);
};

class BgShaderMaterial : public BgMaterial {
public:
	BgShaderMaterial()
		: BgMaterial(BgMaterialType::BGSM) {}

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
	BgString rootMaterialPath;
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

	void Sync(StreamReversible& stream) override;
};

class BgEffectMaterial : public BgMaterial {
public:
	BgEffectMaterial()
		: BgMaterial(BgMaterialType::BGEM) {}

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

	void Sync(StreamReversible& stream) override;
};
} // namespace nifly
