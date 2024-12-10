import random
def rollDie(numberOfSidesOnDie):
    randomRoll = random.randint(1, numberOfSidesOnDie)
    return randomRoll


def main():
    numberOfSidesOnDie = 6
    userWantsToContinue = True
    while userWantsToContinue:
        userChoice = input("Ready to roll? Enter Q to Quit")
        if userChoice.lower() !="q":
            rollResult = rollDie(numberOfSidesOnDie)
            print("You have rolled a",rollResult)
        else:
            userWantsToContinue=False

main()