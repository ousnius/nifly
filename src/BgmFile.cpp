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
	hdr.Clear();
	if (other.material)
		material = other.material->Clone();
	else
		material.reset();
}

int BgmFile::Load(const std::filesystem::path& fileName, const BgmLoadOptions& options) {
	std::ifstream file(fileName, std::ios::in | std::ios::binary);
	return Load(file, options);
}

int BgmFile::Load(std::istream& file, const BgmLoadOptions& options) {
	Clear();

	if (file) {
		BgmIStream stream(&file, &hdr);
		hdr.Get(stream);

		if (!hdr.IsValid()) {
			Clear();
			return 1;
		}

		isEffect = options.isEffect;
		if (isEffect)
			material = std::make_unique<BgEffectMaterial>();
		else
			material = std::make_unique<BgShaderMaterial>();

		material->Get(stream);
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
		BgmOStream stream(&file, &hdr);
		hdr.Put(stream);
		material->Put(stream);
	}
	else
		return 1;

	return 0;
}

void BgmFile::Create(BgmVersion version, BgmType type) {
	Clear();

	switch (type) {
		case BgmType::BGSM:
			material = std::make_unique<BgShaderMaterial>();
			isValid = true;
			break;
		case BgmType::BGEM:
			material = std::make_unique<BgEffectMaterial>();
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
