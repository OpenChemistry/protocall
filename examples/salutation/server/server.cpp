#include <vtkSocketCommunicator.h>
#include <iostream>
#include <protocall/runtime/vtkcommunicatorchannel.h>
#include "SalutationService.pb.h"
#include "SalutationService_Dispatcher.pb.h"
#include <google/protobuf/stubs/common.h>
#include <protocall/runtime/servicemanager.h>
#include "service.h"

int main(int argc, char *argv[])
{
  vtkSocketCommunicator *com = vtkSocketCommunicator::New();
  com->SetReportErrors(0);

  while(true) {
    if(!com->WaitForConnection(8888)) {
      cout << "Listen failure " << endl;
    }

    ProtoCall::Runtime::vtkCommunicatorChannel channel(com);
    ProtoCall::Runtime::ServiceManager *mgr = ProtoCall::Runtime::ServiceManager::instance();
    SalutationServiceImpl service;
    SalutationService::Dispatcher dispatcher(&service);
    mgr->registerService(&dispatcher);

    while(channel.receive());

    com->CloseConnection();
  }
}
