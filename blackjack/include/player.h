#ifndef BLACKJACK_PLAYER_H__
#define BLACKJACK_PLAYER_H__

#include <blackjack/hand.h>

#include <vector>
//#include <boost/uuid.hpp>

struct player_t
{
  int identifier;
  std::vector<int > hands; //< identifiers of the hands run by this player?
};



#endif // BLACKJACK_PLAYER_H__
