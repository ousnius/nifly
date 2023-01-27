find_package(Catch2 REQUIRED)

set(TARGETS
    TestNifFile
    #TestNifFileOptional
    )

foreach(name ${TARGETS})
    add_executable(${name} ${NIFLY_TESTS_DIR}/TestUtil.cpp ${NIFLY_TESTS_DIR}/${name}.cpp)
    target_link_libraries(${name} PRIVATE nifly Catch2::Catch2)
    set_target_properties(${name} PROPERTIES CXX_EXTENSIONS OFF)

    # Add warnings. -Werror disabled for now
    target_compile_options(${name} PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
        -Werror -Werror -Wall -Wextra -Wconversion -Wsign-conversion
        -Wno-shadow-field -Wno-c++98-compat -Wno-c++98-compat-pedantic>
        $<$<CXX_COMPILER_ID:GNU>:-Werror -Wall -Wextra -Wconversion -Wsign-conversion>
        $<$<CXX_COMPILER_ID:MSVC>: /WX /W4 /w44265>
        )

    include(Catch)
    catch_discover_tests(${name} WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
endforeach()
