
#include <blackjack/rules.hpp>

#include <blackjack/identifier.h>
#include <blackjack/message.h>
#include <blackjack/serialize.h>

#include <functional>
#include <iostream>
#include <memory>

#define BET_AMOUNT  10
#define WAIT_PERIOD 10.0


// Check that all the players have defined if they have
// made a bet or not.
bool allBetsIn(table_t& t) {
  bool betsIn = true;
  for (player_t& p : t.players) {
    if (p.hands.size() == 0) {
      betsIn = false;
      break;
    }
  }
  return betsIn;
}

// Check that all the players have defined an action
// for each hand which they are responsible for.
bool allActionsIn(table_t& t) {
  bool actionsIn = false;
  for (hand_t& h : t.hands) {
    if (h.state == HandState::ACTIVE && h.action != HandActions::UNKNOWN) {
      actionsIn = true;
      break;
    }
  }
  return actionsIn;
}

// Check the hand to see if it's still active or not,
// hands over 21 are bust, hands on 21 have won,
// and any other hands are still active unless holding
void checkHand(hand_t& h) {
  int cnt = count(h);
  if (cnt == 21) {
    h.state = HandState::WON;
  }
  else if (cnt > 21) {
    h.state = HandState::BUST;
  }
}

void deal(table_t& t) {
  //deal a card to each player
  for(hand_t& h : t.hands) {
    if (h.state == HandState::ACTIVE && h.action != HandActions::HOLD) {
      card_t card = draw(t.deck);
      h.cards.push_back(card);
      h.action = HandActions::UNKNOWN; //< reset the action.
      checkHand(h);
    }
  }

  //deal a card to the dealers hand.
  t.dealer.cards.push_back(draw(t.deck));
  checkHand(t.dealer);
}

void setupTable(table_t& table) {
  table.deck = shuffle(new_deck());

  // Setup the dealer's hand
  table.dealer.cards.clear();
  table.dealer.state  = HandState::ACTIVE;
  table.dealer.action = HandActions::UNKNOWN;
}

// Check against the dealer.
bool areHandsPopulated(table_t& t, int round) {
  return (t.dealer.cards.size() > (round+1));
}

bool isRoundOver(table_t& t) {
  bool alive = false;
  for (hand_t& h : t.hands) {
    if (h.state == HandState::ACTIVE ||
      h.state == HandState::HOLDING) {
      alive = true;
    }
  }
  return !alive || isBust(t.dealer);
}

bool isBust(hand_t& h) {
  checkHand(h);
  return (h.state == HandState::BUST);
}
