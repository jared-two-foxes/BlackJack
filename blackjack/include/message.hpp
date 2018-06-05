#ifndef BLACKJACK_MESSAGE_H__
#define BLACKJACK_MESSAGE_H__

#include <blackjack/messagetypes.hpp>

struct message_body_t 
{
  int table_id;
  int player_id;
  int hand_id;
};

struct message_t
{
  MessageTypes   cmd;
  message_body_t data;
};

#endif // BLACKJACK_MESSAGE_H__
