if (APPLE)
  set(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBRARY_DIR}")
endif()

SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#to find our renamed version of boost
include_directories(${ProtoCall_SOURCE_DIR}/thirdparty)

add_executable(protoc-gen-protocallc
  rpcplugin.cpp
  rpcgenerator.cpp
  utilities.cpp)

if (WIN32 AND BUILD_SHARED_LIBS)
  set_target_properties(protoc-gen-protocallc PROPERTIES
                       COMPILE_DEFINITIONS "PROTOBUF_USE_DLLS")
endif()

target_link_libraries(protoc-gen-protocallc
  protobuf
  protoc_lib)

install(TARGETS protoc-gen-protocallc
  EXPORT "ProtoCallTargets"
  RUNTIME DESTINATION ${INSTALL_RUNTIME_DIR}
  LIBRARY DESTINATION ${INSTALL_LIBRARY_DIR}
  ARCHIVE DESTINATION ${INSTALL_ARCHIVE_DIR})
