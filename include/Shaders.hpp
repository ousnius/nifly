/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#pragma once

#include "BasicTypes.hpp"
#include "Objects.hpp"

namespace nifly {
enum BSShaderType : uint32_t {
	SHADER_TALL_GRASS,
	SHADER_DEFAULT,
	SHADER_SKY = 10,
	SHADER_SKIN = 14,
	SHADER_WATER = 17,
	SHADER_LIGHTING30 = 29,
	SHADER_TILE = 32,
	SHADER_NOLIGHTING
};

enum BSLightingShaderPropertyShaderType : uint32_t {
	BSLSP_DEFAULT,
	BSLSP_ENVMAP,
	BSLSP_GLOWMAP,
	BSLSP_PARALLAX,
	BSLSP_FACE,
	BSLSP_SKINTINT,
	BSLSP_HAIRTINT,
	BSLSP_PARALLAXOCC,
	BSLSP_MULTITEXTURELANDSCAPE,
	BSLSP_LODLANDSCAPE,
	BSLSP_SNOW,
	BSLSP_MULTILAYERPARALLAX,
	BSLSP_TREEANIM,
	BSLSP_LODOBJECTS,
	BSLSP_MULTIINDEXSNOW,
	BSLSP_LODOBJECTSHD,
	BSLSP_EYE,
	BSLSP_CLOUD,
	BSLSP_LODLANDSCAPENOISE,
	BSLSP_MULTITEXTURELANDSCAPELODBLEND,
	BSLSP_DISMEMBERMENT,
	BSLSP_LAST = BSLSP_DISMEMBERMENT
};

class NiProperty : public NiCloneable<NiProperty, NiObjectNET> {};

class NiShadeProperty : public NiCloneableStreamable<NiShadeProperty, NiProperty> {
public:
	uint16_t flags = 0;

	static constexpr const char* BlockName = "NiShadeProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiSpecularProperty : public NiCloneableStreamable<NiSpecularProperty, NiProperty> {
public:
	uint16_t flags = 0;

	static constexpr const char* BlockName = "NiSpecularProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

struct TexTransform {
	Vector2 translation;
	Vector2 scale;
	float wRotation = 0.0f;
	uint32_t transformType = 0;
	Vector2 center;
};

class TexDesc {
public:
	NiBlockRef<NiSourceTexture> sourceRef;
	TexClampMode clampMode = TexClampMode::WRAP_S_WRAP_T;
	TexFilterMode filterMode = TexFilterMode::FILTER_TRILERP;
	uint16_t flags = 0; // TexturingMapFlags
	uint16_t maxAnisotropy = 0;
	uint32_t uvSet = 0;
	int16_t ps2_l = 0;
	int16_t ps2_k = -75;
	bool hasTexTransform = false;
	TexTransform transform;

	void Sync(NiStreamReversible& stream) {
		const NiFileVersion fileVersion = stream.GetVersion().File();

		if (fileVersion >= NiFileVersion::V3_3_0_13)
			sourceRef.Sync(stream);

		if (fileVersion <= NiFileVersion::V20_0_0_5) {
			stream.Sync(clampMode);
			stream.Sync(filterMode);
			stream.Sync(uvSet);

			if (fileVersion < NiFileVersion::V10_4_0_1) {
				stream.Sync(ps2_l);
				stream.Sync(ps2_k);
			}
		}

		if (fileVersion >= NiFileVersion::V20_1_0_3)
			stream.Sync(flags);

		if (fileVersion >= NiVersion::ToFile(20, 5, 0, 4))
			stream.Sync(maxAnisotropy);

		if (fileVersion >= NiFileVersion::V10_1_0_0) {
			stream.Sync(hasTexTransform);

			if (hasTexTransform)
				stream.Sync(transform);
		}
	}

	void GetChildRefs(std::set<NiRef*>& refs) { refs.insert(&sourceRef); }
	void GetChildIndices(std::vector<uint32_t>& indices) { indices.push_back(sourceRef.index); }
};

class ShaderTexDesc {
public:
	bool isUsed = false;
	TexDesc data;
	uint32_t mapIndex = 0;

