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

#ifndef SERVICEDISPATCHER_H_
#define SERVICEDISPATCHER_H_

#include "proto/messages.pb.h"

namespace ProtoCall {
namespace Runtime {

class Service;
class RpcChannel;

struct ReplyInfo {
  int methodId;
  rpc::Message *requestMessageEnvelope;
  google::protobuf::Message *response;
  RpcChannel *replyChannel;
};

class ServiceDispatcher
{
public:
  ServiceDispatcher(Service *service);
  virtual void dispatch(int methodId, rpc::Message *requestMessageEnvelope,
      const google::protobuf::Message *request, RpcChannel *replyChannel) = 0;

  // Method to send reply and clean up request and response?
  virtual void reply(ReplyInfo info) = 0;
  virtual std::vector<int> implements() = 0;

protected:
  Service *m_service;
};

} /* namespace Runtime */
} /* namespace ProtoCall */

#endif /* SERVICEDISPATCHER_H_ */
