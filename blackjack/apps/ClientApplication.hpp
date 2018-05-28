#ifndef BLACKJACK_CLIENTAPPLICATION_HPP__
#define BLACKJACK_CLIENTAPPLICATION_HPP__

#include <framework/kernel/clientkernel.hpp>
#include <framework/terminal/terminal.hpp>

#include <blackjack/message.h>
#include <blackjack/table.h>

#include <future>


class ClientApplication : public ClientKernel
{
private:
  framework::VirtualTerminal vt_;
  table_t table_;
  std::future<std::string > future_;

  public:
    ClientApplication();

    virtual void setup(int argc, char** argv);
    virtual void processMessage(const zmq::message_t& msg);
    virtual void updateFrame();

  private:
    bool _setupRequest(const std::string& action, message_t& msg);
    void _updateActionPrompt();

};

#endif // BLACKJACK_CLIENTAPPLICATION_HPP__
