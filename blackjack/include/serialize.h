#ifndef BLACKJACK_SERIALIZE_H__
#define BLACKJACK_SERIALIZE_H__

#include <string>
#include <vector>

template <class T>
char* serialize(T& obj, char* data) {
  memcpy(data, &obj, sizeof(T));
  return (data + sizeof(T));
}

template <class T>
char* serialize(std::vector<T>& list, char* data) {
  std::size_t size = list.size();
  data = serialize(size, data);
  for (auto& i : list) {
    data = serialize(i, data);
  }
  return data;
}




template <>
char* serialize(hand_t& h, char* data) {
  data = serialize(h.identifier, data);
  data = serialize(h.state, data);
  data = serialize(h.action, data);
  data = serialize(h.cards, data);
  return data;
}

template <>
char* serialize(player_t& p, char* data) {
  data = serialize(p.identifier, data);
  data = serialize(p.hands, data);
  return data;
}

template <>
char* serialize(table_t& t, char* data) {
  data = serialize(t.state, data);
  //data = serialize(t.deck, data); //< we dont actually want anyone to know whats contained here..?
  data = serialize(t.players, data);
  data = serialize(t.dealer, data);
  return data;
}

template <class T>
char* deserialize(char* buffer, T* obj) {
  memcpy(obj, buffer, sizeof(T));
  return (buffer + sizeof(T));
}

template <class T>
char* deserialize(char* buffer, std::vector<T>* obj) {
  std::size_t size;
  buffer = deserialize(buffer, &size);
  obj->resize(size);
  for (int i = 0; i < size; ++i) {
    buffer = deserialize(buffer, &(*obj)[i]);
  }

  return buffer;
}



template <>
char* deserialize(char* buffer, hand_t* h) {
  buffer = deserialize(buffer, &(h->identifier));
  buffer = deserialize(buffer, &(h->state));
  buffer = deserialize(buffer, &(h->action));
  buffer = deserialize(buffer, &(h->cards));
  return buffer;
}

template <>
char* deserialize(char* buffer, player_t* p) {
  buffer = deserialize(buffer, &(p->identifier));
  buffer = deserialize(buffer, &(p->hands));
  return buffer;
}

template <>
char* deserialize(char* buffer, table_t* t) {
  int state;
  buffer = deserialize(buffer, &state);
  (t->state) = (TableState)state;
  //data = deserialize(buffer, &(t->deck)); //< we dont actually want anyone to know whats contained here..?
  buffer = deserialize(buffer, &(t->players));
  buffer = deserialize(buffer, &(t->dealer));
  return buffer;
}

#endif // BLACKJACK_SERIALIZE_H__
