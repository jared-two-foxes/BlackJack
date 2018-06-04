
#include <framework/online/server.hpp>

#include <iostream>


const std::string publisherEndPoint = "tcp://*:5556";
const std::string listenerEndPoint = "tcp://*:5555";

using namespace framework;

Server::~Server()
{}

void 
Server::setup(int argc, char** argv) {
  context_   = std::make_unique<zmq::context_t >( 1 );
  publisher_ = std::make_unique<zmq::socket_t >( *context_, ZMQ_PUB );
  listener_  = std::make_unique<zmq::socket_t >( *context_, ZMQ_REP );

  try {
    publisher_->bind( publisherEndPoint );
    listener_->bind( listenerEndPoint );
  }
  catch ( zmq::error_t e ) {
    std::cout << e.what() << std::endl;
  }

  //std::cout << "communication layer setup complete" << std::endl;
}

void 
Server::processMessage(const zmq::message_t& msg) {
  // do nothing.
}

void 
Server::updateFrame() {
  // do nothing.
}

int 
Server::run() {
  while (1) {
    _pollForClientMessage();
    updateFrame();
  }

  return 0;
}

void 
Server::sendMessageToClients(zmq::message_t msg) {
  publisher_->send(msg, ZMQ_NOBLOCK);
}

void 
Server::_pollForClientMessage() {
  zmq::message_t request;
  if (listener_->recv(&request, ZMQ_NOBLOCK)) {
    processMessage(request);

    //@todo - Update so that reply message can be determined from result of postMessage call above.
    zmq::message_t reply(20);
    snprintf((char*)reply.data(), 20, "OK");
    listener_->send(reply);
  }
}
