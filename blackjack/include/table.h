#ifndef BLACKJACK_TABLE_H__
#define BLACKJACK_TABLE_H__

#include <blackjack/deck.h>
#include <blackjack/player.h>

#include <vector>

enum class TableState
{
    WAITING_TO_START,
    WAITING_FOR_BETS,
    WAITING_FOR_ACTIONS,
    REWARD
};

struct table_t
{
  TableState state;
  deck_t deck;
  std::vector<player_t > players;
  hand_t dealer;
};

table_t createTable();
void addPlayer(table_t& , player_t& player);
player_t& getPlayer(table_t& , int player_id);
hand_t& getHand(table_t& , int player_id, int hand_id);
void dealIn(table_t& );
void deal(table_t& );
bool allBetsIn(table_t& );
bool allActionsIn(table_t& );
void checkHands(table_t& );
bool allOut(table_t& );


std::size_t calculateSize(table_t& );
void printToConsole(table_t& );



#endif // BLACKJACK_TABLE_H__
