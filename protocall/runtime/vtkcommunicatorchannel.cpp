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

#include "vtkcommunicatorchannel.h"
#include "rpcvoiddata.h"

#include <memory>
#include <vtkClientSocket.h>



namespace ProtoCall {
namespace Runtime {

#define SIZE_TAG 1234
#define MESSAGE_TAG 1235
#define DATAOBJECT_TAG 1236
#define DATAARRAY_TAG 1237

vtkCommunicatorChannel::vtkCommunicatorChannel(vtkSocketCommunicator *communicator)
 : m_communicator(communicator)
{
  m_collection = vtkSocketCollection::New();
  m_collection->AddItem(m_communicator->GetSocket());
}

bool vtkCommunicatorChannel::send(const RpcVoidData *data)
{
  if (!this->send(data->size())) {
    this->setErrorString("Error occurred sending message size");
    return false;
  }

  if (!m_communicator->SendVoidArray(data, data->size(), VTK_UNSIGNED_CHAR, 1,
      MESSAGE_TAG)) {
    this->setErrorString("VTK Error: Error calling SendVoidArray(void*)");
    return false;
  }

  return true;
}

bool vtkCommunicatorChannel::send(unsigned int size)
{
  if (!m_communicator->Send(const_cast<const unsigned int*>(&size), 1, 1, SIZE_TAG)) {
    this->setErrorString("VTK Error: Error calling Send");
    return false;
  }

  return true;
}

bool vtkCommunicatorChannel::send(const rpc::Message *msg)
{
  if (!msg) {
    this->setErrorString("send passed a NULL message");
    return false;
  }

  int size = msg->ByteSize();

  if (size <= 0 ) {
    this->setErrorString("No data to send");
    return false;
  }

  RpcVoidData data(size);
  if(!msg->SerializeToArray(data, size)) {
    this->setErrorString("ProtoBuf Error: Error calling SerializeToArray");
    return false;
  }

  // First send the size
  if (!this->send(size)) {
    this->setErrorString("Error occurred sending message size");
    return false;
  }

  // Now send the message
  if (!this->send(&data)) {
    this->setErrorString("Error occurred sending message data");
    return false;
  }

  return true;
}

bool vtkCommunicatorChannel::send(vtkDataObject *obj)
{
  if (!this->m_communicator->Send(obj, 1, DATAOBJECT_TAG)) {
    this->setErrorString("VTK Error: Error calling Sent(vtkDataObject*)");
    return false;
  }

  return true;
}

bool vtkCommunicatorChannel::send(vtkDataArray *array)
{
  if (!this->m_communicator->Send(array, 1, DATAARRAY_TAG)) {
    this->setErrorString("VTK Error: Error calling Sent(vtkDataArray*)");
    return false;
  }

  return true;
}

//is size the size of the data* or is it the size that *data should
//be after we are done compression
bool vtkCommunicatorChannel::receive(RpcVoidData *data)
{
  unsigned int size;

  if (!this->receive(size)) {
    this->setErrorString("Unable to receive message size");
    return false;
  }

  data->resize(size);

  if (!this->m_communicator->ReceiveVoidArray(data, size, VTK_UNSIGNED_CHAR, 1,
      MESSAGE_TAG)) {
    this->setErrorString("VTK Error: Error calling ReceiveVoidArray(void*)");
    return false;
  }

  return true;
}

bool vtkCommunicatorChannel::receive(unsigned int& size)
{
  if (!this->m_communicator->Receive(&size, 1, 1, SIZE_TAG)) {
    this->setErrorString("VTK Error: Error calling Receive(int*)");
    return false;
  }

  return true;
}


// TODO move to super class
bool vtkCommunicatorChannel::receive(rpc::Message *msg)
{
  unsigned int size;

  if (!this->receive(size)) {
    this->setErrorString("Unable to receive message size");
    return false;
  }

  RpcVoidData data(size);
  if (!this->receive(data)) {
    this->setErrorString("Unable to receive message data");
    return false;
  }

  if (!msg->ParseFromArray(data, size)) {
    this->setErrorString("ProtoBuf Erro: Error calling ParseFromArray");
    return false;
  }

  return true;
}

bool vtkCommunicatorChannel::receive(vtkDataObject *obj)
{
  if (!m_communicator->Receive(obj, 1, DATAOBJECT_TAG)) {
    this->setErrorString("VTK Errror: Error calling Receive(vtkDataObject*)");
    return false;
  }

  return true;
}

bool vtkCommunicatorChannel::receive(vtkDataArray *array)
{
  if (!m_communicator->Receive(array, 1, DATAARRAY_TAG)) {
    this->setErrorString("VTK Errror: Error calling Receive(vtkDataArray*)");
    return false;
  }

  return true;
}


vtkSocketCommunicator *vtkCommunicatorChannel::communicator()
{
  return m_communicator;
}

bool vtkCommunicatorChannel::select()
{
  return (m_collection->SelectSockets(300) == 1);
}

bool vtkCommunicatorChannel::receive(bool nonBlocking)
{
  return RpcChannel::receive(nonBlocking);
}

} /* namespace Runtime */
} /* namespace ProtoCall */
