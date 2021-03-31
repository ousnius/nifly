/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#pragma once

#include "Animation.hpp"
#include "BasicTypes.hpp"
#include "ExtraData.hpp"

namespace nifly {
class NiObjectNET : public NiObjectCRTP<NiObjectNET, NiObject, true> {
public:
	NiStringRef name;

	bool bBSLightingShaderProperty = false;
	uint32_t bslspShaderType = 0; // BSLightingShaderProperty && User Version >= 12

	NiBlockRef<NiTimeController> controllerRef;
	NiBlockRefArray<NiExtraData> extraDataRefs;

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class NiProperty;
class NiCollisionObject;

class NiAVObject : public NiObjectCRTP<NiAVObject, NiObjectNET, true> {
public:
	uint32_t flags = 524302;
	/* "transform" is the coordinate system (CS) transform from this
	object's CS to its parent's CS.
	Recommendation: rename "transform" to "transformToParent". */
	MatTransform transform;

	NiBlockRefArray<NiProperty> propertyRefs;
	NiBlockRef<NiCollisionObject> collisionRef;

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	const MatTransform& GetTransformToParent() const { return transform; }
	void SetTransformToParent(const MatTransform& t) { transform = t; }
};

struct AVObject {
	NiString name;
	NiBlockPtr<NiAVObject> objectRef;
};

class NiAVObjectPalette : public NiObjectCRTP<NiAVObjectPalette, NiObject> {};

class NiDefaultAVObjectPalette : public NiObjectCRTP<NiDefaultAVObjectPalette, NiAVObjectPalette, true> {
protected:
	uint32_t numObjects = 0;
	std::vector<AVObject> objects;

public:
	NiBlockPtr<NiAVObject> sceneRef;

	static constexpr const char* BlockName = "NiDefaultAVObjectPalette";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<NiPtr*>& ptrs) override;

	std::vector<AVObject> GetAVObjects() const;
	void SetAVObjects(std::vector<AVObject>& avo);
};

class NiCamera : public NiObjectCRTP<NiCamera, NiAVObject, true> {
public:
	uint16_t obsoleteFlags = 0;
	float frustumLeft = 0.0f;
	float frustumRight = 0.0f;
	float frustumTop = 0.0f;
	float frustomBottom = 0.0f;
	float frustumNear = 0.0f;
	float frustumFar = 0.0f;
	bool useOrtho = false;
	float viewportLeft = 0.0f;
	float viewportRight = 0.0f;
	float viewportTop = 0.0f;
	float viewportBottom = 0.0f;
	float lodAdjust = 0.0f;

	NiBlockRef<NiAVObject> sceneRef;
	uint32_t numScreenPolygons = 0;
	uint32_t numScreenTextures = 0;

	static constexpr const char* BlockName = "NiCamera";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class NiSequenceStreamHelper : public NiObjectCRTP<NiSequenceStreamHelper, NiObjectNET> {
public:
	static constexpr const char* BlockName = "NiSequenceStreamHelper";
	const char* GetBlockName() override { return BlockName; }
};

class NiPalette : public NiObjectCRTP<NiPalette, NiObject, true> {
public:
	bool hasAlpha = false;
	NiVector<ByteColor4> palette = NiVector<ByteColor4>(256);

	static constexpr const char* BlockName = "NiPalette";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

enum PixelFormat : uint32_t {
	PX_FMT_RGB8,
	PX_FMT_RGBA8,
	PX_FMT_PAL8,
	PX_FMT_DXT1 = 4,
	PX_FMT_DXT5 = 5,
	PX_FMT_DXT5_ALT = 6,
};

enum ChannelType : uint32_t {
	CHNL_RED,
	CHNL_GREEN,
	CHNL_BLUE,
	CHNL_ALPHA,
	CHNL_COMPRESSED,
	CHNL_INDEX = 16,
	CHNL_EMPTY = 19,
};

enum ChannelConvention : uint32_t { CC_FIXED, CC_INDEX = 3, CC_COMPRESSED = 4, CC_EMPTY = 5 };

struct ChannelData {
	ChannelType type = CHNL_EMPTY;
	ChannelConvention convention = CC_EMPTY;
	uint8_t bitsPerChannel = 0;
	uint8_t unkByte1 = 0;
};

struct MipMapInfo {
	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t offset = 0;
};

class TextureRenderData : public NiObjectCRTP<TextureRenderData, NiObject, true> {
protected:
	uint32_t numMipmaps = 0;
	std::vector<MipMapInfo> mipmaps;

public:
	PixelFormat pixelFormat = PX_FMT_RGB8;
	uint8_t bitsPerPixel = 0;
	int unkInt1 = 0xFFFFFFFF;
	uint32_t unkInt2 = 0;
	uint8_t flags = 0;
	uint32_t unkInt3 = 0;

	ChannelData channels[4]{};
	NiBlockRef<NiPalette> paletteRef;

