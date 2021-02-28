Project(nifly)

cmake_minimum_required(VERSION 3.15)

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
    include/Shaders.hpp
    include/Skin.hpp
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

target_compile_features(nifly PUBLIC cxx_std_17)
if(MSVC)
    target_compile_options(nifly PUBLIC "/EHsc" "/bigobj")
endif()