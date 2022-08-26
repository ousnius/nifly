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

	stream.Sync(useSSR);
	stream.Sync(wetnessUseSSR);

	stream.Sync(decal);
	stream.Sync(doubleSided);
	stream.Sync(decalNoFade);
	stream.Sync(nonOccluder);

	stream.Sync(useRefraction);
	stream.Sync(refractionFalloff);
	stream.Sync(refractionStrength);

	stream.Sync(environmentMapping);
	if (stream.GetVersion() < 10)
		stream.Sync(envMapScale);

	stream.Sync(grayscaleToPaletteColor);

	if (stream.GetVersion() > 5)
		stream.Sync(maskWrites);

	switch (stream.GetHeader().GetType()) {
		case BgmType::BGSM:
			if (stream.GetVersion() < 17)
				SyncTextures(stream, 9);
			else
				SyncTextures(stream, 10);
			break;
		case BgmType::BGEM:
			if (stream.GetVersion() < 10)
				SyncTextures(stream, 5);
			else
				SyncTextures(stream, 8);
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
	stream.Sync(enableEditorAlphaRef);

	if (stream.GetVersion() > 7) {
		stream.Sync(useTranslucency);
		stream.Sync(translucencyThickObject);
		stream.Sync(translucencyMixAlbedoWithSubsurfaceColor);
		stream.Sync(translucencySubsurfaceColor);
		stream.Sync(translucencyTransmissiveScale);
		stream.Sync(translucencyTurbulence);
	}
	else {
		stream.Sync(useRimLighting);
		stream.Sync(rimlightPower);
		stream.Sync(backlightPower);
		stream.Sync(useSubsurfaceLighting);
		stream.Sync(subsurfaceRolloff);
	}

	stream.Sync(useSpecular);
	stream.Sync(specularColor);
	stream.Sync(specularStrength);
	stream.Sync(glossiness);
	stream.Sync(fresnelPower);

	stream.Sync(wetnessSpecScale);
	stream.Sync(wetnessSpecPower);
	stream.Sync(wetnessMinVar);
	if (stream.GetVersion() < 10)
		stream.Sync(wetnessEnvmapScale);
	stream.Sync(wetnessFresnelPower);
	stream.Sync(wetnessMetalness);

	if (stream.GetVersion() > 2)
		stream.Sync(isPbr);

	if (stream.GetVersion() > 9) {
		stream.Sync(useCustomPorosity);
		stream.Sync(porosityValue);
	}

	rootMaterialName.Sync(stream);

	stream.Sync(anisoLighting);

	stream.Sync(emmissive);
	if (emmissive)
		stream.Sync(emissiveColor);
	stream.Sync(emissiveMultiple);

	stream.Sync(modelSpaceNormals);
	stream.Sync(externalEmittance);

	if (stream.GetVersion() > 11)
		stream.Sync(lumEmittance);

	if (stream.GetVersion() > 12) {
		stream.Sync(useAdaptiveEmissive);
		stream.Sync(adaptiveEmissiveExposureOffset);
		stream.Sync(adaptiveEmissiveFinalExposureMin);
		stream.Sync(adaptiveEmissiveFinalExposureMax);
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

	stream.Sync(isHair);
	stream.Sync(hairTintColor);

	stream.Sync(isTree);
	stream.Sync(isFace);
	stream.Sync(isSkinTint);
	stream.Sync(tessellate);

	if (stream.GetVersion() == 1) {
		stream.Sync(displacementTextureBias);
		stream.Sync(displacementTextureScale);

		stream.Sync(tessellationPnScale);
		stream.Sync(tessellationBaseFactor);
		stream.Sync(tessellationFadeDistance);
	}

	stream.Sync(grayscaleToPaletteScale);
	stream.Sync(skewSpecularAlpha);

	if (stream.GetVersion() > 2) {
		stream.Sync(isTerrain);

		if (isTerrain) {
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
	if (stream.GetVersion() > 9) {
		stream.Sync(environmentMapping);
		stream.Sync(envMapScale);
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

	if (stream.GetVersion() > 10) {
		stream.Sync(emissiveColor);

		if (stream.GetVersion() > 14) {
			stream.Sync(adaptiveEmissiveExposureOffset);
			stream.Sync(adaptiveEmissiveFinalExposureMin);
			stream.Sync(adaptiveEmissiveFinalExposureMax);

			if (stream.GetVersion() > 15)
				stream.Sync(glowmap);

			if (stream.GetVersion() > 19)
				stream.Sync(effectPbrSpecular);
		}
	}
}
