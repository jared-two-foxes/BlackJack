#ifndef FRAMEWORK_SERVERKERNEL_H__
#define FRAMEWORK_SERVERKERNEL_H__

#include <zmq.hpp>
#include <memory>

class ServerKernel
{
private:
  std::unique_ptr<zmq::context_t > context_;
  std::unique_ptr<zmq::socket_t >  publisher_;
  std::unique_ptr<zmq::socket_t >  listener_;

  public:
    virtual ~ServerKernel();

    virtual void setup(int argc, char** argv);
    virtual void processMessage(const zmq::message_t& msg);
    virtual void updateFrame();
    virtual int  run();

    template <typename T> 
    void sendMessageToClients(T& data);
    void sendMessageToClients(zmq::message_t msg);

  private:
  	void _pollForClientMessage();

};


template <typename T> void 
ServerKernel::sendMessageToClients(T& data) {
  // Now send the message to the server.
  zmq::message_t request( sizeof(T) );
  memcpy( (char*)request.data(), &data, sizeof(T) );
  publisher_->send( request );
}

#endif // FRAMEWORK_SERVERKERNEL_H__