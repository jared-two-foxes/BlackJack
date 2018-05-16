#ifndef BLACKJACK_RULES_HPP__
#define BLACKJACK_RULES_HPP__

#include <blackjack/table.h>

// Check that all the players have defined if they have
// made a bet or not.
bool allBetsIn(table_t& t) ;

// Check that all the players have defined an action
// for each hand which they are responsible for.
bool allActionsIn(table_t& t);

// Check the hand to see if it's still active or not,
// hands over 21 are bust, hands on 21 have won,
// and any other hands are still active unless holding
void checkHand(hand_t& h);

void deal(table_t& t);

void setupTable(table_t& table);

// Check against the dealer.
bool areHandsPopulated(table_t& t);

#endif // BLACKJACK_RULES_HPP__