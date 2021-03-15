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

class NiProperty : public NiObjectCRTP<NiProperty, NiObjectNET> {};

class NiShadeProperty : public NiObjectCRTP<NiShadeProperty, NiProperty, true> {
private:
	uint16_t flags = 0;

public:
	static constexpr const char* BlockName = "NiShadeProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiSpecularProperty : public NiObjectCRTP<NiSpecularProperty, NiProperty, true> {
private:
	uint16_t flags = 0;

public:
	static constexpr const char* BlockName = "NiSpecularProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

struct TexTransform {
	Vector2 translation;
	Vector2 tiling;
	float wRotation = 0.0f;
	uint32_t transformType = 0;
	Vector2 offset;
};

class TexDesc {
private:
	BlockRef<NiSourceTexture> sourceRef;
	uint16_t flags = 0;
	bool hasTexTransform = false;
	TexTransform transform;

public:
	void Sync(NiStreamReversible& stream) {
		sourceRef.Sync(stream);
		stream.Sync(flags);
		stream.Sync(hasTexTransform);

		if (hasTexTransform)
			stream.Sync(transform);
	}

	void GetChildRefs(std::set<Ref*>& refs) { refs.insert(&sourceRef); }

	void GetChildIndices(std::vector<int>& indices) { indices.push_back(sourceRef.GetIndex()); }

	int GetSourceRef() { return sourceRef.GetIndex(); }

	void SetSourceRef(int srcRef) { sourceRef.SetIndex(srcRef); }
};

class ShaderTexDesc {
private:
	bool isUsed = false;
	TexDesc data;
	uint32_t mapIndex = 0;

public:
	void Sync(NiStreamReversible& stream) {
		stream.Sync(isUsed);

		if (isUsed) {
			data.Sync(stream);
			stream.Sync(mapIndex);
		}
	}

	void GetChildRefs(std::set<Ref*>& refs) { data.GetChildRefs(refs); }

	void GetChildIndices(std::vector<int>& indices) { data.GetChildIndices(indices); }
};

class NiTexturingProperty : public NiObjectCRTP<NiTexturingProperty, NiProperty, true> {
private:
	uint16_t flags = 0;
	uint32_t textureCount = 0;

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
	float parallaxFloat = 0.0f;

	bool hasDecalTex0 = false;
	TexDesc decalTex0;

	bool hasDecalTex1 = false;
	TexDesc decalTex1;

	bool hasDecalTex2 = false;
	TexDesc decalTex2;

	bool hasDecalTex3 = false;
	TexDesc decalTex3;

