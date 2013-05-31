include_directories("/home/cjh/protobuf-2.5.0/include")
link_directories("/home/cjh/protobuf-2.5.0/lib")

find_package(VTK COMPONENTS vtkParallelCore)

set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS ${VTK_DEFINITIONS})
include_directories(${VTK_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(runtime
  servicemanager.cpp
  rpcchannel.cpp
  responsehandler.cpp
  vtkcommunicatorchannel.cpp
  servicedispatcher.cpp
  ${MESSAGES_PB_SRC})
  

target_link_libraries(runtime protobuf vtkParallelCore)

PROTOBUF_GENERATE_CPP(MESSAGES_PB_SRC MESSAGES_PB_HDR proto/messages.proto)
add_custom_target(protobuf_header DEPENDS "${MESSAGES_PB_HDR}")
add_dependencies(runtime protobuf_header)

install(TARGETS runtime
  EXPORT "ProtoCallTargets"
  RUNTIME DESTINATION ${INSTALL_RUNTIME_DIR}
  LIBRARY DESTINATION ${INSTALL_LIBRARY_DIR}
  ARCHIVE DESTINATION ${INSTALL_ARCHIVE_DIR})

install(FILES
  vtkcommunicatorchannel.h
  rpcchannel.h
  rpcmanager.h
  DESTINATION ${INSTALL_INCLUDE_DIR})

install(FILES
  proto/messages.proto
  DESTINATION "${INSTALL_LIBRARY_DIR}/proto")
