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

#ifndef PROTOCALL_COMPILER_UTILITIES_H
#define PROTOCALL_COMPILER_UTILITIES_H

#include <algorithm>
#include <vector>
#include <string>

namespace ProtoCall {
namespace Compiler {

bool isVoidType(std::string &type);

std::string toExtensionName(std::string methodName);

std::string replace(std::string &str, const std::string &search,
    const std::string &replace);

std::string externalTypeToClass(const std::string &name);

std::string externalTypeToHeaderPath(const std::string &type);

std::string extractClassName(const std::string &type);

std::vector<std::string> extractNamespaces(const std::string &type);

std::string toHeader(std::string type);
}
}

#endif
