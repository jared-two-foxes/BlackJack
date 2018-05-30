#ifndef BLACKJACK_SERVER_APPLICATION_HPP__
#define BLACKJACK_SERVER_APPLICATION_HPP__

#include <framework/kernel/serverkernel.hpp>
#include <framework/terminal/terminal.hpp>

#include <blackjack/messagetypes.hpp>
#include <blackjack/table.h>
#include <blackjack/timer.h>

#include <stateless++/state_machine.hpp>

#include <functional>
#include <map>


class Application : public ServerKernel
{
private:
  framework::VirtualTerminal vt_;
  int round_;
  table_t table_;
  timer_t timer_;

  stateless::state_machine<TableState, TableActions > fsm_;
  std::map<MessageTypes, std::function<void (table_t&, int, int) > > callbacks_;

public:
  Application();
  ~Application();

  virtual void setup(int argc, char** argv);
  virtual void processMessage(const zmq::message_t& msg);

  void pushTableState();

private:
  void _setupGameState(int argc, char** argv);
  TableActions _translateMessage(MessageTypes m);

};

#endif // BLACKJACK_SERVER_APPLICATION_HPP__