	void Sync(NiStreamReversible& stream) {
		stream.Sync(isUsed);

		if (isUsed) {
			data.Sync(stream);
			stream.Sync(mapIndex);
		}
	}

	void GetChildRefs(std::set<NiRef*>& refs) { data.GetChildRefs(refs); }
	void GetChildIndices(std::vector<uint32_t>& indices) { data.GetChildIndices(indices); }
};

class NiTexturingProperty : public NiCloneableStreamable<NiTexturingProperty, NiProperty> {
public:
	uint16_t flags = 0;
	uint32_t applyMode = 2;
	uint32_t textureCount = 7;

	bool hasBaseTex = false;
	TexDesc baseTex;

	bool hasDarkTex = false;
	TexDesc darkTex;

	bool hasDetailTex = false;
	TexDesc detailTex;

	bool hasGlossTex = false;
	TexDesc glossTex;

	bool hasGlowTex = false;
	TexDesc glowTex;

	bool hasBumpTex = false;
	TexDesc bumpTex;
	float lumaScale = 1.0f;
	float lumaOffset = 0.0f;
	Vector4 bumpMatrix;

	bool hasNormalTex = false;
	TexDesc normalTex;

	bool hasParallaxTex = false;
	TexDesc parallaxTex;
	float parallaxOffset = 0.0f;

	bool hasDecalTex0 = false;
	TexDesc decalTex0;

	bool hasDecalTex1 = false;
	TexDesc decalTex1;

	bool hasDecalTex2 = false;
	TexDesc decalTex2;

	bool hasDecalTex3 = false;
	TexDesc decalTex3;

	NiSyncVector<ShaderTexDesc> shaderTex;

	static constexpr const char* BlockName = "NiTexturingProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;
};

class NiVertexColorProperty : public NiCloneableStreamable<NiVertexColorProperty, NiProperty> {
public:
	uint16_t flags = 0;
	uint32_t vertexMode = 0;
	uint32_t lightingMode = 0;

	static constexpr const char* BlockName = "NiVertexColorProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiDitherProperty : public NiCloneableStreamable<NiDitherProperty, NiProperty> {
public:
	uint16_t flags = 0;

	static constexpr const char* BlockName = "NiDitherProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiFogProperty : public NiCloneableStreamable<NiFogProperty, NiProperty> {
public:
	uint16_t flags = 0;
	float fogDepth = 1.0f;
	Color3 fogColor;

	static constexpr const char* BlockName = "NiFogProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiWireframeProperty : public NiCloneableStreamable<NiWireframeProperty, NiProperty> {
public:
	uint16_t flags = 0;

	static constexpr const char* BlockName = "NiWireframeProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiZBufferProperty : public NiCloneableStreamable<NiZBufferProperty, NiProperty> {
public:
	uint16_t flags = 3;

	static constexpr const char* BlockName = "NiZBufferProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSShaderTextureSet : public NiCloneableStreamable<BSShaderTextureSet, NiObject> {
public:
	NiStringVector<> textures = NiStringVector<>(13);

	BSShaderTextureSet() {}
	BSShaderTextureSet(NiVersion& version);

