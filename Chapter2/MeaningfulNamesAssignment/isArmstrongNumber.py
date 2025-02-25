def calculateArmstrongSum(number):
    # Initializing Sum and Number of Digits
    armstrongSum = 0
    numberOfDigits = 0

    # Calculating Number of individual digits
    temporaryNumber = number
    while temporaryNumber > 0:
        numberOfDigits = numberOfDigits + 1
        temporaryNumber = temporaryNumber // 10

    # Finding Armstrong Number
    temporaryNumber = number
    for index in range(1, temporaryNumber + 1):
        lastDigit = temporaryNumber % 10
        armstrongSum = armstrongSum + (lastDigit ** numberOfDigits)
        temporaryNumber //= 10
    return armstrongSum


# End of Function

# User Input
inputNumber = int(input("\nPlease Enter the Number to Check for Armstrong: "))

if (inputNumber == calculateArmstrongSum(inputNumber)):
    print("\n %d is Armstrong Number.\n" % inputNumber)
else:
    print("\n %d is Not a Armstrong Number.\n" % inputNumber)
