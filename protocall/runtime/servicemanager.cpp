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

#include "servicemanager.h"
#include "service.h"
#include "servicedispatcher.h"

#include <google/protobuf/descriptor.pb.h>
#include <iostream>
#include <algorithm>

namespace ProtoCall {
namespace Runtime {

ServiceManager::ServiceManager()
{

}

void ServiceManager::registerService(ServiceDispatcher *dispatcher)
{
  // TODO check for nulls
  std::vector<int> methodIds = dispatcher->implements();
  for(std::vector<int>::iterator it = methodIds.begin(); it != methodIds.end(); ++it) {
    this->m_dispatcherMap.insert(DispatcherMapEntry(*it, dispatcher));
  }

}


ServiceDispatcher *ServiceManager::lookupServiceDispatcher(int methodId)
{
  return this->m_dispatcherMap[methodId];
}

ServiceManager* ServiceManager::instance()
{
  static ServiceManager instance;
  return &instance;
}
}
}


