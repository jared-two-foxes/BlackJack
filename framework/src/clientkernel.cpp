
#include <framework/kernel/clientkernel.hpp>

#include <memory>
#include <iostream>


const std::string publisherEndPoint = "tcp://localhost:5556";
const std::string listenerEndPoint = "tcp://localhost:5555";


ClientKernel::~ClientKernel()
{}

void
ClientKernel::setup(int argc, char** argv) {
  context_          = std::make_unique<zmq::context_t >( 1 );
  subscriber_ = std::make_unique<zmq::socket_t >( *context_, ZMQ_SUB );
  server_     = std::make_unique<zmq::socket_t >( *context_, ZMQ_REQ );

  try {
    subscriber_->connect( publisherEndPoint );

    // Setting subscription to all events. Argument is not currently supported, server doesnt push an identifier yet.
    const char* filter = ""; //( argc > 1 ) ? argv[1] : "";
    subscriber_->setsockopt( ZMQ_SUBSCRIBE, filter, strlen( filter ) );

    // Set the subscriber socket to only keep the most recent message, dont care about any other messages.
    //int conflate = 1;
    //subscriberSocket->setsockopt( ZMQ_CONFLATE, &conflate, sizeof( conflate ) );

    server_->connect( listenerEndPoint );
  }
  catch ( zmq::error_t e ) {
    std::cout << e.what() << std::endl;
    //return -1;
  }
}

void
ClientKernel::processMessage(const zmq::message_t& msg) {
  // Do nothing.
}

void 
ClientKernel::updateFrame() {
  // Do nothing.
}

int
ClientKernel::run() {
  while (1) {
    _pollForServerMessage();
    updateFrame();
  }

  std::cout << "exiting." << std::endl;
}

void
ClientKernel::_pollForServerMessage() {
  // Check for an updated board state.
  zmq::message_t msg;
  if (subscriber_->recv(&msg, ZMQ_NOBLOCK)) {
    if (msg.size() >= 0) {
      processMessage(msg);
    }
  }
}