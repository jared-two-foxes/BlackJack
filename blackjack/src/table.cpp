
#include <blackjack/table.h>

table_t createTable() {
  table_t table;
  return table;
}

void addPlayer(table_t& table, player_t& player) {
  table.players.push_back(player);
}

void dealIn(table_t& ) {
  // setup a hand for each player
  for(player_t& p : table.players) {
    p.hands.push_back(hand_t());
    hand_t& h = p.hands.back();
    h.state = HandState::ACTIVE;
    h.action = Action::UNKNOWN;
  }
}

void deal(table_t& table) {
  //deal a card to each player
  for(player_t& p : table.players) {
    for(hand_t& h : p.hands) {
      card_t card = draw(table.deck);
      h.cards.push_back(card);
    }
  }

  //deal a card to the dealers hand.
  table.dealer.cards.push_back(draw(table.deck));
}
