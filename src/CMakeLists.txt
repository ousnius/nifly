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
    NifUtil.cpp
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

target_include_directories(nifly PUBLIC
    $<BUILD_INTERFACE:${NIFLY_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include/nifly>
    )

target_include_directories(nifly SYSTEM PUBLIC
    $<BUILD_INTERFACE:${NIFLY_EXTERNAL_DIR}>
    $<INSTALL_INTERFACE:include>
    )


target_compile_features(nifly PUBLIC cxx_std_17)

if(MSVC)
    target_compile_options(nifly PRIVATE "/Zc:inline")
    target_compile_options(nifly PUBLIC "/EHsc" "/bigobj")
endif()

install(DIRECTORY ${NIFLY_INCLUDE_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/nifly)
install(FILES
    ${NIFLY_EXTERNAL_DIR}/half.hpp
    ${NIFLY_EXTERNAL_DIR}/Miniball.hpp
  DESTINATION "${CMAKE_INSTALL_PREFIX}/include/nifly")
  
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  ${PROJECT_BINARY_DIR}/cmake/nifly-config-version.cmake
  VERSION ${NIFLY_VERSION}
  COMPATIBILITY AnyNewerVersion)

install(TARGETS nifly
  EXPORT nifly-targets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/nifly-config.cmake.in
  ${PROJECT_BINARY_DIR}/cmake/nifly-config.cmake
  INSTALL_DESTINATION cmake/})

install(EXPORT nifly-targets
  FILE nifly-targets.cmake
  DESTINATION cmake/)

install(FILES
    ${PROJECT_BINARY_DIR}/cmake/nifly-config.cmake
    ${PROJECT_BINARY_DIR}/cmake/nifly-config-version.cmake
  DESTINATION cmake/)
