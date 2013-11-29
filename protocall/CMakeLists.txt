find_package(protobuf REQUIRED)
include_directories(SYSTEM ${protobuf_INCLUDE_DIRS})

add_subdirectory(runtime)
add_subdirectory(plugin)

# config files for build tree
set(ProtoCall_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/protocall/runtime)
set(ProtoCall_LIBRARY_DIR
  ${CMAKE_BINARY_DIR}${INSTALL_LIBRARY_DIR}${LIB_SUFFIX})
set(ProtoCall_TARGETS_DIR ${CMAKE_BINARY_DIR})
set(ProtoCall_PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/runtime/proto)
set(ProtoCall_CMAKE_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(ProtoCall_SOURCE_DIR ${CMAKE_SOURCE_DIR}/protocall/runtime)
configure_file(ProtoCallConfig.cmake.in
  ${CMAKE_BINARY_DIR}/ProtoCallConfig.cmake @ONLY)

# config files for install tree
set(ProtoCall_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(ProtoCall_INCLUDE_DIRS ${ProtoCall_INSTALL_PREFIX}/${INSTALL_INCLUDE_DIR}
  ${ProtoCall_INSTALL_PREFIX}/${INSTALL_INCLUDE_DIR}/protocall/runtime)
set(ProtoCall_LIBRARY_DIR ${ProtoCall_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR})
set(ProtoCall_CMAKE_DIR
  ${ProtoCall_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}/cmake/protocall)
set(ProtoCall_TARGETS_DIR ${ProtoCall_CMAKE_DIR})
set(ProtoCall_PROTO_DIR ${ProtoCall_LIBRARY_DIR}/proto)
set(ProtoCall_SOURCE_DIR
  ${ProtoCall_INSTALL_PREFIX}/${INSTALL_SOURCE_DIR}/protocall/runtime)

# export targets file in build tree
export(TARGETS protoc-gen-protocallc
  FILE ${CMAKE_BINARY_DIR}/ProtoCallTargets.cmake)

# export targets for install tree
configure_file(ProtoCallConfig.cmake.in
  ${CMAKE_BINARY_DIR}/CMakeFiles/ProtoCallConfig.cmake @ONLY)

# export targets file in build tree
install(FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/serialization/serializer.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/serialization/deserializer.h"
  DESTINATION "${INSTALL_INCLUDE_DIR}/protocall/serialization")

install(FILES
  "${ProtoCall_BINARY_DIR}/CMakeFiles/ProtoCallConfig.cmake"
  DESTINATION "${INSTALL_LIBRARY_DIR}/cmake/protocall")

install(EXPORT "ProtoCallTargets"
  DESTINATION "${INSTALL_LIBRARY_DIR}/cmake/protocall")
