#ifndef ATM_EXCEPTIONS_H
#define ATM_EXCEPTIONS_H

#include <exception>
#include <string>

class ATMException : public std::exception
{
protected:
    std::string message;

public:
    explicit ATMException(const std::string &msg) : message("ATM Error: " + msg) {}

    const char *what() const noexcept override
    {
        return message.c_str();
    }
};

class InvalidPINException : public ATMException
{
public:
    InvalidPINException() : ATMException("Invalid PIN entered.") {}
};

class CardBlockedException : public ATMException
{
public:
    CardBlockedException() : ATMException("Card has been blocked.") {}
};

class ServerConnectionException : public ATMException
{
public:
    ServerConnectionException() : ATMException("Unable to connect to server.") {}
};

class InsufficientFundsException : public ATMException
{
public:
    InsufficientFundsException() : ATMException("Insufficient funds in account.") {}
};

class ATMInsufficientCashException : public ATMException
{
public:
    ATMInsufficientCashException() : ATMException("ATM has insufficient cash.") {}
};

class DailyLimitExceededException : public ATMException
{
public:
    DailyLimitExceededException() : ATMException("Daily withdrawal limit exceeded.") {}
};

#endif
