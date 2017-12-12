
#include <blackjack/hand.h>

int count(hand_t& hand) {
  int v = 0;
  for (card_t& c : hand.cards) {
    v += c.value;
  }
  return v;
}
