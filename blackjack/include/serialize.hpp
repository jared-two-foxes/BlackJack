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

#endif // BLACKJACK_SERIALIZE_H__
