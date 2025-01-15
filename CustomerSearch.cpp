#include <iostream>
#include <vector>
#include <string>
#include <sstream>

class Customer {
private:
    int customerID;
    std::string companyName;
    std::string contactName;
    std::string country;

public:
    Customer(int customerID, std::string companyName, std::string contactName, std::string country)
        : customerID(customerID), companyName(companyName), contactName(contactName), country(country) {}

    int getCustomerID() const {
        return customerID;
    }

    std::string getCompanyName() const {
        return companyName;
    }

    std::string getContactName() const {
        return contactName;
    }

    std::string getCountry() const {
        return country;
    }
};

class CustomerRepository {
private:
    std::vector<Customer> customers;
    std::vector<Customer> searchResult;
    
public:
    CustomerRepository(const std::vector<Customer>& customerList) : customers(customerList) {}
 
    std::vector<Customer> searchByCountry(const std::string& country) {
        for (const auto& customer : customers) {
            if (customer.getCountry().find(country) != std::string::npos) {
                searchResult.push_back(customer);
            }
        }

        return searchResult;
    }
 
    std::vector<Customer> searchByCompanyName(const std::string& company) {
        for (const auto& customer : customers) {
            if (customer.getCompanyName().find(company) != std::string::npos) {
                searchResult.push_back(customer);
            }
        }
        
        return searchResult;
    }

    std::vector<Customer> searchByContact(const std::string& contact) {
        for (const auto& customer : customers) {
            if (customer.getContactName().find(contact) != std::string::npos) {
                searchResult.push_back(customer);
            }
        }
        
        return searchResult;
    }
};
 
std::string exportToCSV(const std::vector<Customer>& customerData) {
    std::stringstream csvData;
 
    for (const auto& customer : customerData) {
        csvData << customer.getCustomerID() << "," << customer.getCompanyName() << "," << customer.getContactName() << "," << customer.getCountry() << std::endl;
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
 
    CustomerRepository customerRepository(customerList);

    std::vector<Customer> indiaCustomers = customerRepository.searchByCountry("India");

    std::cout << "Customers in India:" << std::endl;
    for (const auto& customer : indiaCustomers) {
        std::cout << customer.getCustomerID() << " - " << customer.getCompanyName() << std::endl;
    }

    std::string csvOutput = exportToCSV(indiaCustomers);
    std::cout << "CSV Export (India Customers):\n" << csvOutput;
    
    return 0;
}