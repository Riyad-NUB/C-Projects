#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    int random, guess, count=0;
    srand(time(NULL));


    printf("........Welcome to the world of guessing number........ \n");
    printf("\n");
    random= rand()%100 +1;

    do{

    printf("Guess a number between 1-100 : ");
    scanf("%d", &guess);
    count++;

    if (guess < random){
        printf("\nGuess a bigger number\n");
    }
    else if(guess > random){
        printf("\nGuess a smaller number\n");
    }
    else{
        printf("\nCongratulations !!! You have successfully guessed the correct number in %d attemped\n", count);
        printf("\n......Thank You......\n");
        printf("Developed by: Md. Riyad\n");
    }
    } while(guess != random);

  return 0;

}
