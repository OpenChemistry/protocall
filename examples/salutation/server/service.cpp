#include "service.h"
#include <iostream>
#include <sstream>
#include <vtkFloatArray.h>

void SalutationServiceImpl::sayHello(const Request* input, Response* output, ::google::protobuf::Closure* done)
{
  std::cerr << "Hello " << input->name() << endl;
  std::ostringstream greeting;
  greeting << "Hello " << input->name() << endl;
  output->set_greeting(greeting.str());
  done->Run();
}
void SalutationServiceImpl::sayBye(const Request* input, Response* output, ::google::protobuf::Closure* done)
{}
void SalutationServiceImpl::sayVoid(::google::protobuf::Closure* done)
{
  std::cout << "Touching the void..." << endl;
}
void SalutationServiceImpl::sayNotify(const Notification* input, ::google::protobuf::Closure* done)
{
  std::cout << "Note taken" << std::endl;
}

void SalutationServiceImpl::sayHelloVTK(const RequestVTK *input, ResponseVTK *out, ::google::protobuf::Closure* done)
{
  vtkTable *table = input->myobject().get();
  table->Dump();

  vtkIdType numPoints = table->GetNumberOfRows();

  vtkNew<vtkFloatArray> arrC;
  arrC->SetName("Cosine");
  arrC->SetNumberOfValues(numPoints);
  vtkNew<vtkFloatArray> arrS;
  arrS->SetName("Sine");
  arrS->SetNumberOfValues(numPoints);
  vtkNew<vtkFloatArray> arrS2;
  arrS2->SetName("Sine2");
  arrS2->SetNumberOfValues(numPoints);

  for (int i = 0; i < numPoints; ++i)
  {
    float inc = table->GetValue(i, 0).ToFloat();
    arrC->SetValue(i, cos(i * inc) + 0.0);
    arrS->SetValue(i, sin(i * inc) + 0.0);
    arrS2->SetValue(i, sin(i * inc) + 0.5);
  }

  table->AddColumn(arrC.GetPointer());
  table->AddColumn(arrS.GetPointer());
  table->AddColumn(arrS2.GetPointer());

  out->mutable_myobject()->set(table);
  done->Run();
}


