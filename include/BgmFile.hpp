/*
nifly
C++ NIF library for the Gamebryo/NetImmerse File Format
See the included GPLv3 LICENSE file
*/

#pragma once

#include "Materials.hpp"

#if __has_include(<filesystem>)

#include <filesystem>

#elif __has_include(<experimental/optional>)

#include <experimental/filesystem>
namespace std::filesystem {
using namespace std::experimental::filesystem;
}

#endif

// BgmFile load options
struct BgmLoadOptions {};

// BgmFile load options
struct BgmSaveOptions {};

namespace nifly {
class BgmFile {
private:
	BgmHeader hdr;
	std::unique_ptr<BgMaterial> material;
	bool isValid = false;

public:
	BgmFile() = default;

	BgmFile(const std::filesystem::path& fileName, const BgmLoadOptions& options = BgmLoadOptions()) {
		Load(fileName, options);
	}

	BgmFile(std::istream& file, const BgmLoadOptions& options = BgmLoadOptions()) { Load(file, options); }

	BgmFile(const BgmFile& other) { CopyFrom(other); }

	BgmFile& operator=(const BgmFile& other) {
		CopyFrom(other);
		return *this;
	}

	BgmHeader& GetHeader() { return hdr; }
	const BgmHeader& GetHeader() const { return hdr; }
	void CopyFrom(const BgmFile& other);

	int Load(const std::filesystem::path& fileName, const BgmLoadOptions& options = BgmLoadOptions());
	int Load(std::istream& file, const BgmLoadOptions& options = BgmLoadOptions());
	int Save(const std::filesystem::path& fileName, const BgmSaveOptions& options = BgmSaveOptions());
	int Save(std::ostream& file, const BgmSaveOptions& options = BgmSaveOptions());

	// Indicates that the file was fully loaded or otherwise initialized.
	bool IsValid() const { return isValid; }

	// Indicates if the file was loaded as a shader material.
	bool IsShader() const { return hdr.GetType() == BgmType::BGSM; }

	// Indicates if the file was loaded as an effect material.
	bool IsEffect() const { return hdr.GetType() == BgmType::BGEM; }

	// Creates a new file with with the specified version, and material type.
	void Create(BgmVersion version, BgmType type);

	// Deletes all underlying data, and resets the valid status.
	void Clear();

	// Returns BgMaterial pointer (or nullptr).
	// The underlying shader block type can differ.
	BgMaterial* GetMaterial() { return material.get(); }

	// Returns BgMaterial pointer (or nullptr).
	// The underlying shader block type can differ.
	const BgMaterial* GetMaterial() const { return material.get(); }

	// Returns BgShaderMaterial pointer (or nullptr).
	BgShaderMaterial* GetShaderMaterial() { return reinterpret_cast<BgShaderMaterial*>(material.get()); }

	// Returns BgShaderMaterial pointer (or nullptr).
	const BgShaderMaterial* GetShaderMaterial() const {
		return reinterpret_cast<BgShaderMaterial*>(material.get());
	}

	// Returns BgEffectMaterial pointer (or nullptr).
	BgEffectMaterial* GetEffectMaterial() { return reinterpret_cast<BgEffectMaterial*>(material.get()); }

	// Returns BgEffectMaterial pointer (or nullptr).
	const BgEffectMaterial* GetEffectMaterial() const {
		return reinterpret_cast<BgEffectMaterial*>(material.get());
	}
};
} // namespace nifly
