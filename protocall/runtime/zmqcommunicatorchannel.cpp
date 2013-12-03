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
#include "zmqCommunicatorChannel.h"

#include <zmq.h>
#include <google/protobuf/stubs/common.h> //for int64

#include <iostream>

#if ZMQ_VERSION_MAJOR == 2
//make the zmq 2 api look like the zmq 3 api
# define zmq_ctx_destroy(context) zmq_term(context)
# define zmq_msg_send(msg,sock,opt) zmq_send (sock, msg, opt)
# define zmq_msg_recv(msg,sock,opt) zmq_recv (sock, msg, opt)
# define ZMQ_DONTWAIT ZMQ_NOBLOCK
#endif

namespace ProtoCall {
namespace Runtime {


namespace detail
{
#if ZMQ_VERSION_MAJOR == 2
//need int64 for zmq socket options with version 2.
typedef google::protobuf::int64 zmqoption_t;
#else
//Version 3 and up is sane and uses ints
typedef int zmqoption_t;
#endif

//we don't need to worry about blocking since we can presume that the socket
//is already established.
inline int async_send(void* socket, zmq_msg_t& msg, int flags=0)
{
  short tries = 0;
  while(tries < 5)
    {
    int nbytes = zmq_msg_send(&msg, socket, flags|ZMQ_DONTWAIT);
    if (nbytes >= 0)
        { return nbytes; }
    if (zmq_errno () != EAGAIN)
        { return -1; }
    ++tries;
    }
  return -1;
}

inline int async_recv(void* socket, zmq_msg_t& msg, int flags=0)
{
  short tries = 0;
  while(tries < 5)
    {
    int nbytes = zmq_msg_recv(&msg, socket, flags|ZMQ_DONTWAIT);
    if (nbytes >= 0)
        { return nbytes; }
    if (zmq_errno () != EAGAIN)
        { return -1; }
    ++tries;
    }
  return -1;
}

inline bool isNotReqRepSocket(void *socket, int flags=0)
{
  zmqoption_t socketType;
  std::size_t socketTypeSize = sizeof(socketType);
  zmq_getsockopt(socket,ZMQ_TYPE,&socketType,&socketTypeSize);
  return (socketType != ZMQ_REQ && socketType != ZMQ_REP);
}

} //namespace detail


zmqCommunicatorChannel::socketIdentity::socketIdentity(const char* data,
                                                       std::size_t size):
  Size(size)
  {
  memcpy(Data,data,size);
  }

zmqCommunicatorChannel::socketIdentity::socketIdentity():
  Size(0)
  {

  }

bool zmqCommunicatorChannel::socketIdentity::operator ==(
                                               const socketIdentity& b) const
{
  if(this->size() != b.size()) { return false; }
  return 0 == (memcmp(this->data(),b.data(),this->size()));
}

bool zmqCommunicatorChannel::socketIdentity::operator<(
                                              const socketIdentity& b) const
{
  //sort first on size
  if(this->Size != b.size()) { return this->Size < b.size(); }
  //second sort on contents.

  const char* a_data = this->data();
  const char* b_data = b.data();
  std::size_t index=0;
  while(*a_data == *b_data && index++ < this->Size)
    { ++a_data; ++b_data; }

  if(index < this->Size)
  { return *a_data < *b_data; }

  return false; //both objects are equal
}


zmqCommunicatorChannel::zmqCommunicatorChannel(void* socket):
  m_socket(socket),
  m_sendIdentities()
{
}

bool zmqCommunicatorChannel::send(const rpc::Message *msg)
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

  //serialize the message before we send anything, that way
  //we don't send partial failed messages.
  zmq_msg_t msg_data;
  zmq_msg_init_size(&msg_data,size);
  if(!msg->SerializeToArray( zmq_msg_data(&msg_data) ,size)) {
    this->setErrorString("ProtoBuf Error: Error calling SerializeToArray");
    return false;
  }

  this->sendSocketIdentities();

  //frame 1 attach req header if required
  if(detail::isNotReqRepSocket(m_socket))
    {
    zmq_msg_t empty_msg;
    zmq_msg_init(&empty_msg);
    detail::async_send(m_socket,empty_msg, ZMQ_SNDMORE);
    zmq_msg_close(&empty_msg);
    }

