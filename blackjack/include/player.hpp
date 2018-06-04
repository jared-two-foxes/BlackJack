#ifndef BLACKJACK_PLAYER_H__
#define BLACKJACK_PLAYER_H__

#include <blackjack/hand.hpp>

#include <vector>
//#include <boost/uuid.hpp>

struct player_t
{
  int identifier;
  std::vector<int > hands; 	//< identifiers of the hands run by this player?
  int stash; 				//< amount of chips that the player has on the table at this time.
};

char* serialize(player_t& p, char* data);
char* deserialize(char* buffer, player_t* p);


#endif // BLACKJACK_PLAYER_H__
