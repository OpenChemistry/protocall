//=============================================================================
//
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//
//=============================================================================

#ifndef ZMQCOMMUNICATORCHANNEL_H_
#define ZMQCOMMUNICATORCHANNEL_H_

#include "rpcchannel.h"

#include <cstddef>
#include <vector>

namespace ProtoCall {
namespace Runtime {


//Send rpc messages using ZMQ.
//We send each message as follows:
//
// frames 0..N: Socket Identities in pushed back order
// frame N+1: REQ Header [only exists if sockets isn't REP]
// frame N+2: RPC Data
//
// The reason that we include the REP null section, we
// ensure interoperability with REP sockets
//
//We expect received messages to be shaped as follows:
// frame 0: REP Header [only exists if sockets isn't REP]
// frame 1: RPC Data
//
// We will remove a REP header if it exists
// We currently don't handle forwarding messages which need to be
// routed somewhere else, that exercise is left to other developers.
class zmqCommunicatorChannel: public RpcChannel
{
public:

  //the sockets lifespan better be longer than this class
  zmqCommunicatorChannel(void* socket);

  bool send(const rpc::Message *msg);
  bool receive(bool nonBlocking = true);

protected:
  bool receive(rpc::Message *msg);

  //these methods we need to implement but we never use
  bool send(const void *data, int size);
  bool receive(void *data, int size);

public:
  //a simple socket identity class, that obeys zmq
  //restriction that a socket identity can't be more than 256 bytes
  class socketIdentity
  {
  public:
    socketIdentity(const char* data, std::size_t size);
    socketIdentity();

    bool operator ==(const socketIdentity& b) const;
    bool operator<(const socketIdentity& b) const;

    inline const char* data() const { return &Data[0]; }
    inline std::size_t size() const { return Size; }

  private:
    std::size_t Size;
    char Data[256];
  };

  //push back a socket identity to prefix to each message we send out
  //the order your push back is the order we append them. so if
  //you want the following routing order:
  // A | B | C | Final Dest
  //you want to call push_socket_identity in this order:
  // push_socket_identity(A)
  // push_socket_identity(B)
  // push_socket_identity(C)
  // push_socket_identity(Final Dest)
  void push_socket_identity(zmqCommunicatorChannel::socketIdentity sId)
    { m_sendIdentities.push_back(sId); }

  void clear_socket_identities()
    { m_sendIdentities.clear(); }

private:
  bool sendSocketIdentities();

  void* m_socket;
  std::vector<socketIdentity> m_sendIdentities;
};

} /* namespace Runtime */
} /* namespace ProtoCall */

#endif /* ZMQCOMMUNICATORCHANNEL_H_ */
