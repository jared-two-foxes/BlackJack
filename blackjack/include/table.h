#ifndef BLACKJACK_TABLE_H__
#define BLACKJACK_TABLE_H__

#include <blackjack/deck.h>
#include <blackjack/player.h>

#include <vector>

enum class TableState{
  WAITING_TO_START,
  WAITING_FOR_BETS,
  DEALING,
  WAITING_FOR_ACTIONS,
  REWARD
};

enum class TableActions {
  UNKNOWN = -1,
  JOIN,
  BET,
  DEAL,
  SET
};

struct table_t
{
  int identifier;
  TableState state;
  deck_t deck;
  std::vector<player_t > players;
  std::vector<hand_t> hands;
  hand_t dealer;
  int betSize;
};

char* serialize(table_t& t, char* data);
char* deserialize(char* buffer, table_t* t);

table_t createTable();
void addPlayer(table_t& , player_t& player);
player_t* findPlayer(table_t& , int player_id);
hand_t* addHand(table_t& , player_t& );
hand_t* findHand(table_t& , int hand_id);

std::size_t calculateSize(table_t& );

std::string toString(TableState state);


#endif // BLACKJACK_TABLE_H__
