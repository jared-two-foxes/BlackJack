#ifndef BLACKJACK_CLIENTAPPLICATION_HPP__
#define BLACKJACK_CLIENTAPPLICATION_HPP__

#include <blackjack/message.h>
#include <blackjack/table.h>

#include <zmq.hpp>
#include <memory>

class ClientApplication
{
private:
  table_t                          m_table;
  std::unique_ptr<zmq::context_t > m_context;
  std::unique_ptr<zmq::socket_t >  m_subscriberSocket;
  std::unique_ptr<zmq::socket_t >  m_serverSocket;

  public:
    ClientApplication();

    int Setup();
    int Run();

  private:
    int SetupCommunicationLayer();
    void PollForServerUpdate();
    bool ProcessActionRequest(const std::string& action, message_t& msg);
    void UpdateActionPrompt();

};

#endif // BLACKJACK_CLIENTAPPLICATION_HPP__
