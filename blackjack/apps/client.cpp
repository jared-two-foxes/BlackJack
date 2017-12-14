
#include <blackjack/message.h>
#include <blackjack/table.h>
#include <blackjack/timer.h>

#include <zmq.hpp>

#include <chrono>
#include <future>
#include <iostream>
#include <memory>


std::string GetLineFromCin() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

int main(int argc, char* argv) {

  const std::string publisherEndPoint = "tcp://localhost:5556";
  const std::string listenerEndPoint = "tcp://localhost:5555";

//
// Setup the communication layer.
//
  std::unique_ptr<zmq::context_t > context;
  std::unique_ptr<zmq::socket_t >  subscriberSocket;
  std::unique_ptr<zmq::socket_t >  serverSocket;

  context = std::make_unique<zmq::context_t >( 1 );
  subscriberSocket = std::make_unique<zmq::socket_t >( *context, ZMQ_SUB );
  serverSocket = std::make_unique<zmq::socket_t >( *context, ZMQ_REQ );

  try {
    subscriberSocket->connect( publisherEndPoint );
    serverSocket->connect( listenerEndPoint );

    // Setting subscription to all events. Argument is not currently supported, server doesnt push an identifier yet.
    const char* filter = ""; //( argc > 1 ) ? argv[1] : "";
    subscriberSocket->setsockopt( ZMQ_SUBSCRIBE, filter, strlen( filter ) );

    // Set the subscriber socket to only keep the most recent message, dont care about any other messages.
    int conflate = 1;
    subscriberSocket->setsockopt( ZMQ_CONFLATE, &conflate, sizeof( conflate ) );
  }
  catch ( zmq::error_t e ) {
    std::cout << e.what() << std::endl;
  }

  table_t table;

  auto future = std::async(std::launch::async, GetLineFromCin);

  std::cout << "Blackjack Client!" << std::endl;
  std::cout << "Action: ";

  while (1) {

    // Check for an updated board state.
    zmq::message_t tableStateMsg;
    if (subscriberSocket->recv(&tableStateMsg, ZMQ_NOBLOCK)) {
      std::cout << "Table State Message size: " << std::to_string(tableStateMsg.size()) << std::endl;
      if (tableStateMsg.size() > 0) {
        memcpy(&table, tableStateMsg.data(), tableStateMsg.size());

        std::cout << "Table State updated!" << std::endl;
        printToConsole(table);

        std::cout << "Action: ";
      }
    }

    //@todo - filter available actions to display based upon the board state?

    if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
      auto line = future.get();

      // Set a new line. Subtle race condition between the previous line
      // and this. Some lines could be missed. To aleviate, you need an
      // io-only thread.
      future = std::async(std::launch::async, GetLineFromCin);

      message_t msg;
      msg.player_id = 0; //< handle this somehow?

      bool OkToSend = false;
      if (line == "1" ) {
        msg.cmd = Message::JOIN;
        OkToSend = true;
      }

      if (OkToSend) {
        // Now send the message to the server.
        zmq::message_t request( sizeof(message_t) );
        memcpy( (char*)request.data(), &msg, sizeof(message_t) );
        serverSocket->send( request );

        // Then wait for reply?  This doesnt feel super necessary but oh well.
        zmq::message_t reply;
        serverSocket->recv( &reply ); //< blocking
      }

      std::cout << "Action: ";
    }
  }

  return 0;
}
