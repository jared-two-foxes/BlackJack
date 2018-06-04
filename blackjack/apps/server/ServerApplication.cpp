
#include "ServerApplication.hpp"

#include "shared/writer.hpp"

#include <blackjack/identifier.hpp>
#include <blackjack/message.hpp>
#include <blackjack/serialize.hpp>
#include <blackjack/rules.hpp>

#include <functional>
#include <iostream>
#include <memory>


DWORD setupConsoleOutput() {
  // Set output mode to handle virtual terminal sequences
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE)
  {
    return GetLastError();
  }

  DWORD dwOriginalMode = 0;
  if (!GetConsoleMode(hOut, &dwOriginalMode))
  {
    return GetLastError();
  }

  DWORD dwRequestedModes = ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
  DWORD dwMode = dwOriginalMode | dwRequestedModes;
  if (!SetConsoleMode(hOut, dwMode))
  {
    // we failed to set both modes, try to step down mode gracefully.
    dwRequestedModes = ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    dwMode = dwOriginalMode | dwRequestedModes;
    if (!SetConsoleMode(hOut, dwMode))
    {
        // Failed to set any VT mode, can't do anything here.
        return GetLastError();
    }
  }

  return 0;
}

zmq::message_t setupTableStateMessage(table_t& t) {
  zmq::message_t msg(calculateSize(t));
  serialize(t, (char*)msg.data());
  return msg;
}

void processJoinMessage(message_body_t data) {
  table_t table;
  player_t player;
  player.identifier = data.player_id;
  player.stash = 100; //< Create a default amount of chips.

  //@todo - failure response if there was no room for player etc.
  //@todo - query some back end database or something to grab a better
  //        representation of the player, ie past winnings etc.
  addPlayer(table, player);
}

void processBetMessage(message_body_t data) {
  table_t table;
  player_t* p = findPlayer(table, data.player_id);
  if (p != nullptr) {
    // TODO: handle when player doesnt have enough cash to place bet.
    
    // TODO: prevent the player from "betting" to get multiple hands.
    addHand(table, *p);
  }
}

void processHitMessage(message_body_t data) {
  table_t table;
  //@todo Find the hand requested and set the action requested.
  hand_t* hand = findHand(table, data.hand_id);
  if (hand != nullptr) {
    hand->action = HandActions::HIT;
  }
  else {
    std::cout << "Failed to process client message: " << std::endl;
    std::cout << "Unable to find hand." << std::endl;
  }
}

void processHoldMessage(message_body_t data) {
  table_t table;

  //@todo Find the hand requested and set the action requested.
  hand_t* hand = findHand(table, data.hand_id);
  if (hand != nullptr) {
    hand->action = HandActions::HOLD;
  }
  else {
    std::cout << "Failed to process client message: " << std::endl;
    std::cout << "Unable to find hand." << std::endl;
  }
}

void processSplitMessage(message_body_t data) {
  table_t table;

  //@todo Find the hand requested and set the action requested.
  hand_t* hand = findHand(table, data.hand_id);
  //@todo divide this hand into 2 hands and deal 2 more cards for each of the hands.
}


Application::Application()
{}

Application::~Application()
{}

void Application::setup(int argc, char** argv) {
  server_.setup(argc, argv);
  setupConsoleOutput();
  _setupKernel();
}

void Application::processMessage(const zmq::message_t& request) {
  message_t msg;
  memcpy(&msg, request.data(), sizeof(message_t));
  MessageTypes mt = (MessageTypes)msg.cmd;

  try {   
    kernel_.process(mt, msg.data);
  }
  catch (std::exception& e) {
    std::cout << "Exception thrown during 'fire':\n" << e.what() << std::endl;
  }
}

// Broadcast state change
void Application::pushTableState() {
  table_t table;
  framework::StackLayout<> layout{
    framework::Text{ "BlackJack Server"},
    renderTable(table)
  };
  vt_ = vt_.flip(layout.render(80).toString());

  server_.sendMessageToClients(setupTableStateMessage(table));
}

int Application::run() {
  return server_.run();
}

void Application::_setupKernel() {

  using namespace std::placeholders;

  kernel_.add(MessageTypes::JOIN, processJoinMessage);
  kernel_.add(MessageTypes::HOLD, processHoldMessage);
  kernel_.add(MessageTypes::BET,  processBetMessage);
  kernel_.add(MessageTypes::HIT,  processHitMessage);
  //kernel_.add(MessageTypes::SPLIT, processSplitMessage);
}