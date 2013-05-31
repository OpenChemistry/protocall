/******************************************************************************

 This source file is part of the MoleQueue project.

 Copyright 2013 Kitware, Inc.

 This source code is released under the New BSD License, (the "License").

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 ******************************************************************************/

#ifndef DESERIALIZER_H_
#define DESERIALIZER_H_

namespace ProtoCall {
namespace Serialization {

class Deserializer
{
public:
  virtual ~Deserializer() {};

  virtual bool deserialize(const void *data, size_t size) = 0;
};

} /* namespace Serialization */
} /* namespace ProtoCall */

#endif /* DESERIALIZER_H_ */
