#ifndef BLACKJACK_SERVER_APPLICATION_HPP__
#define BLACKJACK_SERVER_APPLICATION_HPP__

#include <framework/kernel/kernel.hpp>
#include <framework/online/server.hpp>
#include <framework/terminal/terminal.hpp>

#include <blackjack/message.hpp>
#include <blackjack/table.hpp>
#include <blackjack/timer.hpp>

// #include <stateless++/state_machine.hpp>

#include <functional>
#include <map>


class Application
{
private:
  framework::VirtualTerminal vt_;
  
  framework::Server server_;
  framework::Kernel<MessageTypes, std::function<void (message_body_t)> > kernel_;
  
public:
  Application();
  ~Application();

  void setup(int argc, char** argv);
  void processMessage(const zmq::message_t& msg);
  void pushTableState();
  int  run();

 private:
    void _setupKernel();

};

#endif // BLACKJACK_SERVER_APPLICATION_HPP__
