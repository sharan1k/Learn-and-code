#include <iostream>
#include <vector>

struct Query {
    int leftIndex;
    int rightIndex;
};

std::vector<int> computePrefixSums(const std::vector<int>& elementsArray) {
    int numberOfElements = elementsArray.size();
    std::vector<int> prefixSum(numberOfElements + 1, 0);
    
    for (int index = 1; index <= numberOfElements; index++) {
        prefixSum[index] = prefixSum[index - 1] + elementsArray[index - 1];
    }
    
    return prefixSum;
}

int calculateSubarrayMean(const Query& query, const std::vector<int>& prefixSum) {
    int subarraySum = prefixSum[query.rightIndex] - prefixSum[query.leftIndex - 1];
    int elementsInSubarray = query.rightIndex - query.leftIndex + 1;
    return subarraySum / elementsInSubarray; 
}

void processQueries(const std::vector<Query>& queries, const std::vector<int>& prefixSum) {
    std::cout << "Output" << std::endl;

    for (const auto& query : queries) {
        std::cout << calculateSubarrayMean(query, prefixSum) << std::endl;
    }
}

int main() {
    int numberOfElements, numberOfQueries;
    std::cout << "Input" << std::endl;
    std::cin >> numberOfElements >> numberOfQueries;
    std::vector<int> elementsArray(numberOfElements);
    std::vector<Query> queries(numberOfQueries);

    for (int index = 0; index < numberOfElements; index++) {
        std::cin >> elementsArray[index];
    }

    for (int index = 0; index < numberOfQueries; index++) {
        std::cin >> queries[index].leftIndex >> queries[index].rightIndex;
    }

    std::vector<int> prefixSum = computePrefixSums(elementsArray);
    processQueries(queries, prefixSum);
    return 0;
}
