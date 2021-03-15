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
protected:
	StringRef name;

private:
	BlockRef<NiTimeController> controllerRef;
	BlockRefArray<NiExtraData> extraDataRefs;

public:
	uint32_t bslspShaderType = 0; // BSLightingShaderProperty && User Version >= 12
	bool bBSLightingShaderProperty = false;

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<StringRef*>& refs) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	std::string GetName();
	void SetName(const std::string& str);
	void ClearName();

	int GetControllerRef();
	void SetControllerRef(int ctlrRef);

	BlockRefArray<NiExtraData>& GetExtraData();
};

class NiProperty;
class NiCollisionObject;

class NiAVObject : public NiObjectCRTP<NiAVObject, NiObjectNET, true> {
protected:
	BlockRefArray<NiProperty> propertyRefs;
	BlockRef<NiCollisionObject> collisionRef;

public:
	uint32_t flags = 524302;
	/* "transform" is the coordinate system (CS) transform from this
	object's CS to its parent's CS.
	Recommendation: rename "transform" to "transformToParent". */
	MatTransform transform;

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	BlockRefArray<NiProperty>& GetProperties();

	int GetCollisionRef() { return collisionRef.GetIndex(); }
	void SetCollisionRef(const int colRef) { collisionRef.SetIndex(colRef); }

	const MatTransform& GetTransformToParent() const { return transform; }
	void SetTransformToParent(const MatTransform& t) { transform = t; }
};

struct AVObject {
	NiString name;
	BlockRef<NiAVObject> objectRef;
};

class NiAVObjectPalette : public NiObjectCRTP<NiAVObjectPalette, NiObject> {};

class NiDefaultAVObjectPalette : public NiObjectCRTP<NiDefaultAVObjectPalette, NiAVObjectPalette, true> {
private:
	BlockRef<NiAVObject> sceneRef;
	uint32_t numObjects = 0;
	std::vector<AVObject> objects;

public:
	static constexpr const char* BlockName = "NiDefaultAVObjectPalette";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetPtrs(std::set<Ref*>& ptrs) override;
};

class NiCamera : public NiObjectCRTP<NiCamera, NiAVObject, true> {
private:
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

	BlockRef<NiAVObject> sceneRef;
	uint32_t numScreenPolygons = 0;
	uint32_t numScreenTextures = 0;

public:
	static constexpr const char* BlockName = "NiCamera";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	int GetSceneRef();
	void SetSceneRef(int scRef);
};

class NiSequenceStreamHelper : public NiObjectCRTP<NiSequenceStreamHelper, NiObjectNET> {
public:
	static constexpr const char* BlockName = "NiSequenceStreamHelper";
	const char* GetBlockName() override { return BlockName; }
};

class NiPalette : public NiObjectCRTP<NiPalette, NiObject, true> {
private:
	bool hasAlpha = false;
	uint32_t numEntries = 256;
	std::vector<ByteColor4> palette = std::vector<ByteColor4>(256);

public:
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
private:
	PixelFormat pixelFormat = PX_FMT_RGB8;
	uint8_t bitsPerPixel = 0;
	int unkInt1 = 0xFFFFFFFF;
	uint32_t unkInt2 = 0;
	uint8_t flags = 0;
	uint32_t unkInt3 = 0;

	std::vector<ChannelData> channels = std::vector<ChannelData>(4);

	BlockRef<NiPalette> paletteRef;
	uint32_t numMipmaps = 0;
	uint32_t bytesPerPixel = 0;
	std::vector<MipMapInfo> mipmaps;

public:
	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	int GetPaletteRef();
	void SetPaletteRef(int palRef);
};

class NiPersistentSrcTextureRendererData : public NiObjectCRTP<NiPersistentSrcTextureRendererData, TextureRenderData, true> {
private:
	uint32_t numPixels = 0;
	uint32_t unkInt4 = 0;
	uint32_t numFaces = 0;
	uint32_t unkInt5 = 0;

	std::vector<std::vector<uint8_t>> pixelData;

public:
	static constexpr const char* BlockName = "NiPersistentSrcTextureRendererData";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiPixelData : public NiObjectCRTP<NiPixelData, TextureRenderData, true> {
private:
	uint32_t numPixels = 0;
	uint32_t numFaces = 0;

	std::vector<std::vector<uint8_t>> pixelData;

public:
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
private:
	bool useExternal = true;
	StringRef fileName;

	// NiPixelData if < 20.2.0.4 or !persistentRenderData
	// else NiPersistentSrcTextureRendererData
	BlockRef<TextureRenderData> dataRef;

	PixelLayout pixelLayout = PIX_LAY_PALETTISED_4;
	MipMapFormat mipMapFormat = MIP_FMT_DEFAULT;
	AlphaFormat alphaFormat = ALPHA_DEFAULT;
	bool isStatic = true;
	bool directRender = true;
	bool persistentRenderData = false;

public:
	static constexpr const char* BlockName = "NiSourceTexture";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetStringRefs(std::vector<StringRef*>& refs) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	int GetDataRef();
	void SetDataRef(int datRef);
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
private:
	bool switchState = false;
	BlockRefArray<NiNode> affectedNodes;

public:
	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	BlockRefArray<NiNode>& GetAffectedNodes();
};

class NiTextureEffect : public NiObjectCRTP<NiTextureEffect, NiDynamicEffect, true> {
private:
	Matrix3 modelProjectionMatrix;
	Vector3 modelProjectionTranslation;
	TexFilterMode textureFiltering = FILTER_TRILERP;
	TexClampMode textureClamping = WRAP_S_WRAP_T;
	EffectType textureType = EFFECT_ENVIRONMENT_MAP;
	CoordGenType coordinateGenerationType = CG_SPHERE_MAP;
	BlockRef<NiSourceTexture> sourceTexture;
	uint8_t clippingPlane = 0;
	Vector3 unkVector = Vector3(1.0f, 0.0f, 0.0f);
	float unkFloat = 0.0f;

public:
	static constexpr const char* BlockName = "NiTextureEffect";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	int GetSourceTextureRef();
	void SetSourceTextureRef(int srcTexRef);
};

class NiLight : public NiObjectCRTP<NiLight, NiDynamicEffect, true> {
private:
	float dimmer = 0.0f;
	Color3 ambientColor;
	Color3 diffuseColor;
	Color3 specularColor;

public:
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
private:
	float constantAttenuation = 0.0f;
	float linearAttenuation = 0.0f;
	float quadraticAttenuation = 0.0f;

public:
	static constexpr const char* BlockName = "NiPointLight";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};

class NiSpotLight : public NiObjectCRTP<NiSpotLight, NiPointLight, true> {
private:
	float cutoffAngle = 0.0f;
	float unkFloat = 0.0f;
	float exponent = 0.0f;

public:
	static constexpr const char* BlockName = "NiSpotLight";
	const char* GetBlockName() override { return BlockName; }

	void Sync(NiStreamReversible& stream);
};
} // namespace nifly
