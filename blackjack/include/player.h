#ifndef BLACKJACK_PLAYER_H__
#define BLACKJACK_PLAYER_H__

#include <blackjack/hand.h>
#include <vector>

struct player_t
{
  std::vector<hand_t > hands;
};

#endif // BLACKJACK_PLAYER_H__
