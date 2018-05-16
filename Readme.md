# Blackjack

## Rules

* Each game is a round.
* At the start of the game each player chooses to be dealt in by betting x money.
* All players that have chosen to be dealt in is dealt 2 cards in sequence.
* If player is dealt the same cards (value) at start of the game they can "split" their original hand and have 2 hands this round instead
* If a hand's value is over 21 then that hand is "bust" (dead)
* If the dealers hand value is over 21 then all hands that are currently not dead win
* Each turn each hand which is not holding or dead get dealt a card
* Each turn after receiving a card a player can at choose to hold (gain no more cards) or hit (gain a new card) (default)
* Round ends when all players are holding or dead.
* Dealer can never hold while his hand value is less than 17

## Server

Will contain a single blackjack table.

(create a table and wait for players.)

## Table

Once table is full or x time has expired it will go to betting phase.
Once all the bets are received or x time has expired it will go to deal phase.
In deal phase hands will be dealt 1 card unless this is the first deal in which case it will be dealt 2 cards
Once dealing is complete table will go to waiting phase where it will wait for all players


## Board

board state
WAITING

## Hand

Hand state_machine

states
UNKNOWN,
LIVE
HOLDING
BUST

actions
BET
HIT
HOLD
SPLIT
