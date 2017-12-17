#ifndef BLACKJACK_DECK_H__
#define BLACKJACK_DECK_H__

#include <blackjack/card.h>

#include <vector>

struct deck_t
{
  std::vector<card_t > cards;
};

deck_t new_deck();
deck_t shuffle(deck_t& deck);
card_t draw(deck_t& deck);

#endif // BLACKJACK_DECK_H__
