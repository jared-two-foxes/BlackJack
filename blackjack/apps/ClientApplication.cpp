
#include "ClientApplication.hpp"

#include <blackjack/message.h>
#include <blackjack/messagetypes.hpp>
#include <blackjack/serialize.h>

#include <chrono>
#include <future>
#include <iostream>


std::string GetLineFromCin() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}


ClientApplication::ClientApplication()
{
  table_.state = TableState::WAITING_TO_START;
}

int
ClientApplication::setup(argc, char** argv) {
  ClientKernel::setup(argc, argv);

  future_ = std::async(std::launch::async, GetLineFromCin);

  std::cout << "Blackjack Client!" << std::endl;
  _updateActionPrompt();
}

int
ClientApplication::Run() {
  
}

void
ClientApplication::processMessage(const zmq::message_t& msg) {
  deserialize((char*)msg.data(), &table_);

#if DEBUG_
  printToConsole(table_);
#endif

  std::cout << std::endl;
  _updateActionPrompt();
}

void
ClientApplication::updateFrame() {
  using namespace std::chrono_literals;

  if (future_.wait_for(std::chrono::seconds(0)) == std::future__status::ready) {
    auto line = future_.get();

    message_t msg;
    if (_setupRequest(line, msg)) {
      postMessageToServer(msg);
      _updateActionPrompt();
    }
  }
}

bool
ClientApplication::_setupRequest(const std::string& action, message_t& msg) {
  // First check if we are after a local actions
  if (action == "printTable") {
    printToConsole(table_);
  }

  if (table_.players.empty()) {
    msg.player_id = 0;
  }
  else {
    msg.player_id = table_.players[0].identifier; //< handle this somehow?
  }

  if (table_.players.empty() || table_.players[0].hands.empty()) {
    msg.hand_id = -1;
  }
  else {
    msg.hand_id = table_.hands[0].identifier; //< Also handle this!
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
ClientApplication::_updateActionPrompt() {
  std::cout << "Action ";

  if (table_.state == TableState::WAITING_TO_START) {
    std::cout << "(join): ";
  }
  else if (table_.state == TableState::WAITING_FOR_BETS) {
    std::cout << "(bet): ";
  }
  else if (table_.state == TableState::WAITING_FOR_ACTIONS) {
    std::cout << "(bet, hit): ";
  }
}
