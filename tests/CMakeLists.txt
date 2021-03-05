project(nifly-tests)

cmake_minimum_required(VERSION 3.10)

set( HEADER_DIR ${CATCH_DIR}/single_include )

set( SOURCES_SINGLE_FILE
    TestCase.cpp
)

string( REPLACE ".cpp" "" BASENAMES_SINGLE_FILE "${SOURCES_SINGLE_FILE}" )

set( TARGETS_SINGLE_FILE ${BASENAMES_SINGLE_FILE} )

set( TARGETS_ALL
    ${TARGETS_SINGLE_FILE}
)

foreach( name ${TARGETS_SINGLE_FILE} )
    add_executable( ${name} ${TESTS_DIR}/${name}.cpp ${HEADER_DIR}/catch2/catch.hpp )
endforeach()

foreach( name ${TARGETS_ALL} )
    target_include_directories( ${name} PRIVATE ${HEADER_DIR} ${CATCH_DIR} ${NIFLY_DIR}/include ${NIFLY_DIR}/external )
    target_link_libraries( ${name} nifly )

    set_property(TARGET ${name} PROPERTY CXX_STANDARD 11)
    set_property(TARGET ${name} PROPERTY CXX_EXTENSIONS OFF)

    # Add desired warnings
    if ( CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU" )
        target_compile_options( ${name}  PRIVATE -Wall -Wextra -Wunreachable-code )
    endif()
    # Clang specific warning go here
    if ( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
        # Actually keep these
        target_compile_options( ${name}  PRIVATE -Wweak-vtables -Wexit-time-destructors -Wglobal-constructors -Wmissing-noreturn )
    endif()
    if ( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
        target_compile_options( ${name}  PRIVATE /W4 /w44265 /WX )
    endif()
endforeach()