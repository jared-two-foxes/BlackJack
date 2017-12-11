
#include <blackjack/table.h>

int main(int argc, char* argv) {

  table_t table;

  player_t player_1, player_2;
  addPlayer(table, player_1);
  addPlayer(table, player_2);

  deck_t deck = new_deck();
  table.deck = shuffle(deck);

  deal(table);

  return -1;
}
