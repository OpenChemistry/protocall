cmake_minimum_required(VERSION 2.8)

project(ProtoCall)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(InstallLocation)

find_package(protobuf REQUIRED NO_MODULE)
include_directories(${protobuf_INCLUDE_DIRS})

# To find up boost headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(runtime)
add_subdirectory(plugin)

# config files for build tree
set(ProtoCall_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
set(ProtoCall_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})
set(ProtoCall_TARGETS_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(ProtoCall_Proto_DIR ${CMAKE_CURRENT_SOURCE_DIR}/runtime)
configure_file(ProtoCallConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/ProtoCallConfig.cmake @ONLY)

# config files for install tree
set(ProtoCall_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(ProtoCall_INCLUDE_DIRS ${ProtoCall_INSTALL_PREFIX})
set(ProtoCall_CMAKE_DIR ${ProtoCall_INSTALL_PREFIX}/lib/cmake/protocall)
set(ProtoCall_TARGETS_DIR ${ProtoCall_CMAKE_DIR})
set(ProtoCall_Proto_DIR ${ProtoCall_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/${LIB_SUFFIX})

configure_file(ProtoCallConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/ProtoCallConfig.cmake @ONLY)

# export targets file in build tree
install(FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/protocall/serialization/serializer.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/protocall/serialization/deserializer.h"
  DESTINATION "${INSTALL_INCLUDE_DIR}/protocall/serialization")

install(FILES
  "${ProtoCall_BINARY_DIR}/CMakeFiles/ProtoCallConfig.cmake"
  DESTINATION "${INSTALL_LIBRARY_DIR}/cmake/protocall")

install(EXPORT "ProtoCallTargets"
  DESTINATION "${INSTALL_LIBRARY_DIR}/cmake/protocall")
