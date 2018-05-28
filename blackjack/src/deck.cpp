
#include <blackjack/deck.h>
#include <iostream>

deck_t new_deck() {
  std::cout << "new deck" << std::endl;

  deck_t deck;

  // fill the deck with all the cards.
  for(int s = 0; s < 4; s++) {
    for(int v = 0; v < 14; ++v) {
      card_t c;
      c.suit = s;
      c.value = v;
      deck.cards.push_back(c);
    }
  }

  return deck;
}

deck_t shuffle(deck_t& d) {
  return d;
}

card_t draw(deck_t& d) {
  std::cout << "draw" << std::endl;
  card_t& c = d.cards.back();
  d.cards.pop_back();
  return c;
}
