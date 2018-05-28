
#include <blackjack/player.h>
#include <blackjack/serialize.h>


char* serialize(player_t& p, char* data) {
  data = serialize(p.identifier, data);
  data = serialize(p.hands, data);
  return data;
}

char* deserialize(char* buffer, player_t* p) {
  buffer = deserialize(buffer, &(p->identifier));
  buffer = deserialize(buffer, &(p->hands));
  return buffer;
}