	static constexpr const char* BlockName = "BSShaderTextureSet";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiShader : public NiCloneable<NiShader, NiProperty> {
public:
	virtual bool HasTextureSet() const { return false; }
	virtual NiBlockRef<BSShaderTextureSet>* TextureSetRef() { return nullptr; }
	virtual const NiBlockRef<BSShaderTextureSet>* TextureSetRef() const { return nullptr; }

	virtual bool IsSkinTinted() const { return false; }
	virtual bool IsFaceTinted() const { return false; }
	virtual bool IsSkinned() const { return false; }
	virtual void SetSkinned(const bool) {}
	virtual bool IsDoubleSided() const { return false; }
	virtual bool IsModelSpace() const { return false; }
	virtual bool IsEmissive() const { return false; }
	virtual bool HasSpecular() const { return true; }
	virtual bool HasVertexColors() const { return false; }
	virtual void SetVertexColors(const bool) {}
	virtual bool HasVertexAlpha() const { return false; }
	virtual void SetVertexAlpha(const bool) {}
	virtual bool HasBacklight() const { return false; }
	virtual bool HasRimlight() const { return false; }
	virtual bool HasSoftlight() const { return false; }
	virtual bool HasGlowmap() const { return false; }
	virtual bool HasGreyscaleColor() const { return false; }
	virtual bool HasEnvironmentMapping() const { return false; }
	virtual uint32_t GetShaderType() const { return 0; }
	virtual void SetShaderType(const uint32_t) {}
	virtual Vector2 GetUVOffset() const { return Vector2(); }
	virtual Vector2 GetUVScale() const { return Vector2(1.0f, 1.0f); }
	virtual Vector3 GetSpecularColor() const { return Vector3(); }
	virtual void SetSpecularColor(const Vector3&) {}
	virtual float GetSpecularStrength() const { return 0.0f; }
	virtual void SetSpecularStrength(const float) {}
	virtual float GetGlossiness() const { return 0.0f; }
	virtual void SetGlossiness(const float) {}
	virtual float GetEnvironmentMapScale() const { return 0.0f; }
	virtual Color4 GetEmissiveColor() const { return Color4(); }
	virtual void SetEmissiveColor(const Color4&) {}
	virtual float GetEmissiveMultiple() const { return 0.0f; }
	virtual void SetEmissiveMultiple(const float) {}
	virtual float GetAlpha() const { return 1.0f; }
	virtual void SetAlpha(const float) {}
	virtual float GetBacklightPower() const { return 0.0f; }
	virtual float GetRimlightPower() const { return 2.0f; }
	virtual float GetSoftlight() const { return 0.3f; }
	virtual float GetSubsurfaceRolloff() const { return 0.3f; }
	virtual float GetGrayscaleToPaletteScale() const { return 1.0; }
	virtual float GetFresnelPower() const { return 5.0f; }
	virtual std::string GetWetMaterialName() const { return std::string(); }
	virtual void SetWetMaterialName(const std::string&) {}
};

class BSShaderProperty : public NiCloneableStreamable<BSShaderProperty, NiShader> {
public:
	uint16_t shaderFlags = 1;
	BSShaderType shaderType = SHADER_DEFAULT;
	uint32_t shaderFlags1 = 0x82000000;
	uint32_t shaderFlags2 = 1;
	float environmentMapScale = 1.0f;

	uint32_t numSF1 = 0;
	uint32_t numSF2 = 0;
	std::vector<uint32_t> SF1;
	std::vector<uint32_t> SF2;

	Vector2 uvOffset;
	Vector2 uvScale = Vector2(1.0f, 1.0f);

	void Sync(NiStreamReversible& stream);