	uint32_t bytesPerPixel = 0;

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	std::vector<MipMapInfo> GetMipmaps() const;
	void SetMipmaps(std::vector<MipMapInfo>& mm);
};

class NiPersistentSrcTextureRendererData
	: public NiObjectCRTP<NiPersistentSrcTextureRendererData, TextureRenderData, true> {
public:
	uint32_t numPixels = 0;
	uint32_t unkInt4 = 0;
	uint32_t numFaces = 0;
	uint32_t unkInt5 = 0;

	std::vector<std::vector<uint8_t>> pixelData;

	static constexpr const char* BlockName = "NiPersistentSrcTextureRendererData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiPixelData : public NiObjectCRTP<NiPixelData, TextureRenderData, true> {
public:
	uint32_t numPixels = 0;
	uint32_t numFaces = 0;

	std::vector<std::vector<uint8_t>> pixelData;

	static constexpr const char* BlockName = "NiPixelData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

enum PixelLayout : uint32_t {
	PIX_LAY_PALETTISED,
	PIX_LAY_HIGH_COLOR_16,
	PIX_LAY_TRUE_COLOR_32,
	PIX_LAY_COMPRESSED,
	PIX_LAY_BUMPMAP,
	PIX_LAY_PALETTISED_4,
	PIX_LAY_DEFAULT
};

enum MipMapFormat : uint32_t { MIP_FMT_NO, MIP_FMT_YES, MIP_FMT_DEFAULT };

enum AlphaFormat : uint32_t { ALPHA_NONE, ALPHA_BINARY, ALPHA_SMOOTH, ALPHA_DEFAULT };

class NiTexture : public NiObjectCRTP<NiTexture, NiObjectNET> {};

class NiSourceTexture : public NiObjectCRTP<NiSourceTexture, NiTexture, true> {
public:
	bool useExternal = true;
	NiStringRef fileName;

	// NiPixelData if < 20.2.0.4 or !persistentRenderData
	// else NiPersistentSrcTextureRendererData
	NiBlockRef<TextureRenderData> dataRef;

	PixelLayout pixelLayout = PIX_LAY_PALETTISED_4;
	MipMapFormat mipMapFormat = MIP_FMT_DEFAULT;
	AlphaFormat alphaFormat = ALPHA_DEFAULT;
	bool isStatic = true;
	bool directRender = true;
	bool persistentRenderData = false;

	static constexpr const char* BlockName = "NiSourceTexture";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<NiStringRef*>& refs) override;
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class NiSourceCubeMap : public NiObjectCRTP<NiSourceCubeMap, NiSourceTexture> {
public:
	static constexpr const char* BlockName = "NiSourceCubeMap";
	const char* GetBlockName() override { return BlockName; }
};

enum TexFilterMode : uint32_t {
	FILTER_NEAREST,
	FILTER_BILERP,
	FILTER_TRILERP,
	FILTER_NEAREST_MIPNEAREST,
	FILTER_NEAREST_MIPLERP,
	FILTER_BILERP_MIPNEAREST
};

enum TexClampMode : uint32_t { CLAMP_S_CLAMP_T, CLAMP_S_WRAP_T, WRAP_S_CLAMP_T, WRAP_S_WRAP_T };

enum EffectType : uint32_t {
	EFFECT_PROJECTED_LIGHT,
	EFFECT_PROJECTED_SHADOW,
	EFFECT_ENVIRONMENT_MAP,
	EFFECT_FOG_MAP
};

enum CoordGenType : uint32_t {
	CG_WORLD_PARALLEL,
	CG_WORLD_PERSPECTIVE,
	CG_SPHERE_MAP,
	CG_SPECULAR_CUBE_MAP,
	CG_DIFFUSE_CUBE_MAP
};

class NiDynamicEffect : public NiObjectCRTP<NiDynamicEffect, NiAVObject, true> {
public:
	bool switchState = false;
	NiBlockPtrArray<NiNode> affectedNodes;

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<NiPtr*>& ptrs) override;
};

class NiTextureEffect : public NiObjectCRTP<NiTextureEffect, NiDynamicEffect, true> {
public:
	Matrix3 modelProjectionMatrix;
	Vector3 modelProjectionTranslation;
	TexFilterMode textureFiltering = FILTER_TRILERP;
	TexClampMode textureClamping = WRAP_S_WRAP_T;
	EffectType textureType = EFFECT_ENVIRONMENT_MAP;
	CoordGenType coordinateGenerationType = CG_SPHERE_MAP;
	NiBlockRef<NiSourceTexture> sourceTexture;
	uint8_t clippingPlane = 0;
	Vector3 unkVector = Vector3(1.0f, 0.0f, 0.0f);
	float unkFloat = 0.0f;

	static constexpr const char* BlockName = "NiTextureEffect";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<NiRef*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
};

class NiLight : public NiObjectCRTP<NiLight, NiDynamicEffect, true> {
public:
	float dimmer = 0.0f;
	Color3 ambientColor;
	Color3 diffuseColor;
	Color3 specularColor;

	void Sync(NiStreamReversible& stream);
};

class NiAmbientLight : public NiObjectCRTP<NiAmbientLight, NiLight> {
public:
	static constexpr const char* BlockName = "NiAmbientLight";
	const char* GetBlockName() override { return BlockName; }
};

class NiDirectionalLight : public NiObjectCRTP<NiDirectionalLight, NiLight> {
public:
	static constexpr const char* BlockName = "NiDirectionalLight";
	const char* GetBlockName() override { return BlockName; }
};

class NiPointLight : public NiObjectCRTP<NiPointLight, NiLight, true> {
public:
	float constantAttenuation = 0.0f;
	float linearAttenuation = 0.0f;
	float quadraticAttenuation = 0.0f;

	static constexpr const char* BlockName = "NiPointLight";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiSpotLight : public NiObjectCRTP<NiSpotLight, NiPointLight, true> {
public:
	float cutoffAngle = 0.0f;
	float unkFloat = 0.0f;
	float exponent = 0.0f;

	static constexpr const char* BlockName = "NiSpotLight";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};
} // namespace nifly
