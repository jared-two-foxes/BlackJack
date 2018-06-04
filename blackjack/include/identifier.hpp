#ifndef BLACKJACK_IDENTIFIER_H__
#define BLACKJACK_IDENTIFIER_H__

class IdentifierGenerator
{
private:
  static int s_nextIdentifier;

public:
  static int gen() {
    return s_nextIdentifier++;
  }

};

#endif // BLACKJACK_IDENTIFIER_H__
