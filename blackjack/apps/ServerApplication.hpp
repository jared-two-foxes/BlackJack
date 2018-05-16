#ifndef BLACKJACK_SERVER_APPLICATION_HPP__
#define BLACKJACK_SERVER_APPLICATION_HPP__

#include "messagetypes.hpp"

#include <blackjack/table.h>
#include <blackjack/timer.h>

#include <stateless++/state_machine.hpp>

#include <zmq.hpp>

#include <functional>
#include <map>


class Application
{
public:
  typedef stateless::state_machine<HandState, HandActions > HandStateMachine;
  typedef std::pair<int, HandStateMachine > HandInstance;

private:
  table_t table_;
  stateless::state_machine<TableState, TableActions > fsm_;
  timer_t timer_;

  std::vector<HandInstance > handstatemachines_;

  std::unique_ptr<zmq::context_t > context_;
  std::unique_ptr<zmq::socket_t >  publisher_;
  std::unique_ptr<zmq::socket_t >  listener_;
  std::map<MessageTypes, std::function<void (int, int) > > callbacks_;

public:
  Application();
  ~Application();

  void setup(int argc, char** argv);
  int run();

  void pushTableState();

private:
  void setupCommunicationLayer();
  void setupGameState();

  void processJoinMessage(int player_id, int hand_id);
  void processBetMessage(int player_id, int hand_id);
  void processHitMessage(int player_id, int hand_id);
  void processHoldMessage(int player_id, int hand_id);
  void processSplitMessage(int player_id, int hand_id);

};

#endif // BLACKJACK_SERVER_APPLICATION_HPP__
