cmake_minimum_required(VERSION 2.8)

project(ProtoCall)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(InstallLocation)

# To find up boost headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(protocall)

install(FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/protocall.proto.in"
  DESTINATION "${INSTALL_LIBRARY_DIR}/cmake/protocall")
