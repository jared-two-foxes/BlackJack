
#include "ServerApplication.hpp"

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

const std::string publisherEndPoint = "tcp://*:5556";
const std::string listenerEndPoint = "tcp://*:5555";

int round_;

zmq::message_t setupTableStateMessage(table_t& t) {
  zmq::message_t msg(calculateSize(t));
  serialize(t, (char*)msg.data());
  return msg;
}

void processJoinMessage(table_t& table, int player_id, int hand_id) {
  player_t player;
  player.identifier = player_id;

  //@todo - failure response if there was no room for player etc.
  //@todo - query some back end database or something to grab a better
  //        representation of the player, ie past winnings etc.
  addPlayer(table, player);
  std::cout << "Player Added" << std::endl;
}

void processBetMessage(table_t& table, int player_id, int hand_id) {
  player_t* p = findPlayer(table, player_id);
  if (p != nullptr) {
    //check that the player has BET_AMOUNT available.
    //reduce players wallet by BET_AMOUNT;

    //assert that we're not attempting to add multiple hands
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
  _setupGameState(argc, argv);
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
  printToConsole(table_);
  sendMessageToClients(setupTableStateMessage(table_));
}

void Application::_setupGameState(int argc, char** argv) {

  using namespace std::placeholders;
  using namespace stateless;

//
// Setup the Game State.
//
  table_ = createTable();
  std::cout << "table created" << std::endl;


//
// Setup the callback functors
//
  callbacks_[MessageTypes::JOIN]  = &processJoinMessage;
  callbacks_[MessageTypes::HOLD]  = &processHitMessage;
  callbacks_[MessageTypes::BET]   = &processBetMessage;
  callbacks_[MessageTypes::HIT]   = &processHoldMessage;
  //callbacks_[MessageTypes::SPLIT] = &processSplitMessage;


//
// Setup the server fsm which is used to define the game and rules.
//
  //wait(timer_, WAIT_PERIOD);

  //@todo how to reset the timer for use in the next state.
  fsm_.on_transition( [&](state_machine<TableState, TableActions >::TTransition t) {
      std::cout << "onTransition (" << (int)t.source() << "," << (int)t.destination() << ")" << std::endl;
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
      setupTable(table_);
    });

  fsm_.configure(TableState::DEALING)
    .permit_if(TableActions::DEAL, TableState::REWARD, [&](){return isRoundOver(table_);})
    .permit_if(TableActions::DEAL, TableState::WAITING_FOR_ACTIONS, [&](){return !isRoundOver(table_);})
    .on_entry([&](state_machine<TableState, TableActions >::TTransition t){
      if (!areHandsPopulated(table_)) {
        deal(table_);
      }
      fsm_.fire(TableActions::DEAL);
    });

  fsm_.configure(TableState::WAITING_FOR_ACTIONS)
    .permit_reentry_if(TableActions::SET, [&](){return !allActionsIn(table_);}) 
    .permit_if(TableActions::SET, TableState::DEALING, [&](){return allActionsIn(table_);});

  fsm_.configure(TableState::REWARD)
    .on_entry([&](state_machine<TableState, TableActions >::TTransition t){
      std::cout << "Game Over!" << std::endl;
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
