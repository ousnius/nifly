/*
BodySlide and Outfit Studio
See the included LICENSE file
*/

#pragma once

#include "Animation.hpp"
#include "BasicTypes.hpp"
#include "ExtraData.hpp"

namespace nifly {
class NiObjectNET : public NiObject {
protected:
	StringRef name;

private:
	BlockRef<NiTimeController> controllerRef;
	BlockRefArray<NiExtraData> extraDataRefs;

public:
	uint32_t bslspShaderType = 0; // BSLightingShaderProperty && User Version >= 12
	bool bBSLightingShaderProperty = false;

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::set<StringRef*>& refs) override;
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

class NiAVObject : public NiObjectNET {
protected:
	BlockRefArray<NiProperty> propertyRefs;
	BlockRef<NiCollisionObject> collisionRef;

public:
	uint32_t flags = 524302;
	/* "transform" is the coordinate system (CS) transform from this
	object's CS to its parent's CS.
	Recommendation: rename "transform" to "transformToParent". */
	MatTransform transform;

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
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

class NiAVObjectPalette : public NiObject {};

class NiDefaultAVObjectPalette : public NiAVObjectPalette {
private:
	BlockRef<NiAVObject> sceneRef;
	uint32_t numObjects = 0;
	std::vector<AVObject> objects;

public:
	static constexpr const char* BlockName = "NiDefaultAVObjectPalette";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetPtrs(std::set<Ref*>& ptrs) override;
	NiDefaultAVObjectPalette* Clone() override { return new NiDefaultAVObjectPalette(*this); }
};

class NiCamera : public NiAVObject {
private:
	uint16_t obsoleteFlags;
	float frustumLeft;
	float frustumRight;
	float frustumTop;
	float frustomBottom;
	float frustumNear;
	float frustumFar;
	bool useOrtho;
	float viewportLeft;
	float viewportRight;
	float viewportTop;
	float viewportBottom;
	float lodAdjust;

	BlockRef<NiAVObject> sceneRef;
	uint32_t numScreenPolygons = 0;
	uint32_t numScreenTextures = 0;

public:
	static constexpr const char* BlockName = "NiCamera";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	NiCamera* Clone() override { return new NiCamera(*this); }

	int GetSceneRef();
	void SetSceneRef(int scRef);
};

class NiSequenceStreamHelper : public NiObjectNET {
public:
	static constexpr const char* BlockName = "NiSequenceStreamHelper";
	const char* GetBlockName() override { return BlockName; }

	NiSequenceStreamHelper* Clone() override { return new NiSequenceStreamHelper(*this); }
};

class NiPalette : public NiObject {
private:
	bool hasAlpha = false;
	uint32_t numEntries = 256;
	std::vector<ByteColor4> palette = std::vector<ByteColor4>(256);

public:
	static constexpr const char* BlockName = "NiPalette";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	NiPalette* Clone() override { return new NiPalette(*this); }
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

class TextureRenderData : public NiObject {
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
	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	int GetPaletteRef();
	void SetPaletteRef(int palRef);
};

class NiPersistentSrcTextureRendererData : public TextureRenderData {
private:
	uint32_t numPixels = 0;
	uint32_t unkInt4 = 0;
	uint32_t numFaces = 0;
	uint32_t unkInt5 = 0;

	std::vector<std::vector<uint8_t>> pixelData;

public:
	static constexpr const char* BlockName = "NiPersistentSrcTextureRendererData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	NiPersistentSrcTextureRendererData* Clone() override { return new NiPersistentSrcTextureRendererData(*this); }
};

class NiPixelData : public TextureRenderData {
private:
	uint32_t numPixels = 0;
	uint32_t numFaces = 0;

	std::vector<std::vector<uint8_t>> pixelData;

public:
	static constexpr const char* BlockName = "NiPixelData";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;

	NiPixelData* Clone() override { return new NiPixelData(*this); }
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

class NiTexture : public NiObjectNET {};

class NiSourceTexture : public NiTexture {
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

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetStringRefs(std::set<StringRef*>& refs) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	NiSourceTexture* Clone() override { return new NiSourceTexture(*this); }

	int GetDataRef();
	void SetDataRef(int datRef);
};

class NiSourceCubeMap : public NiSourceTexture {
public:
	static constexpr const char* BlockName = "NiSourceCubeMap";
	const char* GetBlockName() override { return BlockName; }

	NiSourceCubeMap* Clone() override { return new NiSourceCubeMap(*this); }
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

class NiDynamicEffect : public NiAVObject {
private:
	bool switchState = false;
	BlockRefArray<NiNode> affectedNodes;

public:
	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;

	BlockRefArray<NiNode>& GetAffectedNodes();
};

class NiTextureEffect : public NiDynamicEffect {
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

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	void GetChildRefs(std::set<Ref*>& refs) override;
	void GetChildIndices(std::vector<int>& indices) override;
	NiTextureEffect* Clone() override { return new NiTextureEffect(*this); }

	int GetSourceTextureRef();
	void SetSourceTextureRef(int srcTexRef);
};

class NiLight : public NiDynamicEffect {
private:
	float dimmer = 0.0f;
	Color3 ambientColor;
	Color3 diffuseColor;
	Color3 specularColor;

public:
	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
};

class NiAmbientLight : public NiLight {
public:
	static constexpr const char* BlockName = "NiAmbientLight";
	const char* GetBlockName() override { return BlockName; }

	NiAmbientLight* Clone() override { return new NiAmbientLight(*this); }
};

class NiDirectionalLight : public NiLight {
public:
	static constexpr const char* BlockName = "NiDirectionalLight";
	const char* GetBlockName() override { return BlockName; }

	NiDirectionalLight* Clone() override { return new NiDirectionalLight(*this); }
};

class NiPointLight : public NiLight {
private:
	float constantAttenuation = 0.0f;
	float linearAttenuation = 0.0f;
	float quadraticAttenuation = 0.0f;

public:
	static constexpr const char* BlockName = "NiPointLight";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiPointLight* Clone() override { return new NiPointLight(*this); }
};

class NiSpotLight : public NiPointLight {
private:
	float cutoffAngle = 0.0f;
	float unkFloat = 0.0f;
	float exponent = 0.0f;

public:
	static constexpr const char* BlockName = "NiSpotLight";
	const char* GetBlockName() override { return BlockName; }

	void Get(NiStream& stream) override;
	void Put(NiStream& stream) override;
	NiSpotLight* Clone() override { return new NiSpotLight(*this); }
};
} // namespace nifly
