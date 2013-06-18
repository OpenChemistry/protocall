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

#ifndef RPCCHANNEL_H_
#define RPCCHANNEL_H_

#include <google/protobuf/stubs/common.h>
#include <stddef.h>
#include <map>

#include "proto/messages.pb.h"
#include "responsehandler.h"

class vtkDataObject;
class vtkDataArray;

namespace ProtoCall {
namespace Runtime {

using google::protobuf::uint64;

struct ResponseCallbackEntry
{
  google::protobuf::Closure *done;
  google:: protobuf::Message *response;
  ResponseHandler *handler;
};

class RpcChannel
{
public:
  RpcChannel();

  // TODO Make these private and use friendship?
  uint64  nextRequestId();
  virtual bool send(const rpc::Message *msg);
  virtual bool send(vtkDataObject *obj);
  virtual bool send(vtkDataArray *array);

  virtual bool receive(bool nonBlocking = true);
  virtual bool receive(vtkDataObject *obj);
  virtual bool receive(vtkDataArray *array);
  virtual bool send(const void *data, int size) = 0;
  virtual bool receive(void *data, int size) = 0;
  virtual bool send(unsigned int i) = 0;
  virtual bool receive(unsigned int *i) = 0;

  bool handleMessage(rpc::Message *messageEnvelope);
  bool handleRequest(rpc::Message *messageEnvelope);
  bool handleResponse(rpc::Message *messageEnvelope);

  void registerResponseCallback(uint64 requestId,
                                google:: protobuf::Message *response,
                                ResponseHandler *handler,
                                google::protobuf::Closure *done);


  const std::string& errorString();
  void setErrorString(const std::string &errorString);

  typedef std::pair<uint64,ResponseCallbackEntry> CallBackRecord;
  typedef std::map<uint64,ResponseCallbackEntry> CallBackMap;

protected:
  virtual bool receive(rpc::Message *msg);


private:
  uint64  m_currentRequestId;
  CallBackMap m_responseCallbacks;

  int m_error;
  std::string m_errorString;

  const google::protobuf::FieldDescriptor * extractDataField(
      google::protobuf::Message *msg);

  char *m_data;
  size_t m_size;

};

} /* namespace Runtime */
} /* namespace ProtoCall */

#endif /* RPCCHANNEL_H_ */
