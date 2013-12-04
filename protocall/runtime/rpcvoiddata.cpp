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

#include "rpcvoiddata.h"
#include <vector>

namespace ProtoCall {
namespace Runtime {

struct RpcVoidData::InternalData
{
  InternalData():void_data(){}
  InternalData(unsigned int numbytes):void_data(numbytes){}
  std::vector<unsigned char> void_data;
};

RpcVoidData::RpcVoidData():
m_idata(new RpcVoidData::InternalData())
{
}

RpcVoidData::RpcVoidData(unsigned int numbytes):
m_idata(new RpcVoidData::InternalData(numbytes))
{
}

RpcVoidData::~RpcVoidData()
{
  delete m_idata;
}

unsigned int RpcVoidData::size() const
{
  return m_idata->void_data.size();
}

void RpcVoidData::resize(unsigned int numbytes)
{
  m_idata->void_data.resize(numbytes);
}

RpcVoidData::operator void* ()
{
  return &m_idata->void_data[0];
}

RpcVoidData::operator const void* () const
{
  return &m_idata->void_data[0];
}

}
}
