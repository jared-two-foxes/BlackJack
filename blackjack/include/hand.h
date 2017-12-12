#ifndef BLACKJACK_HAND_H__
#define BLACKJACK_HAND_H__

#include <blackjack/card.h>
#include <vector>

enum HandState {
  ACTIVE,
  DEAD,
  WON
};

enum Action {
  UNKNOWN,
  HIT,
  HOLD
};

struct hand_t
{
  HandState state;
  Action action;
  std::vector<card_t > cards;
  int bet; //< amount that the player has bet on this hand at this stage.
};

int count(hand_t& );

#endif // BLACKJACK_HAND_H__
