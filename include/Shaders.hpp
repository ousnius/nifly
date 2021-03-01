/*
BodySlide and Outfit Studio
See the included LICENSE file
*/

#pragma once

#include "BasicTypes.hpp"
#include "Objects.hpp"

namespace nifly {
enum BSShaderType : uint {
	SHADER_TALL_GRASS,
	SHADER_DEFAULT,
	SHADER_SKY = 10,
	SHADER_SKIN = 14,
	SHADER_WATER = 17,
	SHADER_LIGHTING30 = 29,
	SHADER_TILE = 32,
	SHADER_NOLIGHTING
};

enum BSLightingShaderPropertyShaderType : uint {
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

class NiProperty : public NiObjectNET {};

class NiShadeProperty : public NiProperty {
private:
	ushort flags = 0;

public:
	static constexpr const char* BlockName = "NiShadeProperty";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	NiShadeProperty* Clone() override { return new NiShadeProperty(*this); }
};

class NiSpecularProperty : public NiProperty {
private:
	ushort flags = 0;

public:
	static constexpr const char* BlockName = "NiSpecularProperty";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	NiSpecularProperty* Clone() override { return new NiSpecularProperty(*this); }
};

struct TexTransform {
	Vector2 translation;
	Vector2 tiling;
	float wRotation = 0.0f;
	uint transformType = 0;
	Vector2 offset;
};

class TexDesc {
private:
	BlockRef<NiSourceTexture> sourceRef;
	ushort flags = 0;
	bool hasTexTransform = false;
	TexTransform transform;

public:
	void Get(NiStream& stream) {
		sourceRef.Get(stream);
		stream >> flags;
		stream >> hasTexTransform;

		if (hasTexTransform)
			stream >> transform;
	}

	void Put(NiStream& stream) {
		sourceRef.Put(stream);
		stream << flags;
		stream << hasTexTransform;

		if (hasTexTransform)
			stream << transform;
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
	uint mapIndex = 0;

public:
	void Get(NiStream& stream) {
		stream >> isUsed;

		if (isUsed) {
			data.Get(stream);
			stream >> mapIndex;
		}
	}

	void Put(NiStream& stream) {
		stream << isUsed;

		if (isUsed) {
			data.Put(stream);
			stream << mapIndex;
		}
	}

	void GetChildRefs(std::set<Ref*>& refs) { data.GetChildRefs(refs); }

	void GetChildIndices(std::vector<int>& indices) { data.GetChildIndices(indices); }
};

class NiTexturingProperty : public NiProperty {
private:
	ushort flags = 0;
	uint textureCount = 0;

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

	uint numShaderTex = 0;
	std::vector<ShaderTexDesc> shaderTex;

public:
	static constexpr const char* BlockName = "NiTexturingProperty";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	NiTexturingProperty* Clone() override { return new NiTexturingProperty(*this); }
};

class NiVertexColorProperty : public NiProperty {
private:
	ushort flags = 0;

public:
	static constexpr const char* BlockName = "NiVertexColorProperty";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	NiVertexColorProperty* Clone() override { return new NiVertexColorProperty(*this); }
};

class NiDitherProperty : public NiProperty {
private:
	ushort flags = 0;

public:
	static constexpr const char* BlockName = "NiDitherProperty";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	NiDitherProperty* Clone() override { return new NiDitherProperty(*this); }
};

class NiFogProperty : public NiProperty {
private:
	ushort flags = 0;
	float fogDepth = 1.0f;
	Color3 fogColor;

public:
	static constexpr const char* BlockName = "NiFogProperty";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	NiFogProperty* Clone() override { return new NiFogProperty(*this); }
};

class NiWireframeProperty : public NiProperty {
private:
	ushort flags = 0;

public:
	static constexpr const char* BlockName = "NiWireframeProperty";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	NiWireframeProperty* Clone() override { return new NiWireframeProperty(*this); }
};

class NiZBufferProperty : public NiProperty {
private:
	ushort flags = 3;

public:
	static constexpr const char* BlockName = "NiZBufferProperty";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	NiZBufferProperty* Clone() override { return new NiZBufferProperty(*this); }
};

class NiShader : public NiProperty {
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
	virtual uint GetShaderType() { return 0; }
	virtual void SetShaderType(const uint) {}
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

class BSShaderProperty : public NiShader {
public:
	ushort shaderFlags = 1;
	BSShaderType shaderType = SHADER_DEFAULT;
	uint shaderFlags1 = 0x82000000;
	uint shaderFlags2 = 1;
	float environmentMapScale = 1.0f;

