#ifndef BLACKJACK_WRITER_HPP__
#define BLACKJACK_WRITER_HPP__

#include <blackjack/table.hpp>
#include <blackjack/hand.hpp>

#include <framework/terminal/components/stacklayout.hpp>
#include <framework/terminal/components/flowlayout.hpp>
#include <framework/terminal/components/text.hpp>


inline framework::FlowLayout<> renderState(table_t& t) {
  return framework::FlowLayout<>{ 
    framework::Text{ "State: " },
    framework::Text{ toString(t.state) },
    framework::Text{ " (" },
    framework::Text{ (int)t.state },
    framework::Text{ ")" }
  };
}

inline framework::FlowLayout<> renderPlayer(player_t& p) {
  return framework::FlowLayout<> {
    framework::Text("Player "),
    framework::Text(p.identifier),
    framework::Text(": "),
    framework::Text(p.stash)
  };
}

inline framework::StackLayout<> renderPlayers(table_t& t) {
  //Setup title element
  framework::StackLayout<> ret { 
    framework::FlowLayout<>{ framework::Text("Players: "), framework::Text(t.players.size()) }
  };

  // Iterate players and add them to the display
  for (player_t& p : t.players) {
    ret.children.push_back(renderPlayer(p));
  }

  return ret;
}

inline framework::FlowLayout<> renderHand(hand_t& h) {
  return framework::FlowLayout<> {
    framework::Text("Hand "),
    framework::Text(h.identifier),
    framework::Text(" ("),
    framework::Text(h.player),
    framework::Text("): "),
    framework::Text(toString(h))
  }; 
}

inline framework::StackLayout<> renderHands(table_t& t) {
  framework::StackLayout<> ret { 
    framework::FlowLayout<>{ framework::Text("Hands: "), framework::Text(t.hands.size()) }
  };

  for (hand_t& h : t.hands) {
    ret.children.push_back(renderHand(h));
  }
  return ret;
}

inline framework::StackLayout<> renderTable(table_t& t) {
  return framework::StackLayout<>{
    renderState(t),
    renderPlayers(t),
    renderHands(t),
    framework::FlowLayout<>{ framework::Text("Dealers Hand: "), renderHand(t.dealer) }
  };
}

#endif // BLACKJACK_WRITER_HPP__