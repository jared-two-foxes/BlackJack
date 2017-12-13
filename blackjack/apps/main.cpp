
#include <blackjack/message.h>
#include <blackjack/table.h>
#include <blackjack/timer.h>

#include <zmq.hpp>

#include <memory>


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

  publisher->bind( publisherEndPoint );
  listener->bind( listenerEndPoint );

  // Set the subscriber socket to only keep the most recent message, dont care about any other messages.
  int conflate = 1;
  publisher->setsockopt( ZMQ_CONFLATE, &conflate, sizeof( conflate ) );


//
// Setup the Game State.
//
  timer_t timer;
  table_t table = createTable();

  // Wait for begin timer to count down.
  table.state = TableState::WAITING_TO_START;
  wait(timer, 120.0f); //< Wait 2 minutes

  // player_t player_1, player_2;
  // addPlayer(table, player_1);
  // addPlayer(table, player_2);

  while (1) {

    //@todo - poll for the next message in the queue.
    message_t msg;

    if (msg.cmd == Message::JOIN) {
      if (table.state == TableState::WAITING_TO_START && table.players.size() < 8 ) { //validate
        player_t player;
        player.identifier = msg.player_id;

        //@todo - query some back end database or something to grab a better
        //        representation of the player, ie past winnings etc.
        addPlayer(table, player);

        //@todo - failure response if there was no room for player etc.
      }
    }
    else if (msg.cmd == Message::BET && table.state == TableState::WAITING_ON_PLAYERS) {
      //
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

  return -1;
}
