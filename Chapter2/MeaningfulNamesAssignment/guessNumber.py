import random
def isValidGuess(guessedNumber):
    if guessedNumber.isdigit() and 1<= int(guessedNumber) <=100:
        return True
    else:
        return False

def main():
    targetNumber = random.randint(1,100)
    guessedCorrectly = False
    userGuess = input("Guess a number between 1 and 100:")
    numberOfGuesses = 0
    while not guessedCorrectly:
        if not isValidGuess(userGuess):
            userGuess =input("I wont count this one Please enter a number between 1 to 100")
            continue
        else:
            numberOfGuesses += 1
            userGuess = int(userGuess)

        if userGuess < targetNumber:
            userGuess = input("Too low. Guess again")
        elif userGuess > targetNumber:
            userGuess = input("Too High. Guess again")
        else:
            print("You guessed it in",numberOfGuesses,"guesses!")
            guessedCorrectly = True


main()