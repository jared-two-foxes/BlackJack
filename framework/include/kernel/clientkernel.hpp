#ifndef FRAMEWORK_CLIENTKERNEL_H__
#define FRAMEWORK_CLIENTKERNEL_H__

#include <zmq.hpp>
#include <memory>

class ClientKernel
{
private:
  std::unique_ptr<zmq::context_t > context_;
  std::unique_ptr<zmq::socket_t >  subscriber_;
  std::unique_ptr<zmq::socket_t >  server_;

public:
  virtual ~ClientKernel();

  virtual void setup(int argc, char** argv);
  virtual void processMessage(const zmq::message_t& msg);
  virtual void updateFrame();
  virtual int  run();


  template <class T>
  void postMessageToServer(T& obj);
  
private:
  void _pollForServerMessage();

};

template <typename T> void 
ClientKernel::postMessageToServer(T& data) {
  // Now send the message to the server.
  zmq::message_t request( sizeof(T) );
  memcpy( (char*)request.data(), &data, sizeof(T) );
  server_->send( request );

  // Then wait for reply?  This doesnt feel super necessary but oh well.
  zmq::message_t reply;
  server_->recv( &reply ); //< blocking
}

#endif // FRAMEWORK_CLIENTKERNEL_H__