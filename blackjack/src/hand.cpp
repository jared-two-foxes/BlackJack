
#include <blackjack/hand.h>

int count(hand_t& hand) {
  int v = 0;
  for (card_t& c : hand.cards) {
    v += c.value;
  }
  return v;
}

char* serialize(hand_t& h, char* data) {
  data = serialize(h.identifier, data);
  data = serialize(h.state, data);
  data = serialize(h.action, data);
  data = serialize(h.cards, data);
  return data;
}

std::string suitToString(char suit) {
  switch (suit) {
  case 0:
    return "C";
  case 1:
    return "S";
  case 2:
    return "D";
  case 3:
    return "H";
  default:
    break;
  }

  return "U"; //< return U for unknown?
}

std::string valueToString(char value) {
  if (value == 0) {
    return "A";
  }
  else if (value < 11) {
    return std::to_string(value);
  }
  else if (value == 11) {
    return "J";
  }
  else if (value == 12) {
    return "Q";
  }
  else if (value == 13) {
    return "K";
  }

  return "U"; //< return U for unknown?
}

std::string toString(hand_t& h) {
  std::string out("[");
  for (card_t& c : h.cards) {
    out += (valueToString(c.value) + suitToString(c.suit));
    out += ","; //< @todo - handle removing this for the last card
  }
  out += "]";
  return out;
}
