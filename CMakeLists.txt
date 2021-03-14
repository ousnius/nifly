cmake_minimum_required(VERSION 3.10)

project(nifly)

add_library(nifly STATIC
    external/half.hpp
    external/Miniball.hpp
    include/Animation.hpp
    include/BasicTypes.hpp
    include/bhk.hpp
    include/ExtraData.hpp
    include/Factory.hpp
    include/Geometry.hpp
    include/Keys.hpp
    include/NifFile.hpp
    include/NifUtil.hpp
    include/Nodes.hpp
    include/Objects.hpp
    include/Particles.hpp
    include/Shaders.hpp include/Skin.hpp
    include/VertexData.hpp
    include/KDMatcher.hpp
    include/Object3d.hpp
    src/Animation.cpp
    src/BasicTypes.cpp
    src/bhk.cpp
    src/ExtraData.cpp
    src/Factory.cpp
    src/Geometry.cpp
    src/NifFile.cpp
    src/Nodes.cpp
    src/Objects.cpp
    src/Particles.cpp
    src/Shaders.cpp
    src/Skin.cpp
    src/Object3d.cpp
    )

set( NIFLY_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
target_include_directories(nifly PUBLIC ${NIFLY_DIR}/include ${NIFLY_DIR}/external)

target_compile_features(nifly PUBLIC cxx_std_17)
if(MSVC)
    target_compile_options(nifly PUBLIC "/EHsc" "/bigobj")
endif()

set( TESTS_DIR ${NIFLY_DIR}/tests )

add_subdirectory( ${TESTS_DIR} )
