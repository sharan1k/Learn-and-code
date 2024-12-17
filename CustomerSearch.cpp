#include <iostream>
#include <vector>
#include <string>
#include <sstream>

class Customer {
public:
    int customerID;
    std::string companyName;
    std::string contactName;
    std::string country;
 
    Customer(int customerID, std::string companyName, std::string contactName, std::string country)
        : customerID(customerID), companyName(companyName), contactName(contactName), country(country) {}
};
 
class CustomerSearch {
private:
    std::vector<Customer> customers;
 
public:
    CustomerSearch(const std::vector<Customer>& customerList) : customers(customerList) {}
 
    std::vector<Customer> searchByCountry(const std::string& country) {
        std::vector<Customer> searchResult;

        for (const auto& customer : customers) {
            if (customer.country.find(country) != std::string::npos) {
                searchResult.push_back(customer);
            }
        }

        return searchResult;
    }
 
    std::vector<Customer> searchByCompanyName(const std::string& company) {
        std::vector<Customer> searchResult;

        for (const auto& customer : customers) {
            if (customer.companyName.find(company) != std::string::npos) {
                searchResult.push_back(customer);
            }
        }
        return searchResult;
    }

    std::vector<Customer> searchByContact(const std::string& contact) {
        std::vector<Customer> searchResult;

        for (const auto& customer : customers) {
            if (customer.contactName.find(contact) != std::string::npos) {
                searchResult.push_back(customer);
            }
        }
        return searchResult;
    }
};
 
std::string exportToCSV(const std::vector<Customer>& customerData) {
    std::stringstream csvData;
 
    for (const auto& customer : customerData) {
        csvData << customer.customerID << "," << customer.companyName << "," << customer.contactName << "," << customer.country << std::endl;
    }
 
    return csvData.str();
}

int main() {
    std::vector<Customer> customerList = {
        Customer(1, "British Telecom", "Varun", "India"),
        Customer(2, "Ribbon Com", "Vinod", "Australia"),
        Customer(3, "AT&T", "Gagan", "India"),
        Customer(4, "Samsung", "Karthik", "India")
    };
 
    CustomerSearch customerSearch(customerList);
    std::vector<Customer> indiaCustomers = customerSearch.searchByCountry("India");
    std::cout << "Customers in India:" << std::endl;

    for (const auto& customer : indiaCustomers) {
        std::cout << customer.customerID << " - " << customer.companyName << std::endl;
    }

    std::string csvOutput = exportToCSV(indiaCustomers);
    std::cout << "CSV Export (India Customers):" << std::endl;
    std::cout << csvOutput;
    return 0;
}