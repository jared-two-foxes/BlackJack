#ifndef BLACKJACK_TABLE_H__
#define BLACKJACK_TABLE_H__

#include <blackjack/deck.h>
#include <blackjack/player.h>
#include <vector>

enum TableState
{
    WAITING_TO_START,
    WAITING_FOR_PLAYERS,
    REWARD
};

struct table_t
{
  deck_t deck;
  TableState state;
  std::vector<player_t > players;
  hand_t dealer;
};

table_t createTable();
void addPlayer(table_t& , player_t& player);
void dealIn(table_t& );
void deal(table_t& );
bool allBetsIn(table_t& );
bool checkHands(table_t& )
bool allOut(table_t& );

#endif // BLACKJACK_TABLE_H__