	uint32_t GetShaderType() const override;
	void SetShaderType(const uint32_t type) override;
	bool IsSkinTinted() const override;
	bool IsFaceTinted() const override;
	bool IsSkinned() const override;
	void SetSkinned(const bool enable) override;
	bool IsDoubleSided() const override;
	bool IsModelSpace() const override;
	bool IsEmissive() const override;
	bool HasSpecular() const override;
	bool HasVertexColors() const override;
	void SetVertexColors(const bool enable) override;
	bool HasVertexAlpha() const override;
	void SetVertexAlpha(const bool enable) override;
	bool HasBacklight() const override;
	bool HasRimlight() const override;
	bool HasSoftlight() const override;
	bool HasGlowmap() const override;
	bool HasGreyscaleColor() const override;
	bool HasEnvironmentMapping() const override;
	float GetEnvironmentMapScale() const override;
	Vector2 GetUVOffset() const override;
	Vector2 GetUVScale() const override;
};

class WaterShaderProperty : public NiCloneable<WaterShaderProperty, BSShaderProperty> {
public:
	static constexpr const char* BlockName = "WaterShaderProperty";
	const char* GetBlockName() override { return BlockName; }
};

class HairShaderProperty : public NiCloneable<HairShaderProperty, BSShaderProperty> {
public:
	static constexpr const char* BlockName = "HairShaderProperty";
	const char* GetBlockName() override { return BlockName; }
};

class DistantLODShaderProperty : public NiCloneable<DistantLODShaderProperty, BSShaderProperty> {
public:
	static constexpr const char* BlockName = "DistantLODShaderProperty";
	const char* GetBlockName() override { return BlockName; }
};

class BSDistantTreeShaderProperty : public NiCloneable<BSDistantTreeShaderProperty, BSShaderProperty> {
public:
	static constexpr const char* BlockName = "BSDistantTreeShaderProperty";
	const char* GetBlockName() override { return BlockName; }
};

class TallGrassShaderProperty : public NiCloneableStreamable<TallGrassShaderProperty, BSShaderProperty> {
public:
	NiString fileName;

	static constexpr const char* BlockName = "TallGrassShaderProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class VolumetricFogShaderProperty : public NiCloneable<VolumetricFogShaderProperty, BSShaderProperty> {
public:
	static constexpr const char* BlockName = "VolumetricFogShaderProperty";
	const char* GetBlockName() override { return BlockName; }
};

class BSTextureArray {
public:
	NiStringVector<> textureArray;

	void Sync(NiStreamReversible& stream) { textureArray.Sync(stream); }
};

class BSLightingShaderProperty : public NiCloneableStreamable<BSLightingShaderProperty, BSShaderProperty> {
public:
	NiBlockRef<BSShaderTextureSet> textureSetRef;

	Vector3 emissiveColor;
	float emissiveMultiple = 1.0f;
	NiStringRef rootMaterialName;
	uint32_t textureClampMode = 3;
	float alpha = 1.0f;
	float refractionStrength = 0.0f;
	float glossiness = 1.0f;
	Vector3 specularColor = Vector3(1.0f, 1.0f, 1.0f);
	float specularStrength = 1.0f;
	float softlighting = 0.3f;	// User Version <= 12, User Version 2 < 130
	float rimlightPower = 2.0f; // User Version <= 12, User Version 2 < 130

	float subsurfaceRolloff = 0.3f;		  // User Version == 12, User Version 2 >= 130
	float rimlightPower2 = NiFloatMax;	  // User Version == 12, User Version 2 >= 130
	float backlightPower = 0.0f;		  // User Version == 12, User Version 2 >= 130
	float grayscaleToPaletteScale = 1.0f; // User Version == 12, User Version 2 >= 130
	float fresnelPower = 5.0f;			  // User Version == 12, User Version 2 >= 130
	float wetnessSpecScale = 0.6f;		  // User Version == 12, User Version 2 >= 130
	float wetnessSpecPower = 1.4f;		  // User Version == 12, User Version 2 >= 130
	float wetnessMinVar = 0.2f;			  // User Version == 12, User Version 2 >= 130
	float wetnessEnvmapScale = 1.0f;	  // User Version == 12, User Version 2 >= 130
	float wetnessFresnelPower = 1.6f;	  // User Version == 12, User Version 2 >= 130
	float wetnessMetalness = 0.0f;		  // User Version == 12, User Version 2 >= 130
	float wetnessUnknown1 = 0.0f;		  // User Version == 12, User Version 2 == 155
	float wetnessUnknown2 = 0.0f;		  // User Version == 12, User Version 2 == 155

	float lumEmittance = 100.0f;   // User Version == 12, User Version 2 == 155
	float exposureOffset = 13.5f;  // User Version == 12, User Version 2 == 155
	float finalExposureMin = 2.0f; // User Version == 12, User Version 2 == 155
	float finalExposureMax = 3.0f; // User Version == 12, User Version 2 == 155

