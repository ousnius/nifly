cmake_minimum_required(VERSION 3.10..3.20)

project(nifly)

set(NIFLY_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(NIFLY_TESTS_DIR ${NIFLY_DIR}/tests)
set(NIFLY_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(NIFLY_EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)

set(NIFLY_VERSION 1.0.0)

add_subdirectory(src)

include(CTest)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif()
