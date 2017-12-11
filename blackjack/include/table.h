#ifndef BLACKJACK_TABLE_H__
#define BLACKJACK_TABLE_H__

#include <blackjack/deck.h>
#include <blackjack/player.h>
#include <vector>

struct table_t
{
  deck_t deck;
  std::vector<player_t > players;
  hand_t dealer;
};

void addPlayer(table_t& table, player_t& player);
void deal(table_t& table);

#endif // BLACKJACK_TABLE_H__
