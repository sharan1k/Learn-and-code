#include <iostream>
#include "CountryData.h"

void toUpperCase(std::string& sentence) {
    for (char& character : sentence) {
        if (character >= 'a' && character <= 'z') {
            character = character - 'a' + 'A';  
        }
    }
}

std::string getCountryName(const std::string& countryCode) {
    auto countryCodeIterator = countryCodes.find(countryCode);
    
    return (countryCodeIterator != countryCodes.end()) 
            ? countryCodeIterator->second 
            : "Country code not found.";
}

std::vector<std::string> getAdjacentCountries(const std::string& countryCode) {
    auto countryNeighboursIterator = countryNeighbours.find(countryCode);

    return (countryNeighboursIterator != countryNeighbours.end()) 
            ? countryNeighboursIterator->second 
            : std::vector<std::string>{};
}


int main() {
    std::string countryCode;

    while(true)
    {
        std::cout << "Enter the Country Code or 'Q' to quit: ";
        std::cin >> countryCode;

        toUpperCase(countryCode);

        if (countryCode == "Q") {
            std::cout << "Exiting program..." << std::endl;
            break;
        }

        std::string countryName = getCountryName(countryCode);

        if (countryName == "Country code not found.") {
            std::cout << countryName << std::endl;
            continue;
        }

        std::cout << "Country Name: " << countryName << std::endl;

        std::vector<std::string> neighbors = getAdjacentCountries(countryCode);

        if (neighbors.empty()) {
            std::cout << "This country has no adjacent countries." << std::endl;
        } else {
            std::cout << "Adjacent Countries:" << std::endl;
            for (const auto& neighbor : neighbors) {
                std::cout << "- " << neighbor << std::endl;
            }
        }
    }
    
    return 0;
}
