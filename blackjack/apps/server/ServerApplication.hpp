#ifndef BLACKJACK_SERVER_APPLICATION_HPP__
#define BLACKJACK_SERVER_APPLICATION_HPP__

#include <framework/kernel/kernel.hpp>
#include <framework/online/server.hpp>
#include <framework/terminal/terminal.hpp>

#include <blackjack/message.hpp>
#include <blackjack/table.hpp>
#include <blackjack/timer.hpp>

#include <shared/roomcontroller.hpp>

#include <functional>
#include <map>

class Application;

struct MessageFn {
  Application& owner;
  explicit MessageFn(Application& app) : owner(app) {}
  virtual void operator()(message_body_t) {};
};

class Application
{
private:
  framework::VirtualTerminal vt_;
  
  framework::Server server_;
  framework::Kernel<MessageTypes, MessageFn > kernel_;
  
  RoomController rooms_;

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
