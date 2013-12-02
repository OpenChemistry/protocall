## Overview

ProtoCall uses protobuf to provide an asynchronous RPC framework for execution of remote C++ methods. Services are defined in the protocol buffer language which are then used to generate proxy code and service stubs via a protoc plugin. The commuication is performed used a [vtkSocketCommunicator](http://www.vtk.org/doc/nightly/html/classvtkSocketCommunicator.html)

<img src="https://raw.github.com/OpenChemistry/protocall/master/ProtoCall.png" width=70%>

## Installation and Build

# Dependencies

1. CMake
2. VTK ( needs vtkParallelCore module for communicator )
3. protobuf

# Building protobuf
ProtoCall required protobuf as one of it dependences, so the first setup if to clone and buld protobuf. A CMake enable version have be clone as follows:

    git clone https://github.com/OpenChemistry/protocall.git

The project can then be build like any other CMake project.

# Building ProtoCall
Clone the ProtoCall repository using the following command:
   git clone http://git.kwsource.kitwarein.com/protocall/protocall.git

The following variables need to be defined:

 -   VTK_DIR - The path to your VTK build tree or directory containing VTKConfig.cmake.
 -   protobuf_DIR - The path to your protobuf build tree or directory containing protobufConfig.cmake

Once these are define the project can be configured and built. This will generate a ProtoCallConfig.cmake that can be used in project that need to use ProtoCall.

## Defining a Service

Services are defined using the the [protocol buffer language](https://developers.google.com/protocol-buffers/docs/proto). The .proto file must import "protocall.proto" this import includes the internal RPC message envelopes and other internal types such as the VTK wrappers.  The file is them make up of message definitions, defining type that inputs or output of RPC calls and service definition that define the signatures of a set of RPC calls.

The following restrictions currently apply:

- The package construct can't be used in ProtoCall ( this can change in the future ).

# Defining messages
An example of a simple message definition is given below or detail can be found in the following [language guide](https://developers.google.com/protocol-buffers/docs/proto):

    message MyRequest {

      optional string myMessage = 1;

    }

# Using VTK types
The following VTK types are supports:

- vtkBitArray
- vtkCharArray
- vtkDoubleArray
- vtkFloatArray
- vtkIdTypeArray
- vtkIntArray
- vtkLongArray
- vtkLongLongArray
- vtkShortArray
- vtkSignedCharArray
- vtkUnsignedCharArray
- vtkUnsignedIntArray
- vtkUnsignedLongArray
- vtkUnsignedLongLongArray
- vtkUnsignedShortArray
- vtkImagaData
- vtkPolyData
- vtkSelection
- vtk[Un]structuredGrid
- vtkTable
- vtkTree

To define a field in a message the type is simply vtk.<type>. For example the following defines a message containing a vtkTable:
    import "protocall.proto"

    ...

    message MyVTKRequest {

       optional vtk.vtkTable myTable = 1;

    }

The following restrictions apply to VTK types:

- They must appear in the top level of a message they can't be nested within another message type, For example the following is not valid:

         message MyVTKNest {

            optional vtk.vtkTable myTable = 1;

         }


         message MyVTKRequest {

           optional MyVTKNest myNest = 1;

        }

- Field rules are currently ignored so repeated VTK field are not supported.

# Defining Services

RPC service can define an options intput or output message type, corresponding to a request/reply message. Here is an example service definition alone with is message types:

    import "protocall.proto";

    message RequestVTK {

      optional vtk.vtkTable myTable = 1;

    }

    message ResponseVTK {

      optional vtk.vtkTable myReturnTable = 1;

    }

    service SalutationService {

      rpc sayHelloVTK(RequestVTK) returns(ResponseVTK);

    }

This service defines a single method sayHelloVTK that take  RequestVTK message and returns a ResponseVTK message.

If a service doesn't want to define either an input or output message for example with a notification the special message type of Void can be used. For example, the following service doesn't use a output message :

    import "protocall.proto";

    message Notification {

      optional string status = 1;

    }

    service NotificationService {

      rpc notify(Notification) returns(Void);

    }

## Generate service stubs and proxy code

Once a service and its associated messages have been defined protoc alone with the ProtoCall plugin need to be used to generate the proxy code and service stubs. The can be done used the CMake macro provide in ProtoCallConfig.cmake, it has the following signature:

    macro(protocallc common_source service_source proxy_source include_dir)

The following variable need to be provided to the macro and will be populated by it:

- common_source - The list of source files generated that are shared between the proxy (client) and service (server)
- service_source - The list of source files generated that need to be include in an service implementation.
- proxy_source - The list of source files generated that make up the proxy.
- include_dir - The directory containing the generated headers, needs to be add to include search path.
- The proto files are provided after these parameters.

The invocation generate serveral header and source files. However, the important one in thems of implementing or calling a service is the service header file which will have the following name:

    <service_name>.pb.h

Where service_name is the name defined in service definition in the .proto file. The best way to describe what this file contains is through a simple example, for the following .proto file:
    ...

    service SalutationService {

      rpc sayHello(Request) returns(Response);

    }

The header file will look something like this:

    ...

    class SalutationService_Proxy;

    // Service definition

    class SalutationService: public ProtoCall::Runtime::Service {

    public:

      virtual void sayHello(const Request* input, Response* output, google::protobuf::Closure* done) = 0;

      // typedef to make things look pretty for client

      typedef SalutationService_Proxy Proxy;
    };

    // The proxy interface that is used by the client

    class SalutationService_Proxy : public SalutationService {

    public:

      SalutationService_Proxy(::ProtoCall::Runtime::RpcChannel *channel);

      void sayHello(const Request* input, Response* output, google::protobuf::Closure* done);

    private:

      ProtoCall::Runtime::RpcChannel *m_channel;
    };

You can see that a SalutationService class has been created with pure virtual method for our single RPC call the service provides. The method has three parameters:

+ input - This is the incoming message provide by the client.
+ output - This is the outgoing message filled out by the service.
+ done - This is the closure that that the service implementation can use to indict that processing of the service is complete and a response can be sent back to the client.

This abstract class defines the interface to the remote service and should be subclasses to provide an implementation of service. An example implementation is shown below:

    ...

    class SalutationServiceImpl: public SalutationService {

      void sayHello(const Request* input, Response* output, google::protobuf::Closure* done);

    };

    ...

    void SalutationServiceImpl::sayHello(const Request* input, Response* output,

       google::protobuf::Closure* done)
    {

      std::ostringstream greeting;

      greeting << "Hello " << input->name() << endl;

      output->set_greeting(greeting.str());

      done->Run();

    }

There is also a subclass of SalutationService defined in SalutationService.pb.h, SalutationService_Proxy. The implementation of the class can be down in SalutationService.pb.cc, This provides the proxy implement of the service that can be used to call the remote service. The generated code takes can of all the marshaling and sending the data on the channel. A typedef is defined in SalutationService as syntax sugar so the proxy can be referred to as SalutationService::Proxy.

### Registering a service implementation

An implementation of a service needs to registered with the ServiceManager before it can be used. This is currently done used a generated dispatcher class, in our case SalutationService_Dispatcher is the generated dispatcher, again is can be reference as SalutationService::Dispatcher.

    ...

    ProtoCall::Runtime::ServiceManager *mgr = ProtoCall::Runtime::ServiceManager::instance();

    SalutationServiceImpl service;

    SalutationService::Dispatcher dispatcher(&service);

    mgr->registerService(&dispatcher);

The registration provide involve associating the methods with their implemented so incoming message can served by the correct service.

## ProtoCall::Runtime::RpcChannel

This class represents the communication between a client and service, wrapping the vtkSocketCommunicator. It has a single public method:

    receive(bool nonBlocking=true);

This method selects on the underling socket and processes any incoming messages. A blocking call can be made by provide the parameter false. This is the method that should be called as part of the event loop or main thread on the client and server applications.

Note RpcChannel is not thread safe, so you can't execute two calls to proxy simultaneously on different threads. 

## Basic server implementation

Here is an example of a simple server that will server our service:

    // Create a socket communicator to listen on the socket.
    vtkSocketCommunicator *com = vtkSocketCommunicator::New();
    // Supress warning ( for the moment )
    com->SetReportErrors(0);

    // Wait for a connection from a client
    while(true) {
      if(!com->WaitForConnection(8888)) {
        std::cerr << "Listen failed" << endl;
        return;
      }

      // Create a new channel around the communicator
      ProtoCall::Runtime::VtkCommunicatorChannel channel(com);

      // Register our service
      ProtoCall::Runtime::ServiceManager *mgr = ProtoCall::Runtime::ServiceManager::instance();
      SalutationServiceImpl service;
      SalutationService::Dispatcher dispatcher(&service);
      mgr->registerService(&dispatcher);

      // Receive incoming messages on the channel
      while(channel.receive());

      com->CloseConnection();
    }

## Calling the service using the proxy

Here is an example of a simple client that will connect to our server and call the service.

    // Call back function use to display the response
    void callback(Response *response)

    {

      std::cout << response->greeting() << endl;

    }

    ...

    // Create a communicator and connect to the server
    vtkSocketCommunicator *com = vtkSocketCommunicator::New();

      if(!com->ConnectTo("localhost", 8888)) {

        std::cerr << "Connection failed" << endl;

        return;

      }

    // Wrap the communicator in a channel
    ProtoCall::Runtime::VtkCommunicatorChannel channel(com);

    // Create the proxy used to call the server\
    salutationService::Proxy proxy(&channel);

    // Create our request and response
    Request request;

    request.set_name("Chris");

    Response response;

    // This will be called then reply is recieve
    Closure *callback = NewCallback(&::callback, &response);

    // Call the service
    proxy.sayHello(&request, &response, callback);

    // Block waiting for the response
    channel.receive(false);


The full code for this example can be found [here](https://github.com/OpenChemistry/protocall/tree/master/examples/salutation)

## Compiling generate code and service implementations

Once a service implement and code calling the service has been written the right combiniation of generate code needs to be compile.
Here is an example CMakeLists.txt that takes the variables set by the protocallc and build a client and server executable.

    # Needs for vtkSocketCommunicator
    find_package(VTK COMPONENTS vtkParallelCore)

    set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS ${VTK_DEFINITIONS})
    include_directories(${VTK_INCLUDE_DIRS})

    # Need to find protobuf and ProtoCall
    find_package(protobuf REQUIRED NO_MODULE)
    include_directories(${protobuf_INCLUDE_DIRS})

    find_package(ProtoCall REQUIRED NO_MODULE)
    include_directories(${ProtoCall_INCLUDE_DIRS})


    # Call to generate the proxy code and stubs
    protocallc(_common_source
      _service_source
      _proxy_source
      _include_dir
      "hello.proto")

    # Add the include directory containing the generated headers
    include_directories(${_include_dir})

    # Create the client executable. Note we need to include the generated
    # proxy code and common code.
    add_executable(client
      ${_proxy_source}
      ${_common_source}
      client/client.cpp)

    # Need to link against ProtoCall runtime
    target_link_libraries(client runtime)

    # Create the server executable. Note we need to include the generated
    # service code and common code.
    add_executable(server
      ${_common_source}
      ${_service_source}
      server/service.cpp
      server/server.cpp)

    # Again we need to link against the runtime library.
    target_link_libraries(server runtime)

## Using external types

Types that are not VTK types or modeled using protobuf are referred to as external types. In order to include these in ProtoCall message the appropriate serialization methods methods need to me provided for ProtoCall to use to serialize/deserialize these types. This is don't by providing am implementation of two class ProtoCall::Serialization::Serializer and ProtoCall::Serialization::Deserializer. The Serializer has the follow methods that need to be implemented:

    virtual bool serialize(void *data, size_t size) = 0;
    virtual size_t size() = 0;

The deserializer has the following method that need to be implemented:
  
     virtual bool deserialize(const void *data, size_t size) = 0;

The implementation of the classes must follow the following naming convention. <type>Serializer and <type>Deserializer, where type  is the type being serialized. The serliaiztion classes must also be in the same namespace as the type being serialized.  Do give the following type:

    namespace Foo {

    class Bar {

    ...

    }

    }


Then two classes need to be provided:

    namespace Foo {

    BarSerializer : public ProtoCall::Serialization::Serializer {

    ...

    }

    BarDeserializer : public ProtoCall::Serialization::Deserializer {

    ...

    } 

    }

Once these classes have been provided, wrapper protobuf types need to be used to hold the types when adding them to protobuf messages. These need to be defined in a package that starts with "external" and include the namespace of the class. So for example given our example type above. The following .proto file needs to be created:

    package external.Foo;

    message Bar {}

This file can then be imported and used to add these types to protobuf messages:

    import "foo.proto"

    message TestMesage {

    external.Foo.Bar bar = 1;

    }

## Getting and setting external types and VTK types on generate message classes

As external type or VTK types are not modelled using protobuf messages setting and getting on these values is a little different to standard protobuf types.

    message RequestVTK {
      optional vtk.vtkTable myTable = 1;
    }

The pointer to the vtkTable is set using the set(...) and accessed using the get() method provided by the type wrapper. Example code is given below:

    vtkTable *table = vtkTable::New();
    RequestVTK myRequest;

    // Setting the table field, the mutable_ prefix is standard protobuf
    // convention
    myRequest.mutable_mytable()->set(table);


    // Getting the table field
    myRequest.mytable()->get()

It is also possible to relinquish ownership of the object if you want the enclosing protobuf message to be responsable for the cleanup of the memory, this is done using the set_allocated(...) method.

    vtkTable *table = vtkTable::New();
    RequestVTK myRequest;

    // Setting the table field, the mutable_ prefix is standard protobuf
    // convention
    myRequest.mutable_mytable()->set_allocated(table);
