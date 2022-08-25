/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#include "Materials.hpp"

using namespace nifly;

void BgMaterial::Sync(BgmStreamReversible& stream) {
	int32_t tileFlags = (tileU << 1) | (tileV << 0);
	stream.Sync(tileFlags);
	tileU = tileFlags & 0x2;
	tileV = tileFlags & 0x1;

	stream.Sync(uvOffset);
	stream.Sync(uvScale);

	stream.Sync(alpha);
	stream.Sync(alphaBlend);
	stream.Sync(alphaSrc);
	stream.Sync(alphaDst);
	stream.Sync(alphaTestRef);
	stream.Sync(alphaTest);

	stream.Sync(zBufferWrite);
	stream.Sync(zBufferTest);

	stream.Sync(screenSpaceReflections);
	stream.Sync(wetnessControlScreenSpaceReflections);

	stream.Sync(decal);
	stream.Sync(twoSided);
	stream.Sync(decalNoFade);
	stream.Sync(nonOccluder);

	stream.Sync(refraction);
	stream.Sync(refractionFalloff);
	stream.Sync(refractionPower);

	stream.Sync(environmentMapping);
	if (stream.GetVersion() < 10)
		stream.Sync(environmentMappingMaskScale);

	if (stream.GetVersion() >= 6)
		stream.Sync(grayscaleToPaletteColor);

	stream.Sync(maskWrites);

	switch (stream.GetHeader().GetType()) {
		case BgmType::BGSM:
			if (stream.GetVersion() >= 17)
				SyncTextures(stream, 10);
			else
				SyncTextures(stream, 9);
			break;
		case BgmType::BGEM:
			if (stream.GetVersion() >= 10)
				SyncTextures(stream, 8);
			else
				SyncTextures(stream, 5);
			break;
	}
}

void BgMaterial::SyncTextures(BgmStreamReversible& stream, size_t count) {
	textures.resize(count);

	for (BgmString& str : textures) {
		str.Sync(stream);
	}
}


void BgShaderMaterial::Sync(BgmStreamReversible& stream) {
	BgMaterial::Sync(stream);

	stream.Sync(enableEditorAlphaRef);

	if (stream.GetVersion() >= 8) {
		stream.Sync(translucency);
		stream.Sync(translucencyThickObject);
		stream.Sync(translucencyMixAlbedoWithSubsurfaceCol);
		stream.Sync(translucencySubsurfaceColor);
		stream.Sync(translucencyTransmissiveScale);
		stream.Sync(translucencyTurbulence);
	}
	else {
		stream.Sync(rimLighting);
		stream.Sync(rimPower);
		stream.Sync(backlightPower);
		stream.Sync(subsurfaceLighting);
		stream.Sync(subsurfaceLightingRolloff);
	}

	stream.Sync(specularEnabled);
	stream.Sync(specularColor);
	stream.Sync(specularMult);
	stream.Sync(smoothness);
	stream.Sync(fresnelPower);

	stream.Sync(wetnessControlSpecScale);
	stream.Sync(wetnessControlSpecPowerScale);
	stream.Sync(wetnessControlSpecMinvar);
	if (stream.GetVersion() < 10)
		stream.Sync(wetnessControlEnvMapScale);
	stream.Sync(wetnessControlFresnelPower);
	stream.Sync(wetnessControlMetalness);

	if (stream.GetVersion() > 2)
		stream.Sync(pbr);

	if (stream.GetVersion() >= 9)
		stream.Sync(customPorosity);
	stream.Sync(porosityValue);

	rootMaterialPath.Sync(stream);

	stream.Sync(anisoLighting);
	stream.Sync(emitEnabled);

	if (emitEnabled)
		stream.Sync(emittanceColor);

	stream.Sync(emittanceMult);
	stream.Sync(modelSpaceNormals);

	stream.Sync(externalEmittance);

	if (stream.GetVersion() >= 12)
		stream.Sync(lumEmittance);

	if (stream.GetVersion() >= 13) {
		stream.Sync(useAdaptativeEmissive);
		stream.Sync(adaptativeEmissiveExposureOffset);
		stream.Sync(adaptativeEmissiveFinalExposureMin);
		stream.Sync(adaptativeEmissiveFinalExposureMax);
	}

	if (stream.GetVersion() < 8)
		stream.Sync(backLighting);

	stream.Sync(receiveShadows);
	stream.Sync(hideSecret);
	stream.Sync(castShadows);
	stream.Sync(dissolveFade);
	stream.Sync(assumeShadowmask);
	stream.Sync(glowmap);

	if (stream.GetVersion() < 7) {
		stream.Sync(environmentMappingWindow);
		stream.Sync(environmentMappingEye);
	}

	stream.Sync(hair);
	stream.Sync(hairTintColor);

	stream.Sync(tree);
	stream.Sync(facegen);
	stream.Sync(skinTint);
	stream.Sync(tessellate);

	if (stream.GetVersion() == 1)
		stream.Sync(displacementTextureBias);
	stream.Sync(displacementTextureScale);

	stream.Sync(tessellationPnScale);
	stream.Sync(tessellationBaseFactor);
	stream.Sync(tessellationFadeDistance);

	stream.Sync(grayscaleToPaletteScale);
	stream.Sync(skewSpecularAlpha);


	if (stream.GetVersion() >= 3) {
		stream.Sync(terrain);

		if (terrain) {
			if (stream.GetVersion() == 3) {
				uint32_t unused = 0;
				stream.Sync(unused);
			}

			stream.Sync(terrainThresholdFalloff);
			stream.Sync(terrainTilingDistance);
			stream.Sync(terrainRotationAngle);
		}
	}
}


void BgEffectMaterial::Sync(BgmStreamReversible& stream) {
	BgMaterial::Sync(stream);

	if (stream.GetVersion() >= 10) {
		stream.Sync(environmentMapping);
		stream.Sync(environmentMappingMaskScale);
	}

	stream.Sync(bloodEnabled);
	stream.Sync(effectLightingEnabled);

	stream.Sync(falloffEnabled);
	stream.Sync(falloffColorEnabled);

	stream.Sync(grayscaleToPaletteAlpha);
	stream.Sync(softEnabled);

	stream.Sync(baseColor);
	stream.Sync(baseColorScale);

	stream.Sync(falloffStartAngle);
	stream.Sync(falloffStopAngle);
	stream.Sync(falloffStartOpacity);
	stream.Sync(falloffStopOpacity);

	stream.Sync(lightingInfluence);
	stream.Sync(envmapMinLOD);
	stream.Sync(softDepth);

	if (stream.GetVersion() >= 11) {
		stream.Sync(emittanceColor);

		if (stream.GetVersion() >= 15) {
			stream.Sync(adaptativeEmissiveExposureOffset);
			stream.Sync(adaptativeEmissiveFinalExposureMin);
			stream.Sync(adaptativeEmissiveFinalExposureMax);

			if (stream.GetVersion() >= 16)
				stream.Sync(glowmap);

			if (stream.GetVersion() >= 20)
				stream.Sync(effectPbrSpecular);
		}
	}
}
