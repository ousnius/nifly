#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include <NifFile.hpp>

using namespace nifly;

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
