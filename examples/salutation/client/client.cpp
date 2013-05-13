#include <vtkSocketCommunicator.h>
#include <iostream>
#include <runtime/vtkcommunicatorchannel.h>
#include "SalutationService.pb.h"
#include <google/protobuf/stubs/common.h>
#include <vtkTable.h>
#include <vtkNew.h>
#include <vtkFloatArray.h>
#include <vtkMutableDirectedGraph.h>

using namespace google::protobuf;

void callback(Response *response)
{
  std::cout << response->greeting() << endl;
}

void vtkCallback(ResponseVTK *response)
{
  response->myobject().get()->Dump();
  delete response;
}

void testSimple()
{
  vtkSocketCommunicator *com = vtkSocketCommunicator::New();
  if(!com->ConnectTo("localhost", 8888)) {
    cout << "Connection failure" << endl;
  }

//  Runtime::VtkCommunicatorChannel channel(com);
//  SalutationService::Proxy proxy(&channel);
//  Request request;
//  request.set_name("Chris");
//  Response response;
//  Closure *callback = NewCallback(&::callback, &response);
//  proxy.sayHello(&request, &response, callback);
//
//  channel.receive();

}

void testVtk()
{
vtkSocketCommunicator *com = vtkSocketCommunicator::New();
  if(!com->ConnectTo("localhost", 8888)) {
    cout << "Connection failure" << endl;
  }

  ProtoCall::Runtime::vtkCommunicatorChannel channel(com);
  SalutationService::Proxy proxy(&channel);

  for (int i=0; i< 100; i++) {
    RequestVTK requestVtk;

    // Create a table with some points in it...
    vtkNew<vtkTable> table;
    vtkNew<vtkFloatArray> arrX;
    arrX->SetName("X Axis");
    table->AddColumn(arrX.GetPointer());

    int numPoints = 69;
    float inc = 7.5 / (numPoints-1);
    table->SetNumberOfRows(numPoints);
    for (int i = 0; i < numPoints; ++i)
    {
      table->SetValue(i, 0, i * inc);
    }

    vtkNew<vtkMutableDirectedGraph> graph;
    vtkIdType a = graph->AddVertex();
    vtkIdType b = graph->AddChild(a);
    vtkIdType c = graph->AddChild(a);
    vtkIdType d = graph->AddChild(b);
    vtkIdType e = graph->AddChild(c);
    vtkIdType f = graph->AddChild(c);

    vtkNew<vtkTree> tree;
    tree->CheckedShallowCopy(graph.GetPointer());

    ResponseVTK *response = new ResponseVTK();

    requestVtk.mutable_myobject()->set(table.GetPointer());
    requestVtk.mutable_mytree()->set(tree.GetPointer());
    Closure *callback = NewCallback(&::vtkCallback, response);
    proxy.sayHelloVTK(&requestVtk, response, callback);

    channel.receive(false);
  }

  com->CloseConnection();
}

int main(int argc, char *argv[])
{

  testVtk();
}



