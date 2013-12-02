
install(FILES
  vtkcommunicatorchannel.h
  rpcchannel.h
  servicemanager.h
  service.h
  servicedispatcher.h
  responsehandler.h
  DESTINATION ${INSTALL_INCLUDE_DIR}/protocall/runtime)

install(FILES
  proto/protocall/messages.proto
  proto/protocall/void.proto
  proto/protocall/vtk.proto
  DESTINATION "${INSTALL_LIBRARY_DIR}/proto/protocall")

set(_src
  servicemanager.cpp
  rpcchannel.cpp
  servicedispatcher.cpp)

set(_vtk_src
   vtkcommunicatorchannel.cpp
   )

# Set in parent scope so we can using it when configuring the config file
set(ProtoCall_RUNTIME_SOURCE ${_src} PARENT_SCOPE)
set(ProtoCall_VTK_RUNTIME_SOURCE ${_vtk_src} PARENT_SCOPE)

install(FILES
  ${_src}
  DESTINATION "${INSTALL_SOURCE_DIR}/protocall/runtime")

