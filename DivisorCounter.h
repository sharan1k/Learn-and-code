#ifndef DIVISOR_COUNTER_H
#define DIVISOR_COUNTER_H

class DivisorCounter
{
public:
    int countPairsWithEqualDivisors(int upperLimit);

private:
    int countDivisors(int number);
};

#endif
