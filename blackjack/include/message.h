#ifndef BLACKJACK_MESSAGE_H__
#define BLACKJACK_MESSAGE_H__

enum Message {
    // player commands
    JOIN,
    BET,
    HOLD,
    HIT,
    SPLIT
};

struct message_t
{
  int player_id;
  Message cmd;
};

#endif // BLACKJACK_MESSAGE_H__
