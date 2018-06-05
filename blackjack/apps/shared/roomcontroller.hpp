#ifndef BLACKJACK_ROOMCONTROLLER_HPP__
#define BLACKJACK_ROOMCONTROLLER_HPP__

#include <blackjack/table.hpp>

#include <stateless++/state_machine.hpp>

struct Room {
  table_t table;
  stateless::state_machine<TableState, TableActions > fsm;
};

class RoomController {

private:
  std::vector<Room > rooms_;

public:
  table_t& findTable(int identifier);
  const table_t& findTable(int identifier) const;

};

table_t getTable(RoomController& rooms, int identifier);

#endif // BLACKJACK_ROOMCONTROLLER_HPP__