
#include <blackjack/table.h>

void addPlayer(table_t& table, player_t& player) {
  table.players.push_back(player);
}

void deal(table_t& table) {
  //deal 2 cards to each player
  for(player_t& p : table.players) {
    for(hand_t& h : p.hands) {
      card_t card = draw(table.deck);
      h.cards.push_back(card);
    }
  }

  //deal a card to the dealers hand.
  table.dealer.cards.push_back(draw(table.deck));
}
