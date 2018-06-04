
#include <blackjack/player.hpp>
#include <blackjack/serialize.hpp>


char* serialize(player_t& p, char* data) {
  data = serialize(p.identifier, data);
  data = serialize(p.hands, data);
  data = serialize(p.stash, data);
  return data;
}

char* deserialize(char* buffer, player_t* p) {
  buffer = deserialize(buffer, &(p->identifier));
  buffer = deserialize(buffer, &(p->hands));
  buffer = deserialize(buffer, &(p->stash));
  return buffer;
}

