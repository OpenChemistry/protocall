#include "SalutationService.pb.h"
#ifndef RPCANNEL_H_
#define RPCANNEL_H_
class SalutationServiceImpl: public SalutationService {
  void sayHello(const Request* input, Response* output, google::protobuf::Closure* done);
  void sayBye(const Request* input, Response* output, google::protobuf::Closure* done);
  void sayVoid(::google::protobuf::Closure* done);
  void sayNotify(const Notification* input, google::protobuf::Closure* done);
  void sayHelloVTK(const RequestVTK*, ResponseVTK*, google::protobuf::Closure*);
};
#endif