	bool doTranslucency = false;	// User Version == 12, User Version 2 == 155
	Color3 subsurfaceColor;			// User Version == 12, User Version 2 == 155
	float transmissiveScale = 1.0f; // User Version == 12, User Version 2 == 155
	float turbulence = 0.0f;		// User Version == 12, User Version 2 == 155
	bool thickObject = false;		// User Version == 12, User Version 2 == 155
	bool mixAlbedo = false;			// User Version == 12, User Version 2 == 155

	bool hasTextureArrays = false;			   // User Version == 12, User Version 2 == 155
	uint32_t numTextureArrays = 0;			   // User Version == 12, User Version 2 == 155
	std::vector<BSTextureArray> textureArrays; // User Version == 12, User Version 2 == 155

	bool useSSR = false;		// Shader Type == 1, User Version == 12, User Version 2 == 130
	bool wetnessUseSSR = false; // Shader Type == 1, User Version == 12, User Version 2 == 130
	Vector3 skinTintColor = Vector3(1.0f,
									1.0f,
									1.0f); // Shader Type == 5, User Version == 12, User Version 2 <= 130
	float skinTintAlpha = 0.0f;			   // Shader Type == 5, User Version == 12, User Version 2 == 130
	Vector3 hairTintColor = Vector3(1.0f,
									1.0f,
									1.0f);	  // Shader Type == 6, User Version == 12, User Version 2 <= 130
	float maxPasses = 1.0f;					  // Shader Type == 7
	float scale = 1.0f;						  // Shader Type == 7
	float parallaxInnerLayerThickness = 0.0f; // Shader Type == 11
	float parallaxRefractionScale = 1.0f;	  // Shader Type == 11
	Vector2 parallaxInnerLayerTextureScale = Vector2(1.0f, 1.0f); // Shader Type == 11
	float parallaxEnvmapStrength = 1.0f;						  // Shader Type == 11
	Color4 sparkleParameters;									  // Shader Type == 14
	float eyeCubemapScale = 1.0f;								  // Shader Type == 16
	Vector3 eyeLeftReflectionCenter;							  // Shader Type == 16
	Vector3 eyeRightReflectionCenter;							  // Shader Type == 16

	BSLightingShaderProperty();
	BSLightingShaderProperty(NiVersion& version);

	static constexpr const char* BlockName = "BSLightingShaderProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;

	bool HasTextureSet() const override { return !textureSetRef.IsEmpty(); }
	NiBlockRef<BSShaderTextureSet>* TextureSetRef() override { return &textureSetRef; }
	const NiBlockRef<BSShaderTextureSet>* TextureSetRef() const override { return &textureSetRef; }

	bool IsSkinTinted() const override;
	bool IsFaceTinted() const override;
	bool HasGlowmap() const override;
	bool HasEnvironmentMapping() const override;
	uint32_t GetShaderType() const override;
	void SetShaderType(const uint32_t type) override;
	Vector3 GetSpecularColor() const override;
	void SetSpecularColor(const Vector3& color) override;
	float GetSpecularStrength() const override;
	void SetSpecularStrength(const float strength) override;
	float GetGlossiness() const override;
	void SetGlossiness(const float gloss) override;
	Color4 GetEmissiveColor() const override;
	void SetEmissiveColor(const Color4& color) override;
	float GetEmissiveMultiple() const override;
	void SetEmissiveMultiple(const float emissive) override;
	float GetAlpha() const override;
	void SetAlpha(const float alphaValue) override;
	float GetBacklightPower() const override;
	float GetRimlightPower() const override;
	float GetSoftlight() const override;
	float GetSubsurfaceRolloff() const override;
	float GetGrayscaleToPaletteScale() const override;
	float GetFresnelPower() const override;
	std::string GetWetMaterialName() const override;
	void SetWetMaterialName(const std::string& matName) override;
};

class BSEffectShaderProperty : public NiCloneableStreamable<BSEffectShaderProperty, BSShaderProperty> {
public:
	NiString sourceTexture;
	uint32_t textureClampMode = 0;
	float falloffStartAngle = 1.0f;
	float falloffStopAngle = 1.0f;
	float falloffStartOpacity = 0.0f;
	float falloffStopOpacity = 0.0f;
	float refractionPower = 0.0f; // User Version == 12, User Version 2 == 155
	Color4 baseColor;
	float baseColorScale = 1.0f;
	float softFalloffDepth = 0.0f;
	NiString greyscaleTexture;

