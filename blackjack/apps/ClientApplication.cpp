
#include "ClientApplication.hpp"
#include "messagetypes.hpp"

#include <blackjack/message.h>
#include <blackjack/serialize.h>

#include <chrono>
#include <future>
#include <iostream>


const std::string publisherEndPoint = "tcp://localhost:5556";
const std::string listenerEndPoint = "tcp://localhost:5555";


std::string GetLineFromCin() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}



ClientApplication::ClientApplication()
{
  m_table.state = TableState::WAITING_TO_START;
}

int
ClientApplication::Setup() {
  return SetupCommunicationLayer();
}

int
ClientApplication::Run() {
  using namespace std::chrono_literals;

  auto future = std::async(std::launch::async, GetLineFromCin);

  std::cout << "Blackjack Client!" << std::endl;
  UpdateActionPrompt();

  while (1) {
    PollForServerUpdate();

    if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
      auto line = future.get();

      // Set a new line. Subtle race condition between the previous line
      // and this. Some lines could be missed. To aleviate, you need an
      // io-only thread.
      future = std::async(std::launch::async, GetLineFromCin);


      message_t msg;

      if (m_table.players.empty()) {
        msg.player_id = 0;
      }
      else {
        msg.player_id = m_table.players[0].identifier; //< handle this somehow?
      }

      if (m_table.players.empty() || m_table.players[0].hands.empty()) {
        msg.hand_id = -1;
      }
      else {
        msg.hand_id = m_table.hands[0].identifier; //< Also handle this!
      }

      bool OkToSend = ProcessActionRequest( line, msg );
      if (OkToSend) {
        // Now send the message to the server.
        zmq::message_t request( sizeof(message_t) );
        memcpy( (char*)request.data(), &msg, sizeof(message_t) );
        m_serverSocket->send( request );

        // Then wait for reply?  This doesnt feel super necessary but oh well.
        zmq::message_t reply;
        m_serverSocket->recv( &reply ); //< blocking

        UpdateActionPrompt();
      }
    }
  }

  std::cout << "out" << std::endl;
}

int
ClientApplication::SetupCommunicationLayer() {
    m_context          = std::make_unique<zmq::context_t >( 1 );
    m_subscriberSocket = std::make_unique<zmq::socket_t >( *m_context, ZMQ_SUB );
    m_serverSocket     = std::make_unique<zmq::socket_t >( *m_context, ZMQ_REQ );

    try {
      m_subscriberSocket->connect( publisherEndPoint );

      // Setting subscription to all events. Argument is not currently supported, server doesnt push an identifier yet.
      const char* filter = ""; //( argc > 1 ) ? argv[1] : "";
      m_subscriberSocket->setsockopt( ZMQ_SUBSCRIBE, filter, strlen( filter ) );

      // Set the subscriber socket to only keep the most recent message, dont care about any other messages.
      //int conflate = 1;
      //subscriberSocket->setsockopt( ZMQ_CONFLATE, &conflate, sizeof( conflate ) );

      m_serverSocket->connect( listenerEndPoint );
    }
    catch ( zmq::error_t e ) {
      std::cout << e.what() << std::endl;
      return -1;
    }

    return 0;
}

void
ClientApplication::PollForServerUpdate() {
  // Check for an updated board state.
  zmq::message_t tableStateMsg;
  if (m_subscriberSocket->recv(&tableStateMsg, ZMQ_NOBLOCK)) {
    if (tableStateMsg.size() >= 0) {
      deserialize((char*)tableStateMsg.data(), &m_table);

#if DEBUG_
      printToConsole(m_table);
#endif

      std::cout << std::endl;
      UpdateActionPrompt();
    }
  }
}

bool
ClientApplication::ProcessActionRequest(const std::string& action, message_t& msg) {
  // First check if we are after a local actions
  if (action == "printTable") {
    printToConsole(m_table);
  }

  //@todo validate requested action?
  bool OkToSend = false;
  if (action == "join") {
    msg.cmd = (int)MessageTypes::JOIN;
    OkToSend = true;
  }
  else if (action == "bet") {
    msg.cmd = (int)MessageTypes::BET;
    OkToSend = true;
  }
  else if (action == "hold") {
    msg.cmd = (int)MessageTypes::HOLD;
    OkToSend = true;
  }
  else if (action == "hit") {
    msg.cmd = (int)MessageTypes::HIT;
    OkToSend = true;
  }
  return OkToSend;
}

void
ClientApplication::UpdateActionPrompt() {
  std::cout << "Action ";

  if (m_table.state == TableState::WAITING_TO_START) {
    std::cout << "(join): ";
  }
  else if (m_table.state == TableState::WAITING_FOR_BETS) {
    std::cout << "(bet): ";
  }
  else if (m_table.state == TableState::WAITING_FOR_ACTIONS) {
    std::cout << "(bet, hit): ";
  }
}
