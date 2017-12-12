
#include <blackjack/table.h>

int main(int argc, char* argv) {

  table_t table;

  player_t player_1, player_2;
  addPlayer(table, player_1);
  addPlayer(table, player_2);

  deck_t deck = new_deck();
  table.deck = shuffle(deck);

  // deal 2 cards to everyone at the table.
  deal(table);
  deal(table);

  // Wait for all players to bet, dealer has to take a hit if his score is 17 or less.

  return -1;
}
