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

#include "runtime/vtkcommunicatorchannel.h"
#include <memory>
#include <vtkClientSocket.h>



namespace ProtoCall {
namespace Runtime {

#define SIZE_TAG 1234
#define MESSAGE_TAG 1235
#define DATAOBJECT_TAG 1236

VtkCommunicatorChannel::VtkCommunicatorChannel(vtkSocketCommunicator *communicator)
 : m_communicator(communicator)
{
  m_collection = vtkSocketCollection::New();
  m_collection->AddItem(m_communicator->GetSocket());
}

bool VtkCommunicatorChannel::send(const void *data, int size)
{
  if (!m_communicator->SendVoidArray(data, size, VTK_UNSIGNED_CHAR, 1,
      MESSAGE_TAG)) {
    this->setErrorString("VTK Error: Error calling SendVoidArray(void*)");
    return false;
  }

  return true;
}

bool VtkCommunicatorChannel::send(int size)
{
  if (!m_communicator->Send(const_cast<const int*>(&size), 1, 1, SIZE_TAG)) {
    this->setErrorString("VTK Error: Error calling Send");
    return false;
  }

  return true;
}

bool VtkCommunicatorChannel::send(const rpc::Message *msg)
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

  std::vector<unsigned char> data(size);
  if(!msg->SerializeToArray(data.data(), size)) {
    this->setErrorString("ProtoBuf Error: Error calling SerializeToArray");
    return false;
  }

  // First send the size
  if (!this->send(size)) {
    this->setErrorString("Error occurred sending message size");
    return false;
  }

  // Now send the message
  if (!this->send(data.data(), size)) {
    this->setErrorString("Error occurred sending message data");
    return false;
  }

  return true;
}

bool VtkCommunicatorChannel::send(::vtkDataObject *obj)
{
  if (!this->m_communicator->Send(obj, 1, DATAOBJECT_TAG)) {
    this->setErrorString("VTK Error: Error calling Sent(vtkDataObject*)");
    return false;
  }

  return true;
}

bool VtkCommunicatorChannel::receive(void *data, int size)
{
  if (!this->m_communicator->ReceiveVoidArray(data, size, VTK_UNSIGNED_CHAR, 1,
      MESSAGE_TAG)) {
    this->setErrorString("VTK Error: Error calling ReceiveVoidArray(void*)");
    return false;
  }

  return true;
}

bool VtkCommunicatorChannel::receive(int *size)
{
  if (!this->m_communicator->Receive(size, 1, 1, SIZE_TAG)) {
    this->setErrorString("VTK Error: Error calling Receive(int*)");
    return false;
  }

  return true;
}


// TODO move to super class
bool VtkCommunicatorChannel::receive(::rpc::Message *msg)
{
  int size;

  if (!this->receive(&size)) {
    this->setErrorString("Unable to receive message size");
    return false;
  }

  std::vector<unsigned char> data(size);
  if (!this->receive(data.data(), size)) {
    this->setErrorString("Unable to receive message data");
    return false;
  }

  if (!msg->ParseFromArray(data.data(), size)) {
    this->setErrorString("ProtoBuf Erro: Error calling ParseFromArray");
    return false;
  }

  return true;
}

bool VtkCommunicatorChannel::receive(bool nonBlocking)
{
  bool rc = true;

  while(!nonBlocking  && !select());

  if (select())
    rc = (RpcChannel::receive(nonBlocking) == 1);

  return rc;
}

bool VtkCommunicatorChannel::receive(::vtkDataObject *obj)
{
  if (!m_communicator->Receive(obj, 1, DATAOBJECT_TAG)) {
    this->setErrorString("VTK Errror: Error calling Receive(vtkDataObject*)");
    return false;
  }

  return true;
}

bool VtkCommunicatorChannel::select()
{
  return (m_collection->SelectSockets(300) == 1);
}

} /* namespace Runtime */
} /* namespace ProtoCall */
