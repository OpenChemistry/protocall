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

#include "utilities.h"

#include <algorithm>
#include <string>

using std::string;
using std::vector;

namespace ProtoCall {
namespace Compiler {

bool isVoidType(string &type)
{
  return type == "Void";
}

string toExtensionName(string methodName)
{
  replace(methodName.begin(), methodName.end(), '.', '_' );
  return methodName;
}

void replace(string &str, const string &search, const string &replace)
{
  size_t pos = 0;
  while((pos = str.find(search, pos)) != std::string::npos)
  {
    str.replace(pos, search.length(), replace);
    pos += replace.length();
  }
}

string externalTypeToClass(const string &name)
{
  string cls = name;
  string ext("external.");
  size_t index = cls.find(ext);
  if (index != string::npos)
    cls.erase(0, ext.length());

  replace(cls, ".", "::");

  return cls;
}

string externalTypeToHeaderPath(const string &type)
{
  string header;

  size_t start = type.find('.', 0) + 1;
  size_t end = 0;
  while((end = type.find('.', start)) != string::npos)
  {
    string name = type.substr(start, end - start);
    transform(name.begin(), name.end(),name.begin(),
      ::tolower);
    header += name;
    header += '/';
    start = end + 1;
  }

  return header;
}

string extractClassName(const string &type)
{
  string className;
  size_t pos = type.find_last_of('.');

  if (pos != string::npos)
    className = type.substr(pos+1, string::npos);

  return className;
}

vector<string> extractNamespaces(const string &type)
{
  vector<string> namespaces;

  size_t start = 0;
  size_t end = 0;
  while((end = type.find("::", start)) != std::string::npos)
  {
    namespaces.push_back(type.substr(start, end - start));
    start = end + 2;
  }

  return namespaces;
}

}
}
