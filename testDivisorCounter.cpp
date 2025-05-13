#include "DivisorCounter.h"
#include <gtest/gtest.h>

class DivisorCounterTest : public ::testing::Test
{
protected:
    DivisorCounter counter;
};

TEST_F(DivisorCounterTest, ReturnsCorrectResultsForSampleInputs)
{
    EXPECT_EQ(counter.countPairsWithEqualDivisors(3), 1); // Pair: (2,3)
    EXPECT_EQ(counter.countPairsWithEqualDivisors(15), 2);
    EXPECT_EQ(counter.countPairsWithEqualDivisors(100), 15);
}

TEST_F(DivisorCounterTest, HandlesMinimumValidInput)
{
    EXPECT_EQ(counter.countPairsWithEqualDivisors(2), 0);
}

TEST_F(DivisorCounterTest, ReturnsZeroForInvalidInputs)
{
    EXPECT_EQ(counter.countPairsWithEqualDivisors(1), 0);
    EXPECT_EQ(counter.countPairsWithEqualDivisors(0), 0);
    EXPECT_EQ(counter.countPairsWithEqualDivisors(-10), 0);
}

TEST_F(DivisorCounterTest, ReturnsExpectedForKnownSmallCases)
{
    EXPECT_EQ(counter.countPairsWithEqualDivisors(4), 1); // Pair: (2,3)
    EXPECT_EQ(counter.countPairsWithEqualDivisors(10), 1);
}
