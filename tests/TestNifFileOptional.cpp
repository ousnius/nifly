#define CATCH_CONFIG_MAIN

#include "TestNifFile.hpp"
#include "TestUtil.hpp"

#include <catch2/catch.hpp>

#include <NifFile.hpp>

using namespace nifly;

TEST_CASE("Load all files", "[NifFile]") {
	constexpr auto dirName = "scaninput";

	if (!std::filesystem::exists(dirName))
	{
		WARN("Input directory missing.");
		return;
	}

	const std::filesystem::path scanPath{dirName};

	auto countIter = std::filesystem::recursive_directory_iterator{scanPath};
	size_t fileCount = std::count_if(
		begin(countIter),
		end(countIter),
		[](auto& entry) { return entry.is_regular_file() && entry.path().extension() == ".nif"; }
	);

	size_t n = 0;
	int oldPercent = 0;

	for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{scanPath}) 
	{
		if (dir_entry.is_regular_file())
		{
			auto const& path = dir_entry.path();
			if (path.extension() == ".nif")
			{
				NifFile nif;
				REQUIRE(nif.Load(path) == 0);

				//std::cout << path << " loaded!" << std::endl;

				n++;

				int percent = (int)((n / (double)fileCount) * 100.0);
				if (percent != oldPercent)
				{
					std::cout << percent << "% processed..." << std::endl;
				}
				oldPercent = percent;
			}
		}
	}
}

TEST_CASE("Scan files for mismatching BSX flags (external emittance)", "[NifFile]") {
	constexpr auto dirName = "scaninput";

	if (!std::filesystem::exists(dirName))
	{
		WARN("Input directory missing.");
		return;
	}

	const std::filesystem::path scanPath{dirName};

	auto countIter = std::filesystem::recursive_directory_iterator{scanPath};
	size_t fileCount = std::count_if(
		begin(countIter),
		end(countIter),
		[](auto& entry) { return entry.is_regular_file() && entry.path().extension() == ".nif"; }
	);

	size_t n = 0;
	int oldPercent = 0;

	for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{scanPath}) 
	{
		if (dir_entry.is_regular_file())
		{
			auto const& path = dir_entry.path();
			if (path.extension() == ".nif")
			{
				NifFile nif;
				REQUIRE(nif.Load(path) == 0);
				
				BSXFlags* bsx = nif.FindBlockByName<BSXFlags>("BSX");
				if (bsx && !(bsx->integerData & BSX_EXTERNAL_EMITTANCE))
				{
					bool flagMissing = false;

					for (const auto shape : nif.GetShapes())
					{
						const auto shader = nif.GetShader(shape);
						if (shader)
						{
							const auto bslsp = dynamic_cast<BSLightingShaderProperty*>(shader);
							if (bslsp)
							{
								if (bslsp->shaderFlags1 & SLSF1_EXTERNAL_EMITTANCE)
								{
									std::cout << path << " has BSXFlags block without external emittance, but shader flags of shape '" << shape->name.get() << "' have external emittance enabled!" << std::endl;
									flagMissing = true;
								}
							}
						}
					}

					REQUIRE_FALSE(flagMissing);
				}

				n++;

				int percent = (int)((n / (double)fileCount) * 100.0);
				if (percent != oldPercent)
				{
					std::cout << percent << "% processed..." << std::endl;
				}
				oldPercent = percent;
			}
		}
	}
}

TEST_CASE("Scan files for mismatching shader flags (environment mapping)", "[NifFile]") {
	constexpr auto dirName = "scaninput";

	if (!std::filesystem::exists(dirName))
	{
		WARN("Input directory missing.");
		return;
	}

	const std::filesystem::path scanPath{dirName};

	auto countIter = std::filesystem::recursive_directory_iterator{scanPath};
	size_t fileCount = std::count_if(
		begin(countIter),
		end(countIter),
		[](auto& entry) { return entry.is_regular_file() && entry.path().extension() == ".nif"; }
	);

	size_t n = 0;
	int oldPercent = 0;

	for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{scanPath}) 
	{
		if (dir_entry.is_regular_file())
		{
			auto const& path = dir_entry.path();
			if (path.extension() == ".nif")
			{
				NifFile nif;
				REQUIRE(nif.Load(path) == 0);

				for (const auto shape : nif.GetShapes())
				{
					const auto shader = nif.GetShader(shape);
					if (shader)
					{
						const auto bslsp = dynamic_cast<BSLightingShaderProperty*>(shader);
						if (bslsp)
						{
							if (bslsp->bslspShaderType != BSLSP_ENVMAP && (bslsp->shaderFlags1 & SLSF1_ENVIRONMENT_MAPPING))
							{
								std::cout << path << " has environment mapping flag without matching shader type (shape '" << shape->name.get() << "')" << std::endl;
							}

							if (bslsp->bslspShaderType == BSLSP_ENVMAP && !(bslsp->shaderFlags1 & SLSF1_ENVIRONMENT_MAPPING))
							{
								std::cout << path << " has environment mapping shader type without matching shader flag (shape '" << shape->name.get() << "')" << std::endl;
							}
						}
					}
				}

				n++;

				int percent = (int)((n / (double)fileCount) * 100.0);
				if (percent != oldPercent)
				{
					std::cout << percent << "% processed..." << std::endl;
				}
				oldPercent = percent;
			}
		}
	}
}
