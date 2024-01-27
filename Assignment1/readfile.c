/*
- using Ubuntu 22.04.3 LTS as operating system
- VSCode as IDE
- Ubuntu is running on Docker
*/

#include <stdio.h>

int main() {
    // Declare variables to store user input
    int num1, num2;

    // Prompt the user to enter the first number
    printf("Enter the first number: ");
    scanf("%d", &num1);

    // Prompt the user to enter the second number
    printf("Enter the second number: ");
    scanf("%d", &num2);

    // Perform the addition
    int sum = num1 + num2;

    // Display the result
    printf("The sum of %d and %d is: %d\n", num1, num2, sum);

    return 0;
}
