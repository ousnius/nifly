set(external_headers
    ${NIFLY_EXTERNAL_DIR}/half.hpp
    ${NIFLY_EXTERNAL_DIR}/Miniball.hpp
    )

set(headers
    ${NIFLY_INCLUDE_DIR}/Animation.hpp
    ${NIFLY_INCLUDE_DIR}/BasicTypes.hpp
    ${NIFLY_INCLUDE_DIR}/bhk.hpp
    ${NIFLY_INCLUDE_DIR}/ExtraData.hpp
    ${NIFLY_INCLUDE_DIR}/Factory.hpp
    ${NIFLY_INCLUDE_DIR}/Geometry.hpp
    ${NIFLY_INCLUDE_DIR}/Keys.hpp
    ${NIFLY_INCLUDE_DIR}/NifFile.hpp
    ${NIFLY_INCLUDE_DIR}/NifUtil.hpp
    ${NIFLY_INCLUDE_DIR}/Nodes.hpp
    ${NIFLY_INCLUDE_DIR}/Objects.hpp
    ${NIFLY_INCLUDE_DIR}/Particles.hpp
    ${NIFLY_INCLUDE_DIR}/Shaders.hpp
    ${NIFLY_INCLUDE_DIR}/Skin.hpp
    ${NIFLY_INCLUDE_DIR}/VertexData.hpp
    ${NIFLY_INCLUDE_DIR}/KDMatcher.hpp
    ${NIFLY_INCLUDE_DIR}/Object3d.hpp
    )

set(sources
    Animation.cpp
    BasicTypes.cpp
    bhk.cpp
    ExtraData.cpp
    Factory.cpp
    Geometry.cpp
    NifFile.cpp
    Nodes.cpp
    Objects.cpp
    Particles.cpp
    Shaders.cpp
    Skin.cpp
    Object3d.cpp
    )

add_library(nifly STATIC
    ${headers}
    ${sources}
    )

target_include_directories(nifly
    PUBLIC ${NIFLY_INCLUDE_DIR}
    SYSTEM ${NIFLY_EXTERNAL_DIR}
    )

target_compile_features(nifly PUBLIC cxx_std_17)


if(MSVC)
    target_compile_options(nifly PUBLIC "/EHsc" "/bigobj")
endif()
