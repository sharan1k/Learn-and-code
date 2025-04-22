#ifndef ATM_H
#define ATM_H

#include <string>

class ATM
{
private:
    std::string pin;
    int invalidAttempts;
    bool cardBlocked;
    double accountBalance;
    double atmCash;
    double dailyWithdrawn;

    bool simulateServerConnection();

public:
    ATM();
    bool login();
    bool isServerConnected();
    void showMenu();
    void checkBalance();
    void depositAmount();
    void withdrawAmount();
};

#endif
