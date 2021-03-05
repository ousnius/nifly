#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include <NifFile.hpp>

using namespace nifly;

// WIP: Implement some proper testing

TEST_CASE("NifFile.Load (not existing)", "[NifFile]") {
	NifFile test;
	test.Load("not_existing.nif");
	REQUIRE(!test.IsValid());
}