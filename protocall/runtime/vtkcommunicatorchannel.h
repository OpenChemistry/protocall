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

#ifndef VTKCOMMUNICATORCHANNEL_H_
#define VTKCOMMUNICATORCHANNEL_H_

#include "rpcchannel.h"

#include <vtkSocketCommunicator.h>
#include <vtkSocketCollection.h>

namespace ProtoCall {
namespace Runtime {

class vtkCommunicatorChannel: public RpcChannel
{
public:
  vtkCommunicatorChannel(vtkSocketCommunicator *communicator);

  bool send(const rpc::Message *msg);
  bool send(vtkDataObject *obj);
  bool send(vtkDataArray *array);
  bool send(const RpcVoidData *data);


  bool receive(vtkDataObject *obj);
  bool receive(vtkDataArray *array);
  bool receive(RpcVoidData *data);

  bool receive(bool nonBlocking = true);

  bool select();

  vtkSocketCommunicator * communicator();

protected:
  bool receive(unsigned int& size);
  bool send(unsigned int size);

  bool receive(rpc::Message *msg);
private:
  vtkSocketCommunicator *m_communicator;
  vtkSocketCollection *m_collection;



};

} /* namespace Runtime */
} /* namespace ProtoCall */

#endif /* VTKCOMMUNICATORCHANNEL_H_ */
