
#include "ServerApplication.hpp"

#include <blackjack/identifier.h>
#include <blackjack/message.h>
#include <blackjack/serialize.h>

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

// Check that all the players have defined if they have
// made a bet or not.
bool allBetsIn(table_t& t) {
  bool betsIn = true;
  for (player_t& p : t.players) {
    if (p.hands.size() == 0) {
      betsIn = false;
      break;
    }
  }
  return betsIn;
}

// Check that all the players have defined an action
// for each hand which they are responsible for.
bool allActionsIn(table_t& t) {
  bool actionsIn = true;
  for (hand_t& h : t.hands) {
    if (h.state == HandState::ACTIVE && h.action != HandActions::UNKNOWN) {
      actionsIn = false;
      break;
    }
  }
  return actionsIn;
}

// Check the hand to see if it's still active or not,
// hands over 21 are bust, hands on 21 have won,
// and any other hands are still active unless holding
void checkHand(hand_t& h) {
  int cnt = count(h);
  if (cnt == 21) {
    h.state = HandState::WON;
  }
  else if (cnt > 21) {
    h.state = HandState::BUST;
  }
}

void deal(table_t& t) {
  std::cout << "deal" << std::endl;
  round_++;

  //deal a card to each player
  for(hand_t& h : t.hands) {
    if (h.state == HandState::ACTIVE) {
      card_t card = draw(t.deck);
      h.cards.push_back(card);
      h.action = HandActions::UNKNOWN; //< reset the action.
      checkHand(h);
    }
  }

  //deal a card to the dealers hand.
  t.dealer.cards.push_back(draw(t.deck));
  checkHand(t.dealer);
}

void setupTable(table_t& table) {

  round_ = 0;

  table.deck = shuffle(new_deck());

  // Setup the dealer's hand
  table.dealer.cards.clear();
  table.dealer.state  = HandState::ACTIVE;
  table.dealer.action = HandActions::UNKNOWN;
}

// Check against the dealer.
bool areHandsPopulated(table_t& t) {
  return (t.dealer.cards.size() < round_);
}


Application::Application() :
  fsm_(TableState::WAITING_TO_START)
{}

Application::~Application()
{}

void Application::setup(int argc, char** argv) {
  setupCommunicationLayer();
  setupGameState();
}

// Broadcast state change
void Application::pushTableState() {
  printToConsole(table_);
  zmq::message_t msg = setupTableStateMessage(table_);
  publisher_->send(msg, ZMQ_NOBLOCK);
}


//cmd == MessageTypes::JOIN
void Application::processJoinMessage(int player_id, int hand_id) {
  player_t player;
  player.identifier = player_id;

  //@todo - failure response if there was no room for player etc.
  //@todo - query some back end database or something to grab a better
  //        representation of the player, ie past winnings etc.
  addPlayer(table_, player);
  std::cout << "Player Added" << std::endl;
}

//else if (cmd == MessageTypes::BET) {
void Application::processBetMessage(int player_id, int hand_id) {
  player_t* p = findPlayer(table_, player_id);
  if (p != nullptr) {
    //check that the player has BET_AMOUNT available.
    //reduce players wallet by BET_AMOUNT;

    //assert that we're not attempting to add multiple hands
    addHand(table_, *p);
  }
}

//else if (cmd == MessageTypes::HIT) {
void Application::processHitMessage(int player_id, int hand_id) {
  //@todo Find the hand requested and set the action requested.
  hand_t* hand = findHand(table_, hand_id);
  if (hand != nullptr) {
    hand->action = HandActions::HIT;
  }
  else {
    std::cout << "Failed to process client message: " << std::endl;
    std::cout << "Unable to find hand." << std::endl;
  }
}

//else if (cmd == MessageTypes::HOLD) {
void Application::processHoldMessage(int player_id, int hand_id) {
  //@todo Find the hand requested and set the action requested.
  hand_t* hand = findHand(table_, hand_id);
  if (hand != nullptr) {
    hand->action = HandActions::HOLD;
  }
  else {
    std::cout << "Failed to process client message: " << std::endl;
    std::cout << "Unable to find hand." << std::endl;
  }
}

//else if (cmd == MessageTypes::SPLIT) {
void Application::processSplitMessage(int player_id, int hand_id) {
  //@todo Find the hand requested and set the action requested.
  hand_t* hand = findHand(table_, hand_id);
  //@todo divide this hand into 2 hands and deal 2 more cards for each of
  // the hands.
}

void Application::setupCommunicationLayer() {
  context_ = std::make_unique<zmq::context_t >( 1 );
  publisher_ = std::make_unique<zmq::socket_t >( *context_, ZMQ_PUB );
  listener_ = std::make_unique<zmq::socket_t >( *context_, ZMQ_REP );

  try {
    publisher_->bind( publisherEndPoint );
    listener_->bind( listenerEndPoint );
  }
  catch ( zmq::error_t e ) {
    std::cout << e.what() << std::endl;
  }

  //std::cout << "communication layer setup complete" << std::endl;
}

void Application::setupGameState() {

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
  callbacks_[MessageTypes::JOIN]  = std::bind(&Application::processJoinMessage, this, _1, _2);
  callbacks_[MessageTypes::HOLD]  = std::bind(&Application::processHitMessage, this, _1, _2);
  callbacks_[MessageTypes::BET]   = std::bind(&Application::processBetMessage, this, _1, _2);
  callbacks_[MessageTypes::HIT]   = std::bind(&Application::processHoldMessage, this, _1, _2);
  //callbacks_[MessageTypes::SPLIT] = std::bind(&Application::processSplitMessage, this, _1, _2);


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

int Application::run() {
  while (1) {

    //@todo how to reset the timer for use in the next state.

    zmq::message_t request;
    if (listener_->recv(&request, ZMQ_NOBLOCK))
    {
      message_t msg;
      memcpy(&msg, request.data(), sizeof(message_t));

      // Process the command contained in the message.
      MessageTypes mt = (MessageTypes)msg.cmd;
      callbacks_[mt](msg.player_id, msg.hand_id);

      try {
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

        // Check to see if we can update the gamestate based upon the passed message.
        fsm_.fire(a);
      }
      catch (std::exception& e) {
        std::cout << "Exception thrown during 'fire':\n" << e.what() << std::endl;
      }

      zmq::message_t reply(20);
      snprintf((char*)reply.data(), 20, "OK");
      listener_->send(reply);
    }

    // Fire off any transitions based upon expiring timer?
    // if (complete(timer_)) {
    //   try {
    //     fsm_.fire(MessageTypes::TIME);
    //   }
    //   catch (std::exception& e) {
    //     std::cout << "Exception thrown during 'fire':\n" << e.what() << std::endl;
    //   }
    // }
  }

  return 0;
}
