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

const auto directoryDisposer = Approvals::useApprovalsSubdirectory("approval_tests");

// WIP: Implement some proper testing

TEST_CASE("NifFile.Load (not existing)", "[NifFile]") {
	NifFile test;
	test.Load("not_existing.nif");
	REQUIRE(!test.IsValid());
}

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
				file.Save("temp.nif");
			}
		}
	};

	process("INPUT/standard");
	opts.headParts = true;
	process("INPUT/headparts");
}
