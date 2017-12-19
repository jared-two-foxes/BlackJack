
#include <blackjack/message.h>
#include <blackjack/table.h>
#include <blackjack/timer.h>
#include <blackjack/serialize.h>

#include <zmq.hpp>


#include <iostream>
#include <memory>

#define BET_AMOUNT  10
#define WAIT_PERIOD 20.0


zmq::message_t setupTableStateMessage(table_t& t) {
  // Print the current state to console.
  printToConsole(t);

  zmq::message_t msg(calculateSize(t));
  serialize(t, (char*)msg.data());

  return msg;
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

  // Wait for begin timer to count down.
  table.state = TableState::WAITING_TO_START;
  //wait(timer, 120.0f); //< Wait 2 minutes
  wait(timer, WAIT_PERIOD); //< Wait 10 seconds

  while (1) {

    zmq::message_t request;
    if (listener->recv(&request, ZMQ_NOBLOCK))
    {
      message_t msg;
      memcpy(&msg, request.data(), sizeof(message_t));

      if (msg.cmd == Message::JOIN) {
        if (table.state == TableState::WAITING_TO_START && table.players.size() < 8 ) { //validate
          player_t player;
          player.identifier = msg.player_id;

          //@todo - failure response if there was no room for player etc.
          //@todo - query some back end database or something to grab a better
          //        representation of the player, ie past winnings etc.
          addPlayer(table, player);

          std::cout << "Player Added" << std::endl;
        }
      }
      else if (msg.cmd == Message::BET && table.state == TableState::WAITING_FOR_BETS) {
        player_t& p = getPlayer(table, msg.player_id);
        p.betValue = BET_AMOUNT;
      }
      else if (msg.cmd == Message::HIT && table.state == TableState::WAITING_FOR_ACTIONS) {
        //@todo Find the hand requested and set the action requested.
        hand_t& hand = getHand(table, msg.player_id, msg.hand_id);
        hand.action = Action::HIT;
      }
      else if (msg.cmd == Message::HOLD && table.state == TableState::WAITING_FOR_ACTIONS) {
        //@todo Find the hand requested and set the action requested.
        hand_t& hand = getHand(table, msg.player_id, msg.hand_id);
        hand.action = Action::HOLD;
      }
      else if (msg.cmd == Message::SPLIT && table.state == TableState::WAITING_FOR_ACTIONS) {
        //@todo Find the hand requested and set the action requested.
        hand_t& hand = getHand(table, msg.player_id, msg.hand_id);
        //@todo divide this hand into 2 hands and deal 2 more cards for each of
        // the hands.
      }

      zmq::message_t reply(20);
      snprintf((char*)reply.data(), 20, "OK");
      listener->send(reply);
    }

    if (table.state == TableState::WAITING_TO_START) {
      if (complete(timer)) {

        // Prepare the table!

        // Create a new deck for play.
        deck_t deck = new_deck();
        table.deck = shuffle(deck);

        // set the table into the next state.
        wait(timer, WAIT_PERIOD);
        table.state = TableState::WAITING_FOR_BETS;

        // Broadcast state change
        zmq::message_t msg = setupTableStateMessage(table);
        publisher->send(msg, ZMQ_NOBLOCK);
      }
    }
    else if (table.state == TableState::WAITING_FOR_BETS) {
      if (complete(timer) || allBetsIn(table)) {
        dealIn(table);

        // Setup the dealer's hand
        table.dealer.cards.clear();
        table.dealer.state = HandState::ACTIVE;
        table.dealer.action = Action::UNKNOWN;

        // deal each player at the table 2 cards.
        deal(table);
        deal(table);

        // Check for anyone that has gone blackjack.
        checkHands(table);

        // set the table into the next state.
        wait(timer, WAIT_PERIOD);
        table.state = TableState::WAITING_FOR_ACTIONS;

        // Broadcast state change
        zmq::message_t msg = setupTableStateMessage(table);
        publisher->send(msg, ZMQ_NOBLOCK);
      }
    }
    else if (table.state == TableState::WAITING_FOR_ACTIONS) {
      if (complete(timer) || allActionsIn(table)) {
        // Deal another card to everyone that wants everyone
        deal(table);
        checkHands(table);

        //@todo handle case whereby the dealer is no longer ACTIVE

        if (allOut(table)) {
          table.state = TableState::REWARD;
        }
        else {
          //@todo - reset the players state.
          wait(timer, WAIT_PERIOD);  // or go again.
        }
      }
    }
  }

  return 0;
}
