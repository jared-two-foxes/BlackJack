#ifndef BLACKJACK_WRITER_HPP__
#define BLACKJACK_WRITER_HPP__

#include <blackjack/table.h>
#include <blackjack/hand.h>

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

inline framework::FlowLayout<> renderHand(hand_t& h) {
  return framework::FlowLayout<> {
    framework::Text(h.identifier),
    framework::Text(" ("),
    framework::Text(h.player),
    framework::Text("): "),
    framework::Text(toString(h))
  }; 
}

inline framework::StackLayout<> renderHands(table_t& t) {
  framework::StackLayout<> ret;
  for (hand_t& h : t.hands) {
    ret.children.push_back(renderHand(h));
  }
  return ret;
}

inline framework::StackLayout<> renderTable(table_t& t) {
  return framework::StackLayout<>{
    renderState(t),
    framework::FlowLayout<>{ framework::Text("Players: "), framework::Text(t.players.size()) },
    renderHands(t),
    framework::FlowLayout<>{ framework::Text("Dealers Hand: "), renderHand(t.dealer) }
  };
}

#endif // BLACKJACK_WRITER_HPP__