	NiString envMapTexture;
	NiString normalTexture;
	NiString envMaskTexture;
	float envMapScale = 1.0f;

	NiString reflectanceTexture;  // User Version == 12, User Version 2 == 155
	NiString lightingTexture;	  // User Version == 12, User Version 2 == 155
	Color3 emittanceColor;		  // User Version == 12, User Version 2 == 155
	NiString emitGradientTexture; // User Version == 12, User Version 2 == 155

	float lumEmittance = 100.0f;   // User Version == 12, User Version 2 == 155
	float exposureOffset = 13.5f;  // User Version == 12, User Version 2 == 155
	float finalExposureMin = 2.0f; // User Version == 12, User Version 2 == 155
	float finalExposureMax = 3.0f; // User Version == 12, User Version 2 == 155

	static constexpr const char* BlockName = "BSEffectShaderProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	float GetEnvironmentMapScale() const override;
	Color4 GetEmissiveColor() const override;
	void SetEmissiveColor(const Color4& color) override;
	float GetEmissiveMultiple() const override;
	void SetEmissiveMultiple(const float emissive) override;
};

class BSWaterShaderProperty : public NiCloneableStreamable<BSWaterShaderProperty, BSShaderProperty> {
public:
	uint32_t waterFlags = 0;

	static constexpr const char* BlockName = "BSWaterShaderProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSSkyShaderProperty : public NiCloneableStreamable<BSSkyShaderProperty, BSShaderProperty> {
public:
	NiString baseTexture;
	uint32_t skyFlags = 0;

	static constexpr const char* BlockName = "BSSkyShaderProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSShaderLightingProperty : public NiCloneableStreamable<BSShaderLightingProperty, BSShaderProperty> {
public:
	uint32_t textureClampMode = 3; // User Version <= 11

	void Sync(NiStreamReversible& stream);
};

enum SkyObjectType : uint32_t {
	BSSM_SKY_TEXTURE,
	BSSM_SKY_SUNGLARE,
	BSSM_SKY,
	BSSM_SKY_CLOUDS,
	BSSM_SKY_STARS = 5,
	BSSM_SKY_MOON_STARS_MASK = 7
};

class SkyShaderProperty : public NiCloneableStreamable<SkyShaderProperty, BSShaderLightingProperty> {
public:
	NiString fileName;
	SkyObjectType skyObjectType = BSSM_SKY_TEXTURE;

	static constexpr const char* BlockName = "SkyShaderProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class TileShaderProperty : public NiCloneableStreamable<TileShaderProperty, BSShaderLightingProperty> {
public:
	NiString fileName;

