
#include "ServerApplication.hpp"

#include "shared/writer.hpp"

#include <blackjack/identifier.h>
#include <blackjack/message.h>
#include <blackjack/messagetypes.hpp>
#include <blackjack/serialize.h>
#include <blackjack/rules.hpp>

#include <functional>
#include <iostream>
#include <memory>


#define BET_AMOUNT  10
#define WAIT_PERIOD 10.0

int round_ = 0;

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

void processJoinMessage(table_t& table, int player_id, int hand_id) {
  player_t player;
  player.identifier = player_id;
  player.stash = 100; //< Create a default amount of chips.

  //@todo - failure response if there was no room for player etc.
  //@todo - query some back end database or something to grab a better
  //        representation of the player, ie past winnings etc.
  addPlayer(table, player);
}

void processBetMessage(table_t& table, int player_id, int hand_id) {
  player_t* p = findPlayer(table, player_id);
  if (p != nullptr) {
    // TODO: handle when player doesnt have enough cash to place bet.
    
    // TODO: prevent the player from "betting" to get multiple hands.
    addHand(table, *p);
  }
}

void processHitMessage(table_t& table, int player_id, int hand_id) {
  //@todo Find the hand requested and set the action requested.
  hand_t* hand = findHand(table, hand_id);
  if (hand != nullptr) {
    hand->action = HandActions::HIT;
  }
  else {
    std::cout << "Failed to process client message: " << std::endl;
    std::cout << "Unable to find hand." << std::endl;
  }
}

void processHoldMessage(table_t& table, int player_id, int hand_id) {
  //@todo Find the hand requested and set the action requested.
  hand_t* hand = findHand(table, hand_id);
  if (hand != nullptr) {
    hand->action = HandActions::HOLD;
  }
  else {
    std::cout << "Failed to process client message: " << std::endl;
    std::cout << "Unable to find hand." << std::endl;
  }
}

void processSplitMessage(table_t& table, int player_id, int hand_id) {
  //@todo Find the hand requested and set the action requested.
  hand_t* hand = findHand(table, hand_id);
  //@todo divide this hand into 2 hands and deal 2 more cards for each of the hands.
}


Application::Application() :
  fsm_(TableState::WAITING_TO_START)
{}

Application::~Application()
{}

void Application::setup(int argc, char** argv) {
  ServerKernel::setup(argc, argv);
  setupConsoleOutput();
  _setupGameState(argc, argv);
  pushTableState();
}

void Application::processMessage(const zmq::message_t& request) {
  message_t msg;
  memcpy(&msg, request.data(), sizeof(message_t));
  MessageTypes mt = (MessageTypes)msg.cmd;

  try {   
    callbacks_[mt](table_, msg.player_id, msg.hand_id);
    fsm_.fire(_translateMessage((MessageTypes)msg.cmd));
  }
  catch (std::exception& e) {
    std::cout << "Exception thrown during 'fire':\n" << e.what() << std::endl;
  }
}


// Broadcast state change
void Application::pushTableState() {
  framework::StackLayout<> layout{
    framework::Text{ "BlackJack Server"},
    renderTable(table_)
  };
  vt_ = vt_.flip(layout.render(80).toString());

  sendMessageToClients(setupTableStateMessage(table_));
}

void Application::_setupGameState(int argc, char** argv) {

  using namespace std::placeholders;
  using namespace stateless;

//
// Setup the Game State.
//
  table_ = createTable();
  table_.betSize = BET_AMOUNT; //< TODO: make bet size dynamically determined.


//
// Setup the callback functors
//
  callbacks_[MessageTypes::JOIN]  = &processJoinMessage;
  callbacks_[MessageTypes::HOLD]  = &processHoldMessage;
  callbacks_[MessageTypes::BET]   = &processBetMessage;
  callbacks_[MessageTypes::HIT]   = &processHitMessage;
  //callbacks_[MessageTypes::SPLIT] = &processSplitMessage;


//
// Setup the server fsm which is used to define the game and rules.
//
  //wait(timer_, WAIT_PERIOD);

  //@todo how to reset the timer for use in the next state.
  fsm_.on_transition( [&](state_machine<TableState, TableActions >::TTransition t) {
      //std::cout << "onTransition (" << (int)t.source() << "," << (int)t.destination() << ")" << std::endl;
      table_.state = fsm_.state();
      pushTableState();
      //wait(timer_, WAIT_PERIOD);
    });

  fsm_.on_unhandled_trigger([](const TableState& s, const TableActions& t) {
      std::cerr << "ignoring unhandled trigger." << std::endl;
    });

  fsm_.configure(TableState::WAITING_TO_START)
    .permit(TableActions::JOIN, TableState::WAITING_FOR_BETS);

  fsm_.configure(TableState::WAITING_FOR_BETS)
    .permit_if(TableActions::BET, TableState::DEALING, [&](){return allBetsIn(table_);})
    .on_entry([&](state_machine<TableState, TableActions >::TTransition t){
      round_ = 0;
      setupTable(table_);
    });

  fsm_.configure(TableState::DEALING)
    .permit_if(TableActions::DEAL, TableState::REWARD, [&](){return isRoundOver(table_);})
    .permit_if(TableActions::DEAL, TableState::WAITING_FOR_ACTIONS, [&](){return !isRoundOver(table_);})
    .on_entry([&](state_machine<TableState, TableActions >::TTransition t){
      while (!areHandsPopulated(table_, round_)) {
        deal(table_);
      }
      round_++;
      fsm_.fire(TableActions::DEAL);
    });

  fsm_.configure(TableState::WAITING_FOR_ACTIONS)
    .permit_reentry_if(TableActions::SET, [&](){return !allActionsIn(table_);}) 
    .permit_if(TableActions::SET, TableState::DEALING, [&](){return allActionsIn(table_);});

  fsm_.configure(TableState::REWARD)
    .permit(TableActions::JOIN, TableState::WAITING_FOR_BETS)
    .on_entry([&](state_machine<TableState, TableActions >::TTransition t){
      rewardPlayers(table_);
      fsm_.fire(TableActions::JOIN);
    });
}

TableActions Application::_translateMessage(MessageTypes mt) {
  TableActions a;
  if (mt == MessageTypes::JOIN) {
    a = TableActions::JOIN;
  }
  else if (mt == MessageTypes::BET) {
    a = TableActions::BET;
  }
  else if (mt == MessageTypes::HIT || mt == MessageTypes::HOLD) {
    a = TableActions::SET;
  }

  return a;
}
