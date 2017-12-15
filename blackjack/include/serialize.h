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
  for (auto& i : list) {
    data = serialize(i, data);
  }
  return data;
}

#endif // BLACKJACK_SERIALIZE_H__
