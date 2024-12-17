#include <iostream>
#include <vector>

std::vector<int> computePrefixSums(const std::vector<int>& elementsArray) {
    int numberOfElements = elementsArray.size();
    std::vector<int> prefixSum(numberOfElements + 1, 0);
    
    for (int index = 1; index <= numberOfElements; index++) {
        prefixSum[index] = prefixSum[index - 1] + elementsArray[index - 1];
    }
    
    return prefixSum;
}

int calculateSubarrayMean(int leftIndex, int rightIndex, const std::vector<int>& prefixSum) {
    int subarraySum = prefixSum[rightIndex] - prefixSum[leftIndex - 1];
    int elementsInSubarray = rightIndex - leftIndex + 1;
    return subarraySum / elementsInSubarray; 
}

void processQueries(int numberOfQueries, const std::vector<std::pair<int, int>>& queries, const std::vector<int>& prefixSum) {
    std::cout << "Output" << std::endl;

    for (int index = 0; index < numberOfQueries; index++) {
        int leftIndex = queries[index].first;
        int rightIndex = queries[index].second;
        std::cout << calculateSubarrayMean(leftIndex, rightIndex, prefixSum) << std::endl;
    }
}

int main() {
    int numberOfElements, numberOfQueries;
    std::cout << "Input" << std::endl;
    std::cin >> numberOfElements >> numberOfQueries;
    std::vector<int> elementsArray(numberOfElements);
    std::vector<std::pair<int, int>> queries(numberOfQueries);

    for (int index = 0; index < numberOfElements; index++) {
        std::cin >> elementsArray[index];
    }

    for (int index = 0; index < numberOfQueries; index++) {
        std::cin >> queries[index].first >> queries[index].second;
    }

    std::vector<int> prefixSum = computePrefixSums(elementsArray);
    processQueries(numberOfQueries, queries, prefixSum);
    return 0;
}
