#include <iostream>
#include "ATM.h"
#include "ATMExceptions.h"

int main()
{
    ATM atm;

    try
    {
        if (atm.login())
        {
            if (!atm.isServerConnected())
            {
                throw ServerConnectionException();
            }

            atm.showMenu();
        }
    }
    catch (const ATMException &exception)
    {
        std::cerr << exception.what() << std::endl;
    }

    return 0;
}
