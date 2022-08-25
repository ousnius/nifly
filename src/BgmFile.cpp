/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#include "BgmFile.hpp"

#include <fstream>

using namespace nifly;

void BgmFile::CopyFrom(const BgmFile& other) {
	Clear();

	if (auto m = GetShaderMaterial())
		material = std::make_unique<BgShaderMaterial>(*m);
	else if (auto m = GetEffectMaterial())
		material = std::make_unique<BgEffectMaterial>(*m);
}

int BgmFile::Load(const std::filesystem::path& fileName, const BgmLoadOptions& options) {
	std::ifstream file(fileName, std::ios::in | std::ios::binary);
	return Load(file, options);
}

int BgmFile::Load(std::istream& file, const BgmLoadOptions& options) {
	Clear();

	if (file) {
		IStream stream(&file);

		isEffect = options.isEffect;
		if (isEffect)
			material = std::make_unique<BgEffectMaterial>();
		else
			material = std::make_unique<BgShaderMaterial>();

		material->Sync(StreamReversible(&stream, nullptr, StreamReversible::Mode::Reading));
	}
	else {
		return 1;
	}

	isValid = true;
	return 0;
}

int BgmFile::Save(const std::filesystem::path& fileName, const BgmSaveOptions& options) {
	std::ofstream file(fileName, std::ios::out | std::ios::binary);
	return Save(file, options);
}

int BgmFile::Save(std::ostream& file, const BgmSaveOptions& options) {
	Clear();

	if (file && material) {
		OStream stream(&file);
		material->Sync(StreamReversible(nullptr, &stream, StreamReversible::Mode::Writing));
	}
	else
		return 1;

	return 0;
}

void BgmFile::Create(BgMaterialVersion version, BgMaterialType type) {
	Clear();

	switch (type) {
		case BgMaterialType::BGSM:
			material = std::make_unique<BgShaderMaterial>();
			material->version = version;
			isValid = true;
			break;
		case BgMaterialType::BGEM:
			material = std::make_unique<BgEffectMaterial>();
			material->version = version;
			isValid = true;
			isEffect = true;
			break;
	}
}

void BgmFile::Clear() {
	material.reset();
	isValid = false;
	isEffect = false;
}
