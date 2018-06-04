
#include <blackjack/hand.hpp>
#include <blackjack/serialize.hpp>


char* serialize(hand_t& h, char* data) {
  data = serialize(h.identifier, data);
  data = serialize(h.player, data);
  data = serialize(h.cards, data);
  data = serialize(h.state, data);
  data = serialize(h.action, data);
  
  return data;
}

char* deserialize(char* buffer, hand_t* h) {
  buffer = deserialize(buffer, &(h->identifier));
  buffer = deserialize(buffer, &(h->player));
  buffer = deserialize(buffer, &(h->cards));
  buffer = deserialize(buffer, &(h->state));
  buffer = deserialize(buffer, &(h->action));
  
  return buffer;
}

bool isValid(hand_t& h ) {
  return (h.identifier != -1);
}

int count(hand_t& hand) {
  int v = 0;
  for (card_t& c : hand.cards) {
    v += c.value;
  }
  return v;
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

std::string stateToString(HandState state) {
  if (state == HandState::ACTIVE) {
    return "ACTIVE";
  }
  else if (state == HandState::HOLDING) {
    return "HOLDING";
  }
  else if (state == HandState::BUST) {
    return "BUST";
  }
  else if (state == HandState::WON) {
    return "WON";
  }
  
  return "UNKNOWN"; //< return U for unknown?
}


std::string toString(hand_t& h) {
  std::string out("[");
  for (card_t& c : h.cards) {
    out += (valueToString(c.value) + suitToString(c.suit));
    out += ","; //< @todo - handle removing this for the last card
  }
  out += "] (";
  out += std::to_string(count(h));
  out += ") ";
  out += stateToString(h.state);
  return out;
}
