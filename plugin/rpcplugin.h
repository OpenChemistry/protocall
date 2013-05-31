/******************************************************************************

 This source file is part of the ProtoCall project.

 Copyright 2013 Kitware, Inc.

 This source code is released under the New BSD License, (the "License").

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 ******************************************************************************/

#ifndef RPCPLUGIN_H_
#define RPCPLUGIN_H_

#include <string>
#include <vector>
#include <google/protobuf/compiler/plugin.pb.h>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/compiler/cpp/cpp_generator.h>



using std::string;
using std::vector;

namespace ProtoCall {
namespace Compiler {

class GeneratorResponseContext;

class RpcPlugin
{
public:
  RpcPlugin();
  ~RpcPlugin();
  static bool isValidExtensionNumber(int extensionNumber);

  static int generateExtensionNumber(string &methodName);

  bool addRpcMessageExtensions(const google::protobuf::MethodDescriptor *method,
      google::protobuf::FileDescriptorProto *rpcProto);

  bool addRpcMessageExtensions(const google::protobuf::FileDescriptor *file,
      google::protobuf::FileDescriptorProto *rpcProto);

  bool callGenerators(vector<const google::protobuf::FileDescriptor *> *files,
      GeneratorResponseContext *context, const string &parameter,
      string &error);

  void addVtkInserts(google::protobuf::compiler::CodeGeneratorResponse &response);
  void addExternalInserts(google::protobuf::compiler::CodeGeneratorResponse &response);
  void addExternalInsert(const google::protobuf::FileDescriptor *fileDes,
      google::protobuf::compiler::CodeGeneratorResponse &response);

  bool main(int argc, char *argv[]) ;

private:
  google::protobuf::DescriptorPool m_pool;
  vector<const google::protobuf::FileDescriptor *> m_fileDescriptors;

};

}
}

#endif /* RPCPLUGIN_H_ */