	uint numSF1 = 0;
	uint numSF2 = 0;
	std::vector<uint> SF1;
	std::vector<uint> SF2;

	Vector2 uvOffset;
	Vector2 uvScale = Vector2(1.0f, 1.0f);

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	uint GetShaderType() override;
	void SetShaderType(const uint type) override;
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

class WaterShaderProperty : public BSShaderProperty {
public:
	static constexpr const char* BlockName = "WaterShaderProperty";
	const char* GetBlockName() override { return BlockName; }

	WaterShaderProperty* Clone() override { return new WaterShaderProperty(*this); }
};

class HairShaderProperty : public BSShaderProperty {
public:
	static constexpr const char* BlockName = "HairShaderProperty";
	const char* GetBlockName() override { return BlockName; }

	HairShaderProperty* Clone() override { return new HairShaderProperty(*this); }
};

class DistantLODShaderProperty : public BSShaderProperty {
public:
	static constexpr const char* BlockName = "DistantLODShaderProperty";
	const char* GetBlockName() override { return BlockName; }

	DistantLODShaderProperty* Clone() override { return new DistantLODShaderProperty(*this); }
};

class BSDistantTreeShaderProperty : public BSShaderProperty {
public:
	static constexpr const char* BlockName = "BSDistantTreeShaderProperty";
	const char* GetBlockName() override { return BlockName; }

	BSDistantTreeShaderProperty* Clone() override { return new BSDistantTreeShaderProperty(*this); }
};

class TallGrassShaderProperty : public BSShaderProperty {
private:
	NiString fileName;

public:
	static constexpr const char* BlockName = "TallGrassShaderProperty";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	TallGrassShaderProperty* Clone() override { return new TallGrassShaderProperty(*this); }
};

class VolumetricFogShaderProperty : public BSShaderProperty {
public:
	static constexpr const char* BlockName = "VolumetricFogShaderProperty";
	const char* GetBlockName() override { return BlockName; }

	VolumetricFogShaderProperty* Clone() override { return new VolumetricFogShaderProperty(*this); }
};

class BSShaderTextureSet : public NiObject {
public:
	int numTextures = 13;
	std::vector<NiString> textures = std::vector<NiString>(13);

	BSShaderTextureSet() {}
	BSShaderTextureSet(NiVersion& version);

	static constexpr const char* BlockName = "BSShaderTextureSet";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSShaderTextureSet* Clone() override { return new BSShaderTextureSet(*this); }
};

class BSTextureArray {
private:
	uint textureArrayWidth = 0;
	std::vector<NiString> textureArray;

public:
	void Get(NiStream& stream) {
		stream >> textureArrayWidth;
		textureArray.resize(textureArrayWidth);

		for (int i = 0; i < textureArrayWidth; i++) {
			NiString tex;
			tex.Get(stream, 4);
			textureArray[i] = std::move(tex);
		}
	}

	void Put(NiStream& stream) {
		stream << textureArrayWidth;

		for (int i = 0; i < textureArrayWidth; i++)
			textureArray[i].Put(stream, 4, false);
	}
};

class BSLightingShaderProperty : public BSShaderProperty {
private:
	StringRef rootMaterialName;
	BlockRef<BSShaderTextureSet> textureSetRef;

public:
	Vector3 emissiveColor;
	float emissiveMultiple = 1.0f;
	uint textureClampMode = 3;
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
	uint numTextureArrays = 0;				   // User Version == 12, User Version 2 == 155
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

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::set<StringRef*>& refs) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	BSLightingShaderProperty* Clone() override { return new BSLightingShaderProperty(*this); }

