
#include <blackjack/table.h>

#include <iostream>


table_t createTable() {
  table_t table;
  return table;
}

void addPlayer(table_t& table, player_t& player) {
  table.players.push_back(player);
}

player_t& getPlayer(table_t& t, int player_id) {
  for (player_t& p : t.players) {
    if (p.identifier == player_id) {
      return p;
    }
  }

  // return a null player?
  return player_t();
}

hand_t& getHand(table_t& t, int player_id, int hand_id) {
  player_t& p = getPlayer(t, player_id);
  for (hand_t& h : p.hands) {
    if (h.identifier == hand_id) {
      return h;
    }
  }

  // return a null hand.
  return hand_t();
}

void deal(table_t& t) {
  //deal a card to each player
  for(player_t& p : t.players) {
    for(hand_t& h : p.hands) {
      if (h.state == HandState::ACTIVE && h.action != Action::HOLD) {
        card_t card = draw(t.deck);
        h.cards.push_back(card);
      }
    }
  }

  //deal a card to the dealers hand.
  t.dealer.cards.push_back(draw(t.deck));
}

bool allBetsIn(table_t& t) {
  // Check that all the hands have actions associated with them.
  bool betsIn = false;
  return betsIn;
}

bool allActionsIn(table_t& t) {
  bool actionsIn = true;
  for (player_t& p : t.players) {
    for (hand_t& h : p.hands) {
      if (h.action != Action::UNKNOWN && h.state != HandState::DEAD) {
        actionsIn = false;
        break;
      }
    }
  }
  return actionsIn;
}

void checkHands(table_t& t) {
  for (player_t& p : t.players) {
    for (hand_t& h : p.hands) {
      if (h.state == HandState::ACTIVE) {
        int cnt = count(h);
        if (cnt == 21) {
          h.state = HandState::WON;
        }
        else if (cnt > 21) {
          h.state = HandState::DEAD;
        }
      }
    }
  }

  if (count(t.dealer) == 21) {
    t.dealer.state = HandState::WON;
  }
  else if (count(t.dealer) > 21) {
    t.dealer.state = HandState::DEAD;
  }
}

bool allOut(table_t& t) {
  bool allOut = true;
  for (player_t& p : t.players) {
    for (hand_t& h : p.hands) {
      if (h.state == HandState::ACTIVE) {
        allOut = false;
        break;
      }
    }
  }
  return allOut;
}


std::size_t calculateSize(table_t& t) {
  std::size_t size = sizeof(table_t);
  for (player_t& p : t.players) {
    size += sizeof(player_t);
    for (hand_t& h : p.hands) {
      size += sizeof(hand_t);
      size += (sizeof(card_t) * h.cards.size());
    }
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

void printToConsole(table_t& t) {
  std::cout << "State: " << std::to_string((int)t.state) << ", " << toString(t.state) << std::endl;
  std::cout << "Players: " << std::to_string(t.players.size()) << std::endl;
  for (player_t& p : t.players) {
    for (hand_t& h : p.hands) {
      std::cout << std::to_string(h.identifier) << " (" << std::to_string(p.identifier) << "): " << toString(h) << std::endl;
    }
  }
  std::cout << "Dealers Hand: " << toString(t.dealer) << std::endl;
}
