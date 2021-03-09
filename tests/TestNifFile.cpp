#define APPROVALS_CATCH // This tells Approval Tests to provide a main() - only do this in one cpp file

#include "ApprovalTests.v.10.8.0.hpp"
#include <catch2/catch.hpp>

#include <NifFile.hpp>

using namespace nifly;
namespace fs = std::filesystem;
using namespace ApprovalTests;

template<>
std::string ApprovalTests::StringMaker::toString(const NifFile& contents) {
	std::stringstream stream;
	auto copy = contents;
	copy.Save(stream);
	return stream.str();
}

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

const auto directoryDisposer = Approvals::useApprovalsSubdirectory("approval_tests");

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

				NifFile file(p.path());
				SECTION("load") { Approvals::verify(file, Options(AutoApproveIfMissingReporter())); }
				file.OptimizeFor(opts);
				SECTION("optimize") { Approvals::verify(file, Options(AutoApproveIfMissingReporter())); }
			}
		}
	};

	process("INPUT/standard");
	opts.headParts = true;
	process("INPUT/headparts");
}
