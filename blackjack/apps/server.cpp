
#include <blackjack/message.h>
#include <blackjack/table.h>
#include <blackjack/timer.h>

#include <zmq.hpp>


#include <iostream>
#include <memory>




zmq::message_t setupTableStateMessage(table_t& t) {
  std::size_t size = sizeof(table_t);
  for (player_t& p : t.players) {
    size += sizeof(player_t);
    for (hand_t& h : p.hands) {
      size += sizeof(hand_t);
      size += (sizeof(card_t) * h.cards.size());
    }
  }

  // Print the current state to console.
  printToConsole(t);

  zmq::message_t msg(size);
  memcpy(msg.data(), &t, size);

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
    int conflate = 1;
    publisher->setsockopt( ZMQ_CONFLATE, &conflate, sizeof( conflate ) );
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
  wait(timer, 120.0f); //< Wait 2 minutes

  while (1) {

    // Poll for the next message in the queue.
    message_t msg;

    zmq::message_t request;
    if (listener->recv(&request, ZMQ_NOBLOCK))
    {
      memcpy(&msg, request.data(), sizeof(message_t));

      if (msg.cmd == Message::JOIN) {
        if (table.state == TableState::WAITING_TO_START && table.players.size() < 8 ) { //validate
          player_t player;
          player.identifier = msg.player_id;

          //@todo - query some back end database or something to grab a better
          //        representation of the player, ie past winnings etc.
          addPlayer(table, player);

          std::cout << "Player Added" << std::endl;

          //@todo - failure response if there was no room for player etc.

          // Broadcast state change
          zmq::message_t msg = setupTableStateMessage(table);
          publisher->send(&msg, ZMQ_NOBLOCK);
        }
      }
      else if (msg.cmd == Message::BET && table.state == TableState::WAITING_ON_PLAYERS) {
        //
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

        // @todo - add a step that allows players to be seated at the
        //         table but not active this round.  Use allBetsIn to determine
        //         which players want to be active this round.

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
        wait(timer, 120.0f);
        table.state = TableState::WAITING_ON_PLAYERS;
        std::cout << "Changing Table State" << std::endl;

        // Broadcast state change
        zmq::message_t msg = setupTableStateMessage(table);
        std::cout << "Table State msg size: " << std::to_string(msg.size()) << std::endl;
        publisher->send(&msg, ZMQ_NOBLOCK);
      }
    }
    else if (table.state == TableState::WAITING_ON_PLAYERS) {
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
          wait(timer, 120.0f);  // or go again.
        }
      }
    }
  }

  return 0;
}