	static constexpr const char* BlockName = "TileShaderProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSShaderNoLightingProperty
	: public NiCloneableStreamable<BSShaderNoLightingProperty, BSShaderLightingProperty> {
public:
	NiString baseTexture;
	float falloffStartAngle = 1.0f;	  // User Version 2 > 26
	float falloffStopAngle = 0.0f;	  // User Version 2 > 26
	float falloffStartOpacity = 1.0f; // User Version 2 > 26
	float falloffStopOpacity = 1.0f;  // User Version 2 > 26

	static constexpr const char* BlockName = "BSShaderNoLightingProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	bool IsSkinned() const override;
	void SetSkinned(const bool enable) override;
};

class BSShaderPPLightingProperty
	: public NiCloneableStreamable<BSShaderPPLightingProperty, BSShaderLightingProperty> {
public:
	NiBlockRef<BSShaderTextureSet> textureSetRef;

	float refractionStrength = 0.0f; // User Version == 11 && User Version 2 > 14
	int refractionFirePeriod = 0;	 // User Version == 11 && User Version 2 > 14
	float parallaxMaxPasses = 4.0f;	 // User Version == 11 && User Version 2 > 24
	float parallaxScale = 1.0f;		 // User Version == 11 && User Version 2 > 24
	Color4 emissiveColor;			 // User Version >= 12

	static constexpr const char* BlockName = "BSShaderPPLightingProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<uint32_t>& indices) override;

	bool HasTextureSet() const override { return !textureSetRef.IsEmpty(); }
	NiBlockRef<BSShaderTextureSet>* TextureSetRef() override { return &textureSetRef; }
	const NiBlockRef<BSShaderTextureSet>* TextureSetRef() const override { return &textureSetRef; }

	bool IsSkinned() const override;
	void SetSkinned(const bool enable) override;
};

class Lighting30ShaderProperty : public NiCloneable<Lighting30ShaderProperty, BSShaderPPLightingProperty> {
public:
	static constexpr const char* BlockName = "Lighting30ShaderProperty";
	const char* GetBlockName() override { return BlockName; }
};

class NiAlphaProperty : public NiCloneableStreamable<NiAlphaProperty, NiProperty> {
public:
	uint16_t flags = 4844;
	uint8_t threshold = 128;

	static constexpr const char* BlockName = "NiAlphaProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};


class NiMaterialProperty : public NiCloneableStreamable<NiMaterialProperty, NiShader> {
protected:
	uint16_t legacyFlags = 0;
	Vector3 colorSpecular = Vector3(1.0f, 1.0f, 1.0f);
	Vector3 colorEmissive;
	float glossiness = 10.0f;
	float alpha = 1.0f;
	float emitMulti = 1.0f;

public:
	Vector3 colorAmbient = Vector3(1.0f, 1.0f, 1.0f);
	Vector3 colorDiffuse = Vector3(1.0f, 1.0f, 1.0f);

	static constexpr const char* BlockName = "NiMaterialProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	bool IsEmissive() const override;
	bool HasSpecular() const override;
	void SetSpecularColor(const Vector3& color) override;
	Vector3 GetSpecularColor() const override;
	float GetGlossiness() const override;
	void SetGlossiness(const float gloss) override;
	Color4 GetEmissiveColor() const override;
	void SetEmissiveColor(const Color4& color) override;
	float GetEmissiveMultiple() const override;
	void SetEmissiveMultiple(const float emissive) override;
	float GetAlpha() const override;
	void SetAlpha(const float alpha) override;
};

enum StencilMasks {
	ENABLE_MASK = 0x0001,
	FAIL_MASK = 0x000E,
	FAIL_POS = 1,
	ZFAIL_MASK = 0x0070,
	ZFAIL_POS = 4,
	ZPASS_MASK = 0x0380,
	ZPASS_POS = 7,
	DRAW_MASK = 0x0C00,
	DRAW_POS = 10,
	TEST_MASK = 0x7000,
	TEST_POS = 12
};

enum DrawMode { DRAW_CCW_OR_BOTH, DRAW_CCW, DRAW_CW, DRAW_BOTH, DRAW_MAX };

class NiStencilProperty : public NiCloneableStreamable<NiStencilProperty, NiProperty> {
public:
	uint16_t legacyFlags = 0;
	uint16_t flags = 19840;
	bool stencilEnabled = false;
	uint32_t stencilFunction = 0;
	uint32_t stencilRef = 0;
	uint32_t stencilMask = 0xFFFFFFFF;
	uint32_t failAction = 0;
	uint32_t zFailAction = 0;
	uint32_t passAction = 0;
	uint32_t drawMode = 3;

	static constexpr const char* BlockName = "NiStencilProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};
} // namespace nifly
