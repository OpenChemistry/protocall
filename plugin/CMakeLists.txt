add_executable(protoc-gen-protocallc
  rpcplugin.cpp
  rpcgenerator.cpp)

target_link_libraries(protoc-gen-protocallc
  protobuf
  protoc
  common)

install(TARGETS protoc-gen-protocallc
  EXPORT "ProtoCallTargets"
  RUNTIME DESTINATION ${INSTALL_RUNTIME_DIR}
  LIBRARY DESTINATION ${INSTALL_LIBRARY_DIR}
  ARCHIVE DESTINATION ${INSTALL_ARCHIVE_DIR})