	uint32_t numShaderTex = 0;
	std::vector<ShaderTexDesc> shaderTex;

public:
	static constexpr const char* BlockName = "NiTexturingProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class NiVertexColorProperty : public NiObjectCRTP<NiVertexColorProperty, NiProperty, true> {
private:
	uint16_t flags = 0;

public:
	static constexpr const char* BlockName = "NiVertexColorProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiDitherProperty : public NiObjectCRTP<NiDitherProperty, NiProperty, true> {
private:
	uint16_t flags = 0;

public:
	static constexpr const char* BlockName = "NiDitherProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiFogProperty : public NiObjectCRTP<NiFogProperty, NiProperty, true> {
private:
	uint16_t flags = 0;
	float fogDepth = 1.0f;
	Color3 fogColor;

public:
	static constexpr const char* BlockName = "NiFogProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiWireframeProperty : public NiObjectCRTP<NiWireframeProperty, NiProperty, true> {
private:
	uint16_t flags = 0;

public:
	static constexpr const char* BlockName = "NiWireframeProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiZBufferProperty : public NiObjectCRTP<NiZBufferProperty, NiProperty, true> {
private:
	uint16_t flags = 3;

public:
	static constexpr const char* BlockName = "NiZBufferProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiShader : public NiObjectCRTP<NiShader, NiProperty> {
public:
	virtual bool IsSkinTinted() { return false; }
	virtual bool IsFaceTinted() { return false; }
	virtual bool IsSkinned() { return false; }
	virtual void SetSkinned(const bool) {}
	virtual bool IsDoubleSided() { return false; }
	virtual bool IsModelSpace() { return false; }
	virtual bool IsEmissive() { return false; }
	virtual bool HasSpecular() { return true; }
	virtual bool HasVertexColors() { return false; }
	virtual void SetVertexColors(const bool) {}
	virtual bool HasVertexAlpha() { return false; }
	virtual void SetVertexAlpha(const bool) {}
	virtual bool HasBacklight() { return false; }
	virtual bool HasRimlight() { return false; }
	virtual bool HasSoftlight() { return false; }
	virtual bool HasGlowmap() { return false; }
	virtual bool HasGreyscaleColor() { return false; }
	virtual bool HasEnvironmentMapping() { return false; }
	virtual uint32_t GetShaderType() { return 0; }
	virtual void SetShaderType(const uint32_t) {}
	virtual Vector2 GetUVOffset() { return Vector2(); }
	virtual Vector2 GetUVScale() { return Vector2(1.0f, 1.0f); }
	virtual Vector3 GetSpecularColor() { return Vector3(); }
	virtual void SetSpecularColor(const Vector3&) {}
	virtual float GetSpecularStrength() { return 0.0f; }
	virtual void SetSpecularStrength(const float) {}
	virtual float GetGlossiness() { return 0.0f; }
	virtual void SetGlossiness(const float) {}
	virtual float GetEnvironmentMapScale() { return 0.0f; }
	virtual int GetTextureSetRef() { return NIF_NPOS; }
	virtual void SetTextureSetRef(const int) {}
	virtual Color4 GetEmissiveColor() { return Color4(); }
	virtual void SetEmissiveColor(const Color4&) {}
	virtual float GetEmissiveMultiple() { return 0.0f; }
	virtual void SetEmissiveMultiple(const float) {}
	virtual float GetAlpha() { return 1.0f; }
	virtual float GetBacklightPower() { return 0.0f; }
	virtual float GetRimlightPower() { return 2.0f; }
	virtual float GetSoftlight() { return 0.3f; }
	virtual float GetSubsurfaceRolloff() { return 0.3f; }
	virtual float GetGrayscaleToPaletteScale() { return 1.0; }
	virtual float GetFresnelPower() { return 5.0f; }
	virtual std::string GetWetMaterialName() { return std::string(); }
	virtual void SetWetMaterialName(const std::string&) {}
};

class BSShaderProperty : public NiObjectCRTP<BSShaderProperty, NiShader, true> {
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

	uint32_t GetShaderType() override;
	void SetShaderType(const uint32_t type) override;
	bool IsSkinTinted() override;
	bool IsFaceTinted() override;
	bool IsSkinned() override;
	void SetSkinned(const bool enable) override;
	bool IsDoubleSided() override;
	bool IsModelSpace() override;
	bool IsEmissive() override;
	bool HasSpecular() override;
	bool HasVertexColors() override;
	void SetVertexColors(const bool enable) override;
	bool HasVertexAlpha() override;
	void SetVertexAlpha(const bool enable) override;
	bool HasBacklight() override;
	bool HasRimlight() override;
	bool HasSoftlight() override;
	bool HasGlowmap() override;
	bool HasGreyscaleColor() override;
	bool HasEnvironmentMapping() override;
	float GetEnvironmentMapScale() override;
	Vector2 GetUVOffset() override;
	Vector2 GetUVScale() override;
};

class WaterShaderProperty : public NiObjectCRTP<WaterShaderProperty, BSShaderProperty> {
public:
	static constexpr const char* BlockName = "WaterShaderProperty";
	const char* GetBlockName() override { return BlockName; }
};

class HairShaderProperty : public NiObjectCRTP<HairShaderProperty, BSShaderProperty> {
public:
	static constexpr const char* BlockName = "HairShaderProperty";
	const char* GetBlockName() override { return BlockName; }
};

class DistantLODShaderProperty : public NiObjectCRTP<DistantLODShaderProperty, BSShaderProperty> {
public:
	static constexpr const char* BlockName = "DistantLODShaderProperty";
	const char* GetBlockName() override { return BlockName; }
};

class BSDistantTreeShaderProperty : public NiObjectCRTP<BSDistantTreeShaderProperty, BSShaderProperty> {
public:
	static constexpr const char* BlockName = "BSDistantTreeShaderProperty";
	const char* GetBlockName() override { return BlockName; }
};

class TallGrassShaderProperty : public NiObjectCRTP<TallGrassShaderProperty, BSShaderProperty, true> {
private:
	NiString fileName;

public:
	static constexpr const char* BlockName = "TallGrassShaderProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class VolumetricFogShaderProperty : public NiObjectCRTP<VolumetricFogShaderProperty, BSShaderProperty> {
public:
	static constexpr const char* BlockName = "VolumetricFogShaderProperty";
	const char* GetBlockName() override { return BlockName; }
};

class BSShaderTextureSet : public NiObjectCRTP<BSShaderTextureSet, NiObject, true> {
public:
	int numTextures = 13;
	std::vector<NiString> textures = std::vector<NiString>(13);

