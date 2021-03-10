#define APPROVALS_CATCH

#include "Utils.hpp"

// WIP: Implement some proper testing

TEST_CASE("Load not existing file", "[NifFile]") {
	NifFile nif;
	REQUIRE(nif.Load("not_existing.nif") != 0);
}

TEST_CASE("Load and save file", "[NifFile]") {
	NifFile nif;
	REQUIRE(nif.Load("TestNifFile_LoadSave.nif") == 0);
	REQUIRE(nif.Save("TestNifFile_LoadSave_output.nif") == 0);
}

TEST_CASE("Load, optimize and save file", "[NifFile]") {
	OptOptions options;
	options.targetVersion = NiVersion::getSSE();

	NifFile nif;
	REQUIRE(nif.Load("TestNifFile_Optimize.nif") == 0);
	nif.OptimizeFor(options);
	REQUIRE(nif.Save("TestNifFile_Optimize_output.nif") == 0);
}

////// Approval tests


TEST_CASE("Optimize lots of meshes") {
	OptOptions opts;
	opts.removeParallax = false;
	opts.targetVersion = NiVersion::getSSE();
	ExceptionCollector exceptions; // Allow all files to be written, regardless of errors
	const auto process = [&opts](const auto& dir) {
		for (auto& p : fs::recursive_directory_iterator(dir)) {
			SECTION(p.path().u8string()) {
				if (!p.is_regular_file())
					continue;

				if (p.path().extension() != ".nif")
					continue;

				NifFile file;
				REQUIRE_FALSE(file.Load(p.path()));
				SECTION("load") { Approvals::verify(file); }
				SECTION("optimize1") { Approvals::verify(file.OptimizeFor(opts)); };
				SECTION("optimize2") { Approvals::verify(file); }
				std::stringstream temp;
				REQUIRE_FALSE(file.Save(temp));
			}
		}
	};

	process("approval/standard");
	opts.headParts = true;
	process("approval/headparts");
}
