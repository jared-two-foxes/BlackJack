#ifndef BLACKJACK_PLAYER_H__
#define BLACKJACK_PLAYER_H__

#include <blackjack/hand.h>

#include <vector>

struct player_t
{
  int identifier;
  std::vector<hand_t > hands;
  int wallet;
  int betValue;
};



#endif // BLACKJACK_PLAYER_H__
