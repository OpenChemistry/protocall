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

#include "rpcchannel.h"
#include "rpcvoiddata.h"
#include "servicemanager.h"
#include "servicedispatcher.h"
#include "responsehandler.h"
#include <google/protobuf/message.h>

#include <iostream>
#include <vector>
#include <sstream>


using namespace std;
using namespace google::protobuf;

namespace ProtoCall {
namespace Runtime {

RpcChannel::RpcChannel()
  : m_currentRequestId(0)
{

}

RpcChannel::~RpcChannel()
{

}

uint64 RpcChannel::nextRequestId()
{
  return m_currentRequestId++;
}

bool RpcChannel::send(const rpc::Message *messageEnvelope)
{
  if (!messageEnvelope) {
    this->setErrorString("RpcChannel::send passed NULL message");
    return false;
  }

  unsigned int size = static_cast<unsigned int>(messageEnvelope->ByteSize());
  RpcVoidData data(size);

  if(!messageEnvelope->SerializeToArray(data.data(), size)) {
    this->setErrorString("ProtoBuf: Error calling SerializeToArray");
    return false;
  }

  return this->send(&data);
}

bool RpcChannel::send(vtkDataObject *obj)
{
  this->setErrorString("Not supported, try a VTK subclass!");
  return false;
}

bool RpcChannel::receive(vtkDataObject *obj)
{
  this->setErrorString("Not supported, try a VTK subclass!");
  return false;
}

bool RpcChannel::send(vtkDataArray *array)
{
  this->setErrorString("Not supported, try a VTK subclass!");
  return false;
}

bool RpcChannel::receive(vtkDataArray *array)
{
  this->setErrorString("Not supported, try a VTK subclass!");
  return false;
}

void RpcChannel::registerResponseCallback(uint64 requestId,
    google:: protobuf::Message *response, ResponseHandler *handler,
    google::protobuf::Closure *done)
{
  ResponseCallbackEntry entry;
  entry.response = response;
  entry.done = done;
  entry.handler = handler;

  m_responseCallbacks.insert(CallBackRecord(requestId, entry));
}

bool RpcChannel::receive(rpc::Message *messageEnvelope)
{
  if (!messageEnvelope->ParseFromArray(m_data, m_size)) {
    this->setErrorString("ProtoBuf: Error calling ParseFromArray");
    return false;
  }

  return true;
}

bool RpcChannel::receive(bool nonBlocking)
{
  rpc::Message *msg = new rpc::Message();
  if (!this->receive(msg)) {
    delete msg;
    return false;
  }

  // handle will take ownership of the message
  return this->handleMessage(msg);
}

bool RpcChannel:: handleMessage(rpc::Message *messageEnvelope)
{
  bool rc = false;
  if (messageEnvelope->has_request()) {
    rc = handleRequest(messageEnvelope);
  }
  else if (messageEnvelope->has_response()) {
    rc = handleResponse(messageEnvelope);
  }
  else {
    this->setErrorString("Invalid message format");
  }

  return rc;
}

const FieldDescriptor *RpcChannel::extractDataField(Message *msg)
{

  // Loop over the fields and lookup the dispatcher
  std::vector<const FieldDescriptor *> fields;
  const Reflection *reflection = msg->GetReflection();
  const Message *constMessage = const_cast<const Message*>(msg);
  reflection->ListFields(*constMessage, &fields);

  const FieldDescriptor *descriptor = NULL;
  for(std::vector<const FieldDescriptor *>::iterator it = fields.begin();
    it != fields.end(); ++it) {
    descriptor = *it;

    // Should only be one extension, so we have it.
    if (descriptor->is_extension())
      break;
  }

  return descriptor;
}

bool RpcChannel::handleRequest(rpc::Message *messageEnvelope)
{
  ServiceManager *mgr = ServiceManager::instance();
  rpc::Request *request = messageEnvelope->mutable_request();

  const FieldDescriptor *descriptor = this->extractDataField(request);
  if (!descriptor) {
    this->setErrorString("Unable to extract data field from message");
    return false;
  }

  int methodId = descriptor->number();
  ServiceDispatcher *dispatcher = mgr->lookupServiceDispatcher(methodId);
  if(!dispatcher) {
    std::ostringstream msg;
    msg << "Can't load dispatcher for methodId: " << methodId;
    this->setErrorString(msg.str());
    return false;
  }

  const Reflection *reflection = request->GetReflection();
  Message *in
    = reflection->MutableMessage(request, descriptor);

  dispatcher->dispatch(methodId, messageEnvelope, in, this);

  return true;
}

bool RpcChannel::handleResponse(rpc::Message *messageEnvelope)
{
  rpc::Response *response = messageEnvelope->mutable_response();
  CallBackMap::iterator it = m_responseCallbacks.find(response->id());
  if (it == m_responseCallbacks.end()) {
    std::ostringstream msg;
    msg << "Received unexpected response with id: " << response->id();
    this->setErrorString(msg.str());
    return false;
  }

  const FieldDescriptor *descriptor = this->extractDataField(response);
  if (!descriptor) {
    this->setErrorString("Unable to extract data field");
    return false;
  }

  const Reflection *reflection = response->GetReflection();
  Message *data
    = reflection->MutableMessage(response, descriptor);

  ResponseCallbackEntry entry = it->second;

  int32 errorCode = 0;
  std::string errorString;

  if (response->has_errorcode())
    errorCode = response->errorcode();

  if (response->has_errorstring())
    errorString = response->errorstring();

  // Call response handler
  entry.handler->handle(this, descriptor->number(), data, entry.response,
      errorCode, errorString, entry.done);

  // remove from map
  m_responseCallbacks.erase(it);

  // delete the handler
  delete entry.handler;

  // Incoming message can now be cleaned up
  delete messageEnvelope;

  return true;
}

const std::string& RpcChannel::errorString()
{
  return this->m_errorString;
}

void RpcChannel::setErrorString(const string &errorString)
{
  this->m_error = 0;
  this->m_errorString = errorString;
}

} /* namespace Runtime */
} /* namespace ProtoCall */