	BSShaderTextureSet() {}
	BSShaderTextureSet(NiVersion& version);

	static constexpr const char* BlockName = "BSShaderTextureSet";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSTextureArray {
private:
	uint32_t textureArrayWidth = 0;
	std::vector<NiString> textureArray;

public:
	void Sync(NiStreamReversible& stream) {
		stream.Sync(textureArrayWidth);
		textureArray.resize(textureArrayWidth);

		for (uint32_t i = 0; i < textureArrayWidth; i++) {
			auto& tex = textureArray[i];
			tex.Sync(stream, 4);
		}
	}
};

class BSLightingShaderProperty : public NiObjectCRTP<BSLightingShaderProperty, BSShaderProperty, true> {
private:
	StringRef rootMaterialName;
	BlockRef<BSShaderTextureSet> textureSetRef;

public:
	Vector3 emissiveColor;
	float emissiveMultiple = 1.0f;
	uint32_t textureClampMode = 3;
	float alpha = 1.0f;
	float refractionStrength = 0.0f;
	float glossiness = 1.0f;
	Vector3 specularColor = Vector3(1.0f, 1.0f, 1.0f);
	float specularStrength = 1.0f;
	float softlighting = 0.3f;	// User Version <= 12, User Version 2 < 130
	float rimlightPower = 2.0f; // User Version <= 12, User Version 2 < 130

	float subsurfaceRolloff = 0.3f;		  // User Version == 12, User Version 2 >= 130
	float unkFloat1 = 0.0f;				  // User Version == 12, User Version 2 >= 130
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
	void GetStringRefs(std::vector<StringRef*>& refs) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;


	bool IsSkinTinted() override;
	bool IsFaceTinted() override;
	bool HasGlowmap() override;
	bool HasEnvironmentMapping() override;
	uint32_t GetShaderType() override;
	void SetShaderType(const uint32_t type) override;
	Vector3 GetSpecularColor() override;
	void SetSpecularColor(const Vector3& color) override;
	float GetSpecularStrength() override;
	void SetSpecularStrength(const float strength) override;
	float GetGlossiness() override;
	void SetGlossiness(const float gloss) override;
	int GetTextureSetRef() override;
	void SetTextureSetRef(const int texSetRef) override;
	Color4 GetEmissiveColor() override;
	void SetEmissiveColor(const Color4& color) override;
	float GetEmissiveMultiple() override;
	void SetEmissiveMultiple(const float emissive) override;
	float GetAlpha() override;
	float GetBacklightPower() override;
	float GetRimlightPower() override;
	float GetSoftlight() override;
	float GetSubsurfaceRolloff() override;
	float GetGrayscaleToPaletteScale() override;
	float GetFresnelPower() override;
	std::string GetWetMaterialName() override;
	void SetWetMaterialName(const std::string& matName) override;
};

class BSEffectShaderProperty : public NiObjectCRTP<BSEffectShaderProperty, BSShaderProperty, true> {
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

