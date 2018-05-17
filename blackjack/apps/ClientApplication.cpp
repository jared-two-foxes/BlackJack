
#include "ClientApplication.hpp"

#include <blackjack/message.h>
#include <blackjack/messagetypes.hpp>
#include <blackjack/serialize.h>

#include <chrono>

#include <iostream>


std::string GetLineFromCin() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

DWORD setupConsole() {
  // Set output mode to handle virtual terminal sequences
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE)
  {
      return GetLastError();
  }

  DWORD dwMode = 0;
  if (!GetConsoleMode(hOut, &dwMode))
  {
      return GetLastError();
  }

  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  if (!SetConsoleMode(hOut, dwMode))
  {
      return GetLastError();
  }

  return 0;
}


ClientApplication::ClientApplication()
{
  table_.state = TableState::WAITING_TO_START;
}

void
ClientApplication::setup(int argc, char** argv) {
  ClientKernel::setup(argc, argv);
  setupConsole();

  future_ = std::async(std::launch::async, GetLineFromCin);

  std::cout << "Blackjack Client!" << std::endl;
  _updateActionPrompt();
}

void
ClientApplication::processMessage(const zmq::message_t& msg) {
  deserialize((char*)msg.data(), &table_);
  _updateActionPrompt();
}

void
ClientApplication::updateFrame() {
  using namespace std::chrono_literals;

  if (future_.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
    auto line = future_.get();
    vt_.append(line + "\n");

    message_t msg;
    if (_setupRequest(line, msg)) {
      postMessageToServer(msg);
    }

    // Do I need to reset the future, probably.
    future_ = std::async(std::launch::async, GetLineFromCin);
  }
}

bool
ClientApplication::_setupRequest(const std::string& action, message_t& msg) {
  // // First check if we are after a local actions
  // if (action == "printTable") {
  //   printToConsole(table_);
  // }

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
  
  if (table_.state == TableState::DEALING || table_.state == TableState::REWARD) {
    vt_.flip("Dealing/Rewarding");
    return; // Dont display anything.
  }

  std::string s("Action ");

  if (table_.state == TableState::WAITING_TO_START) {
    s += "(join): ";
  }
  else if (table_.state == TableState::WAITING_FOR_BETS) {
    s += "(bet): ";
  }
  else if (table_.state == TableState::WAITING_FOR_ACTIONS) {
    s += "(bet, hit): ";
  }

  vt_.flip(s);
}
