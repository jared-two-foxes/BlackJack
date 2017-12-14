#ifndef BLACKJACK_HAND_H__
#define BLACKJACK_HAND_H__

#include <blackjack/card.h>

#include <string>
#include <vector>

enum class HandState {
  ACTIVE,
  HOLDING,
  DEAD,
  WON
};

enum class Action {
  UNKNOWN,
  HIT,
  HOLD
};

struct hand_t
{
  int identifier;
  HandState state;
  Action action;
  std::vector<card_t > cards;
};

int count(hand_t& );
std::string toString(hand_t& );

#endif // BLACKJACK_HAND_H__
