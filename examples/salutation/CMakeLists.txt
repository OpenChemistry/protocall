cmake_minimum_required(VERSION 2.8)

project(RpcPluginImpl)


find_package(VTK COMPONENTS vtkParallelCore)

set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS ${VTK_DEFINITIONS})
include_directories(${VTK_INCLUDE_DIRS})

find_package(protobuf REQUIRED)
include_directories(${protobuf_INCLUDE_DIRS})

find_package(ProtoCall REQUIRED)
include_directories(SYSTEM ${ProtoCall_INCLUDE_DIRS})


protocallc(_common_source
  _service_source
  _proxy_source
  _include_dir
  "hello.proto")

#request that we find the vtk communicator and append it to the src files
protocall_communicator( vtk _common_source )


message(STATUS "_common_source: ${_common_source} ")

include_directories(${_include_dir})

add_library(runtime ${_common_source} )
target_link_libraries(runtime ${ProtoCall_LIBRARIES})

add_executable(client
  ${_proxy_source}
  client/client.cpp)

target_link_libraries(client runtime)

add_executable(server
  ${_service_source}
  server/service.cpp
  server/server.cpp)

target_link_libraries(server runtime)