  //frame 2
  int nbytes = detail::async_send(m_socket,msg_data);
  if(nbytes != size)
    {
    this->setErrorString("Unable to send the message using zmq");
    zmq_msg_close(&msg_data);
    return false;
    }

  zmq_msg_close(&msg_data);

  return true;
}

bool zmqCommunicatorChannel::send(const void *data, int size)
{
  if (size <= 0 ) {
    this->setErrorString("No data to send");
    return false;
  }

  zmq_msg_t msg_data;
  zmq_msg_init_size(&msg_data,size);
  memcpy(zmq_msg_data(&msg_data),data,size);

  //attach socket ids
  this->sendSocketIdentities();

  //frame 1 attach req header if required
  if(detail::isNotReqRepSocket(m_socket))
    {
    zmq_msg_t empty_msg;
    zmq_msg_init(&empty_msg);
    detail::async_send(m_socket,empty_msg, ZMQ_SNDMORE);
    zmq_msg_close(&empty_msg);
    }

  //frame 2
  int nbytes = detail::async_send(m_socket,msg_data);
  if(nbytes != size)
    {
    this->setErrorString("Unable to send the message using zmq");
    zmq_msg_close(&msg_data);
    return false;
    }

  zmq_msg_close(&msg_data);

  return true;
}

bool zmqCommunicatorChannel::receive(rpc::Message *msg)
{
  //we are sending our selves as a multi part message
  //frame 0: fake rep spacer
  //frame 1: rpc msg

  //remove the fake rep spacer if it exists
  if(detail::isNotReqRepSocket(m_socket))
    {
    zmq_msg_t empty_msg;
    zmq_msg_init(&empty_msg);
    detail::async_recv(m_socket,empty_msg);
    zmq_msg_close(&empty_msg);
    }

  zmq_msg_t msg_data;
  zmq_msg_init(&msg_data);
  int nbytes = detail::async_recv(m_socket,msg_data);
  if(nbytes == -1)
    {
    this->setErrorString("Incorrect amount of data received");
    zmq_msg_close(&msg_data);
    return false;
    }

  msg->ParseFromArray( zmq_msg_data(&msg_data),
                       zmq_msg_size(&msg_data) );
  zmq_msg_close(&msg_data);

  return true;
}


bool zmqCommunicatorChannel::receive(void *data, int size)
{
  //we are sending our selves as a multi part message
  //frame 0: fake rep spacer
  //frame 1: rpc msg

  //remove the fake rep spacer if it exists
  if(detail::isNotReqRepSocket(m_socket))
    {
    zmq_msg_t empty_msg;
    zmq_msg_init(&empty_msg);
    detail::async_recv(m_socket,empty_msg);
    zmq_msg_close(&empty_msg);
    }

  zmq_msg_t msg_data;
  int nbytes = detail::async_recv(m_socket,msg_data);
  if( nbytes != size)
    {
    zmq_msg_close(&msg_data);
    this->setErrorString("Incorrect amount of data received");
    return false;
    }

  memcpy(data, zmq_msg_data(&msg_data), size);
  zmq_msg_close(&msg_data);

  return true;
}


bool zmqCommunicatorChannel::receive(bool nonBlocking)
{
  return RpcChannel::receive(nonBlocking);
}


bool zmqCommunicatorChannel::sendSocketIdentities()
{
  //since we want to be able to support routing message
  //through different zmq routers we need to be able
  //to set multiple socket identities to route through
  for(std::vector<socketIdentity>::const_iterator i = m_sendIdentities.begin();
      i != m_sendIdentities.end();
      ++i)
    {
    zmq_msg_t msg_socket_id;
    zmq_msg_init_size(&msg_socket_id,i->size());
    memcpy( zmq_msg_data(&msg_socket_id),
            i->data(),
            i->size());
    detail::async_send(m_socket,msg_socket_id,ZMQ_SNDMORE);

    zmq_msg_close(&msg_socket_id);
    }
  return true;
}





} /* namespace Runtime */
} /* namespace ProtoCall */
