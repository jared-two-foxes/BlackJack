
#include <blackjack/table.h>
#include <blackjack/identifier.h>
#include <blackjack/serialize.h>

#include <iostream>


#define MAX_PLAYERS 8

int IdentifierGenerator::s_nextIdentifier = 1;


char* serialize(table_t& t, char* data) {
  data = serialize(t.identifier, data);
  data = serialize(t.state, data);
  //data = serialize(t.deck, data); //< we dont actually want anyone to know whats contained here..?
  data = serialize(t.players, data);
  data = serialize(t.hands, data);
  data = serialize(t.dealer, data);
  return data;
}

char* deserialize(char* buffer, table_t* t) {
  buffer = deserialize(buffer, &(t->identifier));
  int state;
  buffer = deserialize(buffer, &state);
  (t->state) = (TableState)state;
  //data = deserialize(buffer, &(t->deck)); //< we dont actually want anyone to know whats contained here..?
  buffer = deserialize(buffer, &(t->players));
  buffer = deserialize(buffer, &(t->hands));
  buffer = deserialize(buffer, &(t->dealer));
  return buffer;
}

table_t createTable() {
  return table_t();
}

void addPlayer(table_t& table, player_t& player) {
  table.players.push_back(player);
}

player_t* findPlayer(table_t& t, int player_id) {
  for (player_t& p : t.players) {
    if (p.identifier == player_id) {
      return &p;
    }
  }

  // return a null player?
  return nullptr;
}

hand_t* addHand(table_t& t, player_t& p) {
  t.hands.push_back(hand_t());
  hand_t& h = t.hands.back();
  h.identifier = IdentifierGenerator::gen();
  h.state      = HandState::ACTIVE;
  h.action     = HandActions::UNKNOWN;
  p.hands.push_back(h.identifier);
  return &h;
}

hand_t* findHand(table_t& t, int id) {
  for (hand_t& h : t.hands) {
    if (h.identifier == id) {
      return &h;
    }
  }

  // return a null hand.
  return nullptr;
}

bool tableFull(table_t& t) {
  return (t.players.size() >= MAX_PLAYERS);
}

bool allOut(table_t& t) {
  bool allOut = true;
  for (hand_t& h : t.hands) {
    if (h.state == HandState::ACTIVE) {
      allOut = false;
      break;
    }
  }
  return allOut;
}

bool isRoundOver(table_t& t) {
  bool alive = false;
  for (hand_t& h : t.hands) {
    if (h.state == HandState::ACTIVE ||
      h.state == HandState::HOLDING) {
      alive = true;
    }
  }
  return !alive;
}

std::size_t calculateSize(table_t& t) {
  std::size_t size = sizeof(table_t);
  for (player_t& p : t.players) {
    size += sizeof(player_t);
  }
  for (hand_t& h : t.hands) {
    size += sizeof(hand_t);
    size += (sizeof(card_t) * h.cards.size());
  }
  return size;
}


std::string toString(TableState state) {
  if (state == TableState::WAITING_TO_START) {
    return "Waiting to start.";
  }
  else if (state == TableState::WAITING_FOR_BETS) {
    return "Waiting for bets.";
  }
  else if (state == TableState::WAITING_FOR_ACTIONS) {
    return "Waiting for actions.";
  }
  else if (state == TableState::REWARD) {
    return "Reward";
  }

  return "Unknown State";
}