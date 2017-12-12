
#include <blackjack/table.h>

int main(int argc, char* argv) {

  timer_t timer;
  table_t table = createTable();

  // Wait for begin timer to count down.
  table.state = WAITING_TO_START;
  wait(timer, 120.0f); //< Wait 2 minutes

  // player_t player_1, player_2;
  // addPlayer(table, player_1);
  // addPlayer(table, player_2);

  while (1) {

    //@todo - poll for the next message in the queue.
    message_t msg;

    if (msg.cmd == JOIN) {
      if (table.state == TableState::WAITING_TO_START && table.players.size() < 8 ) { //validate
        player_t player;
        player.identifier = msg.player_id;

        //@todo - query some back end database or something to grab a better
        //        representation of the player, ie past winnings etc.
        addPlayer(table, player);

        //@todo - failure response if there was no room for player etc.
      }
    }
    else if (msg.cmd == MEssage::BET && TableState::WAITING_ON_PLAYERS) {
      //
    }

    if (table.state == TableState::WAITING_TO_START) {
      if (complete(timer)) {
        // Create a new deck for play.
        deck_t deck = new_deck();
        table.deck = shuffle(deck);

        // @todo - add a step that allows players to be seated at the
        //         table but not active this round.

        dealIn(table);

        // Setup the dealer's hand
        table.dealer.cards.clear();
        table.dealer.state = HandState::ACTIVE;
        table.dealer.action = Action::UNKNOWN;

        // deal each player at the table 2 cards.
        deal(table)
        deal(table);

        // Check for anyone that has gone blackjack.
        checkHands(table);

        // set the table into the next state.
        wait(timer, 120.0f);
        table.state = TableState::WAITING_ON_PLAYERS;
      }
    }
    else if (table.state == TableState::WAITING_ON_PLAYERS) {
      if (complete(timer) || allBetsIn(table)) {
        // Deal another card to everyone that wants everyone
        deal(table);
        checkHands(table);

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