	float GetEnvironmentMapScale() override;
	Color4 GetEmissiveColor() override;
	void SetEmissiveColor(const Color4& color) override;
	float GetEmissiveMultiple() override;
	void SetEmissiveMultiple(const float emissive) override;
};

class BSWaterShaderProperty : public NiObjectCRTP<BSWaterShaderProperty, BSShaderProperty, true> {
private:
	uint32_t waterFlags = 0;

public:
	static constexpr const char* BlockName = "BSWaterShaderProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSSkyShaderProperty : public NiObjectCRTP<BSSkyShaderProperty, BSShaderProperty, true> {
private:
	NiString baseTexture;
	uint32_t skyFlags = 0;

public:
	static constexpr const char* BlockName = "BSSkyShaderProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSShaderLightingProperty : public NiObjectCRTP<BSShaderLightingProperty, BSShaderProperty, true> {
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

class SkyShaderProperty : public NiObjectCRTP<SkyShaderProperty, BSShaderLightingProperty, true> {
private:
	NiString fileName;
	SkyObjectType skyObjectType = BSSM_SKY_TEXTURE;

public:
	static constexpr const char* BlockName = "SkyShaderProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class TileShaderProperty : public NiObjectCRTP<TileShaderProperty, BSShaderLightingProperty, true> {
private:
	NiString fileName;

public:
	static constexpr const char* BlockName = "TileShaderProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class BSShaderNoLightingProperty
	: public NiObjectCRTP<BSShaderNoLightingProperty, BSShaderLightingProperty, true> {
public:
	NiString baseTexture;
	float falloffStartAngle = 1.0f;	  // User Version 2 > 26
	float falloffStopAngle = 0.0f;	  // User Version 2 > 26
	float falloffStartOpacity = 1.0f; // User Version 2 > 26
	float falloffStopOpacity = 1.0f;  // User Version 2 > 26

	static constexpr const char* BlockName = "BSShaderNoLightingProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	bool IsSkinned();
	void SetSkinned(const bool enable);
};

class BSShaderPPLightingProperty
	: public NiObjectCRTP<BSShaderPPLightingProperty, BSShaderLightingProperty, true> {
private:
	BlockRef<BSShaderTextureSet> textureSetRef;

public:
	float refractionStrength = 0.0f; // User Version == 11 && User Version 2 > 14
	int refractionFirePeriod = 0;	 // User Version == 11 && User Version 2 > 14
	float parallaxMaxPasses = 4.0f;	 // User Version == 11 && User Version 2 > 24
	float parallaxScale = 1.0f;		 // User Version == 11 && User Version 2 > 24
	Color4 emissiveColor;			 // User Version >= 12

	static constexpr const char* BlockName = "BSShaderPPLightingProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	bool IsSkinned();
	void SetSkinned(const bool enable);
	int GetTextureSetRef();
	void SetTextureSetRef(const int texSetRef);
};

class Lighting30ShaderProperty : public NiObjectCRTP<Lighting30ShaderProperty, BSShaderPPLightingProperty> {
public:
	static constexpr const char* BlockName = "Lighting30ShaderProperty";
	const char* GetBlockName() override { return BlockName; }
};

class NiAlphaProperty : public NiObjectCRTP<NiAlphaProperty, NiProperty, true> {
public:
	uint16_t flags = 4844;
	uint8_t threshold = 128;

	static constexpr const char* BlockName = "NiAlphaProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};


class NiMaterialProperty : public NiObjectCRTP<NiMaterialProperty, NiProperty, true> {
private:
	Vector3 colorSpecular;
	Vector3 colorEmissive;
	float glossiness = 1.0f;
	float alpha = 1.0f;
	float emitMulti = 1.0f; // Version == 20.2.0.7 && User Version >= 11 && User Version 2 > 21

public:
	Vector3 colorAmbient; // !(Version == 20.2.0.7 && User Version >= 11 && User Version 2 > 21)
	Vector3 colorDiffuse; // !(Version == 20.2.0.7 && User Version >= 11 && User Version 2 > 21)

	static constexpr const char* BlockName = "NiMaterialProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);

	bool IsEmissive();
	Vector3 GetSpecularColor();
	void SetSpecularColor(const Vector3& color);
	float GetGlossiness();
	void SetGlossiness(const float gloss);
	Color4 GetEmissiveColor();
	void SetEmissiveColor(const Color4& color);
	float GetEmissiveMultiple();
	void SetEmissiveMultiple(const float emissive);
	float GetAlpha();
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

class NiStencilProperty : public NiObjectCRTP<NiStencilProperty, NiProperty, true> {
public:
	uint16_t flags = 19840;
	uint32_t stencilRef = 0;
	uint32_t stencilMask = 0xFFFFFFFF;

	static constexpr const char* BlockName = "NiStencilProperty";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};
} // namespace nifly
