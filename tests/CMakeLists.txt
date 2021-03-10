cmake_minimum_required(VERSION 3.10)

project(nifly-tests)

find_package(Catch2 REQUIRED)

set(SOURCES
	TestNifFile
	)

include(CTest)
include(Catch)

foreach(name ${SOURCES})
	add_executable(${name} src/${name}.cpp)

	target_link_libraries(${name} PRIVATE nifly Catch2::Catch2)

	set_property(TARGET ${name} PROPERTY CXX_EXTENSIONS OFF)

	# Add desired warnings
	if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
		target_compile_options(${name} PRIVATE -Wall -Wextra -Wunreachable-code)
	endif()
	# Clang specific warning go here
	if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		# Actually keep these
		target_compile_options(${name} PRIVATE -Wweak-vtables -Wexit-time-destructors -Wglobal-constructors -Wmissing-noreturn)
	endif()
	if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
		target_compile_options(${name} PRIVATE /W4 /w44265 /WX)
	endif()

	catch_discover_tests(${name} WORKING_DIRECTORY "${TESTS_DIR}/data")
endforeach()
