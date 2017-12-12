#ifndef BLACKJACK_HAND_H__
#define BLACKJACK_HAND_H__

#include <blackjack/card.h>
#include <vector>

struct hand_t
{
  std::vector<card_t > cards;
  int bet; //< amount that the player has bet on this hand at this stage.
};

int count(hand_t& );

#endif // BLACKJACK_HAND_H__