	bool IsSkinTinted() override;
	bool IsFaceTinted() override;
	bool HasGlowmap() override;
	bool HasEnvironmentMapping() override;
	uint GetShaderType() override;
	void SetShaderType(const uint type) override;
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

class BSEffectShaderProperty : public BSShaderProperty {
public:
	NiString sourceTexture;
	uint textureClampMode = 0;
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

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSEffectShaderProperty* Clone() override { return new BSEffectShaderProperty(*this); }

	float GetEnvironmentMapScale() override;
	Color4 GetEmissiveColor() override;
	void SetEmissiveColor(const Color4& color) override;
	float GetEmissiveMultiple() override;
	void SetEmissiveMultiple(const float emissive) override;
};

class BSWaterShaderProperty : public BSShaderProperty {
private:
	uint waterFlags = 0;

public:
	static constexpr const char* BlockName = "BSWaterShaderProperty";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSWaterShaderProperty* Clone() override { return new BSWaterShaderProperty(*this); }
};

class BSSkyShaderProperty : public BSShaderProperty {
private:
	NiString baseTexture;
	uint skyFlags = 0;

public:
	static constexpr const char* BlockName = "BSSkyShaderProperty";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSSkyShaderProperty* Clone() override { return new BSSkyShaderProperty(*this); }
};

class BSShaderLightingProperty : public BSShaderProperty {
public:
	uint textureClampMode = 3; // User Version <= 11

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
};

enum SkyObjectType : uint {
	BSSM_SKY_TEXTURE,
	BSSM_SKY_SUNGLARE,
	BSSM_SKY,
	BSSM_SKY_CLOUDS,
	BSSM_SKY_STARS = 5,
	BSSM_SKY_MOON_STARS_MASK = 7
};

class SkyShaderProperty : public BSShaderLightingProperty {
private:
	NiString fileName;
	SkyObjectType skyObjectType = BSSM_SKY_TEXTURE;

public:
	static constexpr const char* BlockName = "SkyShaderProperty";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	SkyShaderProperty* Clone() override { return new SkyShaderProperty(*this); }
};

class TileShaderProperty : public BSShaderLightingProperty {
private:
	NiString fileName;

public:
	static constexpr const char* BlockName = "TileShaderProperty";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	TileShaderProperty* Clone() override { return new TileShaderProperty(*this); }
};

class BSShaderNoLightingProperty : public BSShaderLightingProperty {
public:
	NiString baseTexture;
	float falloffStartAngle = 1.0f;	  // User Version 2 > 26
	float falloffStopAngle = 0.0f;	  // User Version 2 > 26
	float falloffStartOpacity = 1.0f; // User Version 2 > 26
	float falloffStopOpacity = 1.0f;  // User Version 2 > 26

	static constexpr const char* BlockName = "BSShaderNoLightingProperty";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	BSShaderNoLightingProperty* Clone() override { return new BSShaderNoLightingProperty(*this); }

	bool IsSkinned();
	void SetSkinned(const bool enable);
};

class BSShaderPPLightingProperty : public BSShaderLightingProperty {
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

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	BSShaderPPLightingProperty* Clone() override { return new BSShaderPPLightingProperty(*this); }

	bool IsSkinned();
	void SetSkinned(const bool enable);
	int GetTextureSetRef();
	void SetTextureSetRef(const int texSetRef);
};

class Lighting30ShaderProperty : public BSShaderPPLightingProperty {
public:
	static constexpr const char* BlockName = "Lighting30ShaderProperty";
	const char* GetBlockName() override { return BlockName; }

	Lighting30ShaderProperty* Clone() override { return new Lighting30ShaderProperty(*this); }
};

class NiAlphaProperty : public NiProperty {
public:
	ushort flags = 4844;
	byte threshold = 128;

	static constexpr const char* BlockName = "NiAlphaProperty";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiAlphaProperty* Clone() override { return new NiAlphaProperty(*this); }
};


class NiMaterialProperty : public NiProperty {
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

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiMaterialProperty* Clone() override { return new NiMaterialProperty(*this); }

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

class NiStencilProperty : public NiProperty {
public:
	ushort flags = 19840;
	uint stencilRef = 0;
	uint stencilMask = 0xFFFFFFFF;

	static constexpr const char* BlockName = "NiStencilProperty";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiStencilProperty* Clone() override { return new NiStencilProperty(*this); }
};
} // namespace nifly
