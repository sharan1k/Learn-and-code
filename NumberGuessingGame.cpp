#include <iostream>
#include <cstdlib>
#include <ctime>     
#include <string>   

bool isValidGuess(const std::string &input) {
    for (char character : input) {
        if (!isdigit(character)) {
            return false;
        }
    }

    int guess = std::stoi(input);
    return (guess >= 1 && guess <= 100);
}

int getUserGuess() {
    std::string userInput;
    
    while (true) {
        std::cout << "Guess a number between 1 and 100: ";
        std::getline(std::cin, userInput);

        if (isValidGuess(userInput)) {
            return std::stoi(userInput); 
        } else {
            std::cout << "Invalid input. Please enter a valid number between 1 and 100.\n";
        }
    }
}

int generateRandomNumber() {
    return rand() % 100 + 1;
}

void giveFeedback(int guess, int targetNumber) {
    if (guess < targetNumber) {
        std::cout << "Too low. Try again.\n";
    } else if (guess > targetNumber) {
        std::cout << "Too high. Try again.\n";
    }
}

void playNumberGuessingGame() {
    int targetNumber = generateRandomNumber();
    int guessCount = 0;
    bool guessedCorrectly = false;

    while (!guessedCorrectly) {
        int userGuess = getUserGuess();
        guessCount++;

        if (userGuess == targetNumber) {
            std::cout << "Congratulations You guessed the correct number in " << guessCount << " tries.\n";
            guessedCorrectly = true;
        } else {
            giveFeedback(userGuess, targetNumber);
        }
    }
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    playNumberGuessingGame();                  
    return 0;
}
