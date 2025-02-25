#include <iostream>
#include <string>

class Wallet {
private:
    float value;

public:
    Wallet(float initialValue = 0.0f) : value(initialValue) {}

    float getTotalMoney() const {
        return value;
    }

    void addMoney(float amount) {
        value += amount;
    }

    void subtractMoney(float amount) {
        value -= amount;
    }
};

class Customer {
private:
    std::string firstName;
    std::string lastName;
    Wallet wallet;

public:
    Customer(const std::string& firstName, const std::string& lastName, float initialAmount)
        : firstName(firstName), lastName(lastName), wallet(initialAmount) {}

    std::string getFirstName() const {
        return firstName;
    }

    std::string getLastName() const {
        return lastName;
    }

    bool makePayment(float amount) {
        if (wallet.getTotalMoney() >= amount) {
            wallet.subtractMoney(amount);
            return true;
        }

        return false;
    }

    void addMoneyToWallet(float amount) {
        wallet.addMoney(amount);
    }
};

int main() {
    Customer customer("sharan", "KM", 5000.0f);
    float payment = 2000.0f;

    bool paymentSuccessful = customer.makePayment(payment);

    if (paymentSuccessful) {
        std::cout << "Payment of Rs." << payment << " was successful." << std::endl;
    } else {
        std::cout << "Not enough money. Come back later." << std::endl;
    }

    return 0;
}
