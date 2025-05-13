#include "DivisorCounter.h"

int DivisorCounter::countPairsWithEqualDivisors(int upperLimit)
{
    if (upperLimit < 2)
    {
        return 0;
    }

    int matchingPairs = 0;

    for (int n = 2; n < upperLimit; ++n)
    {
        int divisorsCurrent = countDivisors(n);
        int divisorsNext = countDivisors(n + 1);

        if (divisorsCurrent == divisorsNext)
        {
            ++matchingPairs;
        }
    }

    return matchingPairs;
}

int DivisorCounter::countDivisors(int number)
{
    int divisorCount = 0;

    for (int i = 1; i <= number; ++i)
    {
        if (number % i == 0)
        {
            ++divisorCount;
        }
    }

    return divisorCount;
}
