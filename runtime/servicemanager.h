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

#ifndef SERVICEMANAGER_H_
#define SERVICEMANAGER_H_

#include <google/protobuf/descriptor.h>
#include <map>

namespace ProtoCall {
namespace Runtime {

class ServiceDispatcher;

class ServiceManager
{
public:
  static ServiceManager *instance();

  ServiceManager();

  void registerService(ServiceDispatcher *dispatcher);

  ServiceDispatcher *lookupServiceDispatcher(int methodId);

private:
  typedef std::map<int, Runtime::ServiceDispatcher *> DispatcherMap;
  typedef std::pair<int, Runtime::ServiceDispatcher *> DispatcherMapEntry;

  DispatcherMap m_dispatcherMap;

};
}
}

#endif /* SERVICEMANAGER_H_ */
