#include "rpcplugin.h"
#include "rpcgenerator.h"
#include "common/utils.h"

#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/compiler/plugin.pb.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <boost/crc.hpp>

#include <iostream>
#include <sstream>
#include <list>
#include <climits>
#include <algorithm>

namespace ProtoCall {
namespace Compiler {

using namespace std;
using namespace google::protobuf;
using namespace google::protobuf::compiler;
using namespace google::protobuf::compiler::cpp;

static const int minExtensionNumber = 9;
static const int maxExtensionNumber = 536870911;
static const int numberOfReserverExtensionNumbers
  = FieldDescriptor::kLastReservedNumber
      - FieldDescriptor::kFirstReservedNumber + 1;
static const int availableExtensionNumbers
  = maxExtensionNumber - minExtensionNumber + 1
      - numberOfReserverExtensionNumbers;


//
// Taken from protobuf: google/protobuf/compiler/plugin.cc
//
class GeneratorResponseContext : public GeneratorContext {
 public:
  GeneratorResponseContext(CodeGeneratorResponse* response,
                           const vector<const FileDescriptor*>& parsed_files)
      : response_(response),
        parsed_files_(parsed_files) {}
  virtual ~GeneratorResponseContext() {}

  // implements GeneratorContext --------------------------------------

  virtual io::ZeroCopyOutputStream* Open(const string& filename) {
    CodeGeneratorResponse::File* file = response_->add_file();
    file->set_name(filename);
    return new io::StringOutputStream(file->mutable_content());
  }

  virtual io::ZeroCopyOutputStream* OpenForInsert(
      const string& filename, const string& insertion_point) {
    CodeGeneratorResponse::File* file = response_->add_file();
    file->set_name(filename);
    file->set_insertion_point(insertion_point);
    return new io::StringOutputStream(file->mutable_content());
  }

  void ListParsedFiles(vector<const FileDescriptor*>* output) {
    *output = parsed_files_;
  }

