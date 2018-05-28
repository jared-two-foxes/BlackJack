#ifndef BLACKJACK_MESSAGETYPES_HPP__
#define BLACKJACK_MESSAGETYPES_HPP__

enum class MessageTypes {
  // Player Actions.
  JOIN,
  BET,

  // Hand Actions.
  HOLD,
  HIT,
  SPLIT
};

#endif // BLACKJACK_MESSAGETYPES_HPP__
