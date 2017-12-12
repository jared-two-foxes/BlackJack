#ifndef BLACKJACK_TIMER_H__
#define BLACKJACK_TIMER_H__

struct timer_t
{
    time_t expiresAt;
};

void wait(timer_t&, float timeToWait);
bool complete(timer_t& );

#endif // BLACKJACK_TIMER_H__