 private:
  CodeGeneratorResponse* response_;
  const vector<const FileDescriptor*>& parsed_files_;
};


RpcPlugin::RpcPlugin()
{
  m_pool.AllowUnknownDependencies();
}
RpcPlugin::~RpcPlugin()
{
}

bool RpcPlugin::isValidExtensionNumber(int number)
{        // in extension range
  return (minExtensionNumber < number && number < maxExtensionNumber ) &&
         // Not in reserved range
         !(number >= FieldDescriptor::kFirstReservedNumber &&
           number <= FieldDescriptor::kLastReservedNumber);
}

int RpcPlugin::generateExtensionNumber(string &methodName)
{
  boost::crc_32_type crc;
  crc.process_bytes(methodName.data(), methodName.length());

  unsigned int hash = crc.checksum();

//  cerr << "hash: " << hash << endl;
//  cerr << "minEx: " << minExtensionNumber << endl;

  int extensionNumber = minExtensionNumber +
    static_cast<int>(availableExtensionNumbers
      * (hash / static_cast<double>(UINT_MAX)));

//  cerr << "exten: " << extensionNumber << endl;

  if (!isValidExtensionNumber(extensionNumber))
    cerr << "Invalid extension number generated." << endl;

//  cerr << "extension" << endl;

  return extensionNumber;
}

void addExtension(FileDescriptorProto *rpcProto, const string &name,
    const string &extendee, int extensionNumber, const string &typeName)
{
  FieldDescriptorProto *extension = rpcProto->add_extension();
  extension->set_label(FieldDescriptorProto::LABEL_OPTIONAL);
  extension->set_type(FieldDescriptorProto::TYPE_MESSAGE);
  extension->set_name(name);
  extension->set_extendee(extendee);
  extension->set_number(extensionNumber);
  extension->set_type_name(typeName);
}

bool RpcPlugin::addRpcMessageExtensions(const google::protobuf::MethodDescriptor *method,
                                        FileDescriptorProto *rpcProto)
{
  //string serviceName = method->service()->full_name();
  string methodName = method->full_name();

  int extensionNumber = this->generateExtensionNumber(methodName);

  string extensionName =  Common::toExtensionName(methodName);

  // Add Request extension
  string requestTypeName = method->input_type()->full_name();
  addExtension(rpcProto, extensionName+"_request", "rpc.Request", extensionNumber,
      requestTypeName);

  // Add Response extension
  string responseTypeName = method->output_type()->full_name();
  if (!Common::isVoidType(responseTypeName))
    addExtension(rpcProto, extensionName+ "_response", "rpc.Response", extensionNumber,
        responseTypeName);

  return true;
}

bool RpcPlugin::addRpcMessageExtensions(const google::protobuf::FileDescriptor * file,
                                        FileDescriptorProto *rpcProto)
{
  string name = file->name();
  size_t index = name.find(".proto");
  if (index != string::npos)
    name = name.substr(0, index);

  rpcProto->set_name(name + "_rpc.proto");
  rpcProto->set_package(file->package());
  rpcProto->mutable_options()->CopyFrom(file->options());
  // TODO need to work out how to load this one ....
  //rpcProto->add_dependency("proto/messages.proto");
  rpcProto->add_dependency(file->name());

  for(int i=0; i<file->dependency_count(); i++)
    rpcProto->add_dependency(file->dependency(i)->name());

  for(int i=0; i<file->service_count(); i++) {
    const google::protobuf::ServiceDescriptor *descriptor = file->service(i);
    string serviceName = descriptor->full_name();
    for(int j=0; j<descriptor->method_count(); j++) {
      const MethodDescriptor *method = descriptor->method(j);
      addRpcMessageExtensions(method, rpcProto);
    }
  }
}

bool RpcPlugin::callGenerators(vector<const FileDescriptor *> *files,
  GeneratorResponseContext *context, const string &parameter, string &error)
{
  CppGenerator cppGenerator;
  RpcGenerator rpcGenerator;
  vector<const FileDescriptor *>::iterator it =  files->begin();
  for (; it != files->end(); it++) {
    const FileDescriptor *file = *it;

    if (!cppGenerator.Generate(file, parameter, context, &error))
      return false;

    if (!rpcGenerator.Generate(file, parameter, context, &error))
          return false;

  }

  return true;
}

void RpcPlugin::addVtkInserts(CodeGeneratorResponse &response)
{

  // Add vtkNew include
  google::protobuf::compiler::CodeGeneratorResponse_File *file
      = response.add_file();

  file->set_name("proto/vtk.pb.h");
  file->set_insertion_point("includes");
  file->set_content("#include <vtkNew.h>\n");

  // Add inserting point for vtk types
  const FileDescriptor *vtkDescriptors = m_pool.FindFileByName("proto/vtk.proto");

  for(int i=0; i<vtkDescriptors->message_type_count(); i++) {

    const Descriptor *type = vtkDescriptors->message_type(i);
    string vtkType = type->name();

    google::protobuf::compiler::CodeGeneratorResponse_File *file
        = response.add_file();

    file->set_name("proto/vtk.pb.h");
    file->set_insertion_point("includes");
    string include = string("#include <") + vtkType + ".h>\n";
    file->set_content(include);

    // Add inserting point for vtkDataObject accessors
    file = response.add_file();
    file->set_name("proto/vtk.pb.h");
    string insertionPoint = "class_scope:vtk.";
    insertionPoint += vtkType;
    file->set_insertion_point(insertionPoint);

    string insert;
    io::StringOutputStream stream(&insert);
    io::Printer printer(&stream, '$');


    printer.Print("private:\n");
    printer.Indent();
    printer.Print("class VTKObjectHolder {\n");
    printer.Print("public:\n");
    printer.Indent();
    printer.Print("VTKObjectHolder() : m_object(NULL), m_owns(false) {};\n");
    printer.Print("~VTKObjectHolder() {\n");
    printer.Indent();
    printer.Print("if (m_owns && m_object)\n");
    printer.Indent();
    printer.Print("m_object->Delete();\n");
    printer.Outdent();
    printer.Outdent();
    printer.Print("};\n");
    printer.Print("void set(::$vtkType$ *obj) { m_object = obj; };\n",
        "vtkType", vtkType);
    printer.Print("void set_allocated(::$vtkType$ *obj) { m_object = obj; m_owns = true; };\n",
            "vtkType", vtkType);
    printer.Print("::$vtkType$ *get() const { return m_object; };\n", "vtkType",
        vtkType);
    printer.Outdent();
    printer.Print("private:\n");
    printer.Indent();
    printer.Print("::$vtkType$ *m_object;\n", "vtkType", vtkType);
    printer.Print("bool m_owns;\n");
    printer.Outdent();
    printer.Print("};\n");
    printer.Print("VTKObjectHolder m_holder;");
    printer.Outdent();

    printer.Print("public:\n");
    printer.Indent();
    printer.Print("void set(::$vtkType$ *obj) { m_holder.set(obj); };\n",
        "vtkType", vtkType);
    printer.Print("void set_allocated(::$vtkType$ *obj) { m_holder.set_allocated(obj); };\n",
            "vtkType", vtkType);
    printer.Print("::$vtkType$ *get() const { return m_holder.get(); };\n", "vtkType",
        vtkType);
    printer.Outdent();

    file->set_content(insert.c_str());
  }
}

bool RpcPlugin::main(int argc, char *argv[])
{
  CodeGeneratorRequest request;
  request.ParseFromIstream(&cin);

  Parser parser;

  // Load all the files into the pool
  for(int i=0; i< request.proto_file_size(); i++) {
    const FileDescriptor *des = m_pool.BuildFile(request.proto_file(i));
    if (!des) {
      cerr << "protoc did not provide descriptor for: " << i;
      return false;
    }

    m_fileDescriptors.push_back(des);
  }

  vector<const FileDescriptor*> parsedFiles(m_fileDescriptors);

  CppGenerator cppGenerator;

  CodeGeneratorResponse response;

  vector<const FileDescriptor *>::iterator it =  m_fileDescriptors.begin();
  for (; it != m_fileDescriptors.end(); it++) {
    const FileDescriptor *file = *it;

    if (file->service_count() > 0) {
      FileDescriptorProto rpcExtensionsProto;
      this->addRpcMessageExtensions(file, &rpcExtensionsProto);
      const FileDescriptor *rpcExtensions
          = m_pool.BuildFile(rpcExtensionsProto);
      parsedFiles.push_back(rpcExtensions);
    }
  }
  string error;

  GeneratorResponseContext context(&response, parsedFiles);
  if (!this->callGenerators(&parsedFiles, &context, request.parameter(), error)) {
    response.set_error(error);
  }

  this->addVtkInserts(response);

  response.SerializePartialToOstream(&cout);

  return true;
}

}
}

int main(int argc, char *argv[])
{
  ProtoCall::Compiler::RpcPlugin plugin;
  plugin.main(argc, argv);
}


