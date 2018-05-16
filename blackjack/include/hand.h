#ifndef BLACKJACK_HAND_H__
#define BLACKJACK_HAND_H__

#include <blackjack/card.h>

#include <string>
#include <vector>

enum class HandState {
  ACTIVE,
  HOLDING,
  BUST,
  WON
};

enum class HandActions {
  UNKNOWN = -1,
  HIT,
  HOLD
};

struct hand_t
{
  int identifier;
  int player;
  std::vector<card_t > cards;
  HandState state;
  HandActions action;
};

bool isValid(hand_t& );
int count(hand_t& );
std::string toString(hand_t& );


#endif // BLACKJACK_HAND_H__
