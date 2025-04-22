#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>
#include "ATM.h"
#include "ATMExceptions.h"

const std::string HARDCODED_PIN = "2105";
const int MAX_INVALID_ATTEMPTS = 3;
const double DAILY_WITHDRAWAL_LIMIT = 20000.0;
const double INITIAL_ATM_CASH = 20000.0;

ATM::ATM() : pin(HARDCODED_PIN), invalidAttempts(0), cardBlocked(false),
             accountBalance(10000.0), atmCash(INITIAL_ATM_CASH), dailyWithdrawn(0.0)
{
    srand(time(0));
}

bool ATM::login()
{
    std::string inputPin;
    while (invalidAttempts < MAX_INVALID_ATTEMPTS)
    {
        std::cout << "Enter 4-digit PIN: ";
        std::cin >> inputPin;

        if (inputPin == pin)
        {
            std::cout << "Login successful.\n\n";
            return true;
        }
        else
        {
            invalidAttempts++;
            std::cout << "Invalid PIN. Attempts left: " << MAX_INVALID_ATTEMPTS - invalidAttempts << std::endl;
        }
    }

    throw CardBlockedException();
}

bool ATM::simulateServerConnection()
{
    return rand() % 10 != 0;
}

void ATM::showMenu()
{
    int choice;
    do
    {
        std::cout << "\nATM MENU:\n";
        std::cout << "1. Check Balance\n";
        std::cout << "2. Deposit Amount\n";
        std::cout << "3. Withdraw Amount\n";
        std::cout << "4. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        try
        {
            switch (choice)
            {
            case 1:
                checkBalance();
                break;
            case 2:
                depositAmount();
                break;
            case 3:
                withdrawAmount();
                break;
            case 4:
                std::cout << "Thank you for using the ATM.\n";
                break;
            default:
                std::cout << "Invalid choice. Try again.\n";
            }
        }
        catch (const ATMException &exception)
        {
            std::cerr << exception.what() << std::endl;
        }
    } while (choice != 4);
}

void ATM::checkBalance()
{
    std::cout << "Your current account balance is: " << accountBalance << std::endl;
}

void ATM::depositAmount()
{
    double amount;
    std::cout << "Enter deposit amount: ";
    std::cin >> amount;

    if (amount <= 0)
    {
        std::cout << "Invalid amount.\n";
        return;
    }

    accountBalance += amount;
    atmCash += amount;
    std::cout << "Amount deposited successfully.\n";
}

void ATM::withdrawAmount()
{
    double amount;
    std::cout << "Enter withdrawal amount: ";
    std::cin >> amount;

    if (amount <= 0)
    {
        std::cout << "Invalid amount.\n";
        return;
    }

    if (dailyWithdrawn + amount > DAILY_WITHDRAWAL_LIMIT)
        throw DailyLimitExceededException();

    if (amount > accountBalance)
        throw InsufficientFundsException();

    if (amount > atmCash)
        throw ATMInsufficientCashException();

    accountBalance -= amount;
    atmCash -= amount;
    dailyWithdrawn += amount;
    std::cout << "Withdrawal successful. Please collect your cash.\n";
}

bool ATM::isServerConnected()
{
    return simulateServerConnection();
}
