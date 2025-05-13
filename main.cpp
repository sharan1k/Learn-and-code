#include "DivisorCounter.h"
#include <iostream>
#include <vector>

int main()
{
    int testCaseCount;
    std::cin >> testCaseCount;

    if (testCaseCount <= 0)
    {
        std::cerr << "Test case count must be positive." << std::endl;
        return 1;
    }

    std::vector<int> inputs(testCaseCount);
    for (int i = 0; i < testCaseCount; ++i)
    {
        std::cin >> inputs[i];
    }

    DivisorCounter counter;

    for (int k : inputs)
    {
        int result = counter.countPairsWithEqualDivisors(k);
        std::cout << result << std::endl;
    }

    return 0;
}
