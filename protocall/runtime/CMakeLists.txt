
install(FILES
  vtkcommunicatorchannel.h
  rpcchannel.h
  rpcvoiddata.h
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
  rpcvoiddata.cpp
  servicedispatcher.cpp)

set(_commuicator_src
   vtkcommunicatorchannel.cpp
   )

# Set in parent scope so we can using it when configuring the config file
set(ProtoCall_RUNTIME_SOURCE ${_src} PARENT_SCOPE)
set(ProtoCall_COMMUNICATOR_SOURCES ${_commuicator_src} PARENT_SCOPE)

install(FILES
  ${_src}
  ${_commuicator_src}
  DESTINATION "${INSTALL_SOURCE_DIR}/protocall/runtime")

