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

#ifndef RESPONSEHANDLER_H_
#define RESPONSEHANDLER_H_

#include <google/protobuf/message.h>

namespace ProtoCall {
namespace Runtime {

class RpcChannel;

class ResponseHandler
{
public:
  ResponseHandler();

  virtual void handle(RpcChannel *channel, int methodId, google::protobuf::Message *incomingResponse,
      google::protobuf::Message *targetResponse,
      google::protobuf::Closure *callback) = 0;
};

} /* namespace Runtime */
} /* namespace ProtoCall */

#endif /* RESPONSEHANDLER_H_ */
