
#include <blackjack/message.h>
#include <blackjack/table.h>
#include <blackjack/timer.h>
#include <blackjack/serialize.h>

#include <stateless++/state_machine.hpp>


#include <zmq.hpp>


#include <iostream>
#include <memory>

#define BET_AMOUNT  10
#define WAIT_PERIOD 20.0

typedef stateless::state_machine<TableState, Triggers > TStateMachine;


zmq::message_t setupTableStateMessage(table_t& t) {
  zmq::message_t msg(calculateSize(t));
  serialize(t, (char*)msg.data());
  return msg;
}

// Broadcast state change
void pushTableState( zmq::socket_t* publisher, table_t& table, TStateMachine& sm ) {
  std::cout << "Transitioning!!" << std::endl;
  table.state = sm.state();
  printToConsole(table);
  zmq::message_t msg = setupTableStateMessage(table);
  publisher->send(msg, ZMQ_NOBLOCK);
}

void ProcessMessage(table_t& table, message_t& msg) {
  Triggers cmd = (Triggers)msg.cmd;
  if (cmd == Triggers::JOIN) {
    player_t player;
    player.identifier = msg.player_id;

    //@todo - failure response if there was no room for player etc.
    //@todo - query some back end database or something to grab a better
    //        representation of the player, ie past winnings etc.
    addPlayer(table, player);
    std::cout << "Player Added" << std::endl;
  }
  else if (cmd == Triggers::BET) {
    player_t& p = getPlayer(table, msg.player_id);
    p.betValue = BET_AMOUNT;
  }
  else if (cmd == Triggers::HIT) {
    //@todo Find the hand requested and set the action requested.
    hand_t& hand = getHand(table, msg.player_id, msg.hand_id);
    hand.action = Action::HIT;
  }
  else if (cmd == Triggers::HOLD) {
    //@todo Find the hand requested and set the action requested.
    hand_t& hand = getHand(table, msg.player_id, msg.hand_id);
    hand.action = Action::HOLD;
  }
  else if (cmd == Triggers::SPLIT) {
    //@todo Find the hand requested and set the action requested.
    hand_t& hand = getHand(table, msg.player_id, msg.hand_id);
    //@todo divide this hand into 2 hands and deal 2 more cards for each of
    // the hands.
  }
}

void setupTable(table_t& table) {
  // Create a new deck for play.
  deck_t deck = new_deck();
  table.deck = shuffle(deck);
}

void dealIn(table_t& table) {
  // Setup the dealer's hand
  table.dealer.cards.clear();
  table.dealer.state = HandState::ACTIVE;
  table.dealer.action = Action::UNKNOWN;

  // deal each player at the table 2 cards.
  deal(table);
  deal(table);

  // Check for anyone that has gone blackjack.
  checkHands(table); //< ??
}

int main(int argc, char* argv) {

  const std::string publisherEndPoint = "tcp://*:5556";
  const std::string listenerEndPoint = "tcp://*:5555";


//
// Setup the communication layer.
//
  std::unique_ptr<zmq::context_t > context;
  std::unique_ptr<zmq::socket_t >  publisher;
  std::unique_ptr<zmq::socket_t >  listener;

  context = std::make_unique<zmq::context_t >( 1 );
  publisher = std::make_unique<zmq::socket_t >( *context, ZMQ_PUB );
  listener = std::make_unique<zmq::socket_t >( *context, ZMQ_REP );

  try {
    publisher->bind( publisherEndPoint );
    listener->bind( listenerEndPoint );

    // Set the subscriber socket to only keep the most recent message, dont care about any other messages.
    //int conflate = 1;
    //publisher->setsockopt( ZMQ_CONFLATE, &conflate, sizeof( conflate ) );
  }
  catch ( zmq::error_t e ) {
    std::cout << e.what() << std::endl;
  }

  std::cout << "communication layer setup complete" << std::endl;

//
// Setup the Game State.
//
  timer_t timer;
  table_t table = createTable();
  std::cout << "table created" << std::endl;


//
// Setup the server fsm which is used to define the game and rules.
//
  using namespace stateless;
  using namespace std::placeholders;

  // state, trigger?
  TStateMachine fsm(TableState::WAITING_TO_START);

  fsm
    .on_transition(std::bind(pushTableState, publisher.get(), table, fsm));

  fsm.configure(TableState::WAITING_TO_START)
    .permit(Triggers::JOIN, TableState::WAITING_FOR_BETS);

  fsm.configure(TableState::WAITING_FOR_BETS)
    .permit_if(Triggers::BET, TableState::WAITING_FOR_ACTIONS, std::bind(allBetsIn, table))
    .on_entry(std::bind(setupTable, table));

  fsm.configure(TableState::WAITING_FOR_ACTIONS)
    .permit_reentry_if(Triggers::HIT, std::bind(allActionsIn, table))
    .permit_reentry_if(Triggers::HOLD, std::bind(allActionsIn, table))
    .on_entry_from(Triggers::BET, std::bind(dealIn, table)) //< first time deal everyones hands
    .on_entry(std::bind(deal, table)); //< other times just deal a single card where required.

  // Replace with timed trigger?
  wait(timer, WAIT_PERIOD);


//
// Main Loop.
//
  while (1) {

    zmq::message_t request;
    if (listener->recv(&request, ZMQ_NOBLOCK))
    {
      message_t msg;
      memcpy(&msg, request.data(), sizeof(message_t));

      //@todo - Process the command contained in the message.
      ProcessMessage(table, msg);

      // Check to see if we can update the gamestate based upon the passed message.
      fsm.fire((Triggers)msg.cmd);

      zmq::message_t reply(20);
      snprintf((char*)reply.data(), 20, "OK");
      listener->send(reply);
    }

    // Fire off any transitions based upon expiring timer?
    // if (complete(timer)) {
    //   std::string trigger;
    //   fsm.fire(trigger);
    // }
  }

  return 0;
}
