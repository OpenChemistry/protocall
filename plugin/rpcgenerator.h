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

#ifndef RCPGENERATOR_H_
#define RCPGENERATOR_H_

#include <string>
#include <vector>
#include <map>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>

using std::string;
using std::vector;

namespace ProtoCall {
namespace Compiler {

class RpcGenerator: public google::protobuf::compiler::CodeGenerator
{
public:
  RpcGenerator();
  // Override
  bool Generate(const google::protobuf::FileDescriptor * file,
                const string & parameter,
                google::protobuf::compiler::GeneratorContext * generator_context,
                string * error) const;
private:
  void generateCallMethod(const google::protobuf::ServiceDescriptor *descriptor,
                          google::protobuf::io::Printer &printer) const;

  void generateServiceHeader(google::protobuf::compiler::GeneratorContext *generator_context,
                             const google::protobuf::ServiceDescriptor *descriptor,
                             const string &protoFileName) const;

  void generateServiceProxyCc(google::protobuf::compiler::GeneratorContext *generator_context,
                          const google::protobuf::ServiceDescriptor *descriptor) const;

  void generateDispatcherHeader(google::protobuf::compiler::GeneratorContext *generator_context,
                             const google::protobuf::ServiceDescriptor *descriptor,
                             const string &protoFileName) const;

  void generateRelyMethod(const google::protobuf::ServiceDescriptor *descriptor,
      google::protobuf::io::Printer &printer) const;

  void generateDispatchMethodBody(const google::protobuf::ServiceDescriptor *descriptor,
      google::protobuf::io::Printer &printer) const;

  void generateDispatcherCc(google::protobuf::compiler::GeneratorContext *generator_context,
                          const google::protobuf::ServiceDescriptor *descriptor) const;

  void generateMethodSignature(const google::protobuf::MethodDescriptor *method,
                               google::protobuf::io::Printer &printer) const;

  void printHeaderGuardStart(google::protobuf::io::Printer &printer,
      const string &define) const;

  void printHeaderGuardEnd(google::protobuf::io::Printer &printer,
      const string &define) const;

  void generateImplementsMethod(const google::protobuf::ServiceDescriptor *descriptor,
      google::protobuf::io::Printer &printer) const;

  void generateSentVtkBlock(const string &variableName,
      const string &channelName, const google::protobuf::Descriptor *descriptor,
      google::protobuf::io::Printer &printer) const;

  void generateReceiveVtkBlock(const string &variableName,
      const string &channelName, const google::protobuf::Descriptor *descriptor,
      google::protobuf::io::Printer &printer) const;

  void generateResponseHandler(google::protobuf::compiler::GeneratorContext *generator_context,
      const google::protobuf::ServiceDescriptor *descriptor) const;

  void generateResponseHandlerHeader(google::protobuf::compiler::GeneratorContext *generator_context,
      const google::protobuf::ServiceDescriptor *descriptor) const;

  void generateResponseHandlerCpp(google::protobuf::compiler::GeneratorContext *generator_context,
      const google::protobuf::ServiceDescriptor *descriptor) const;

  void generateErrorCheck(const char *callText,
      std::map< string, string > & variables,
      google::protobuf::io::Printer &printer,
      const char *blockText = NULL) const;

  void generateErrorCheck(const char *callText,
      google::protobuf::io::Printer &printer,
      const char *blockText = NULL) const;

};

} /* End Compiler */
} /* End ProtoCall */

#endif /* RCPGENERATOR_H_ */
