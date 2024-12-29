//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include "stm32f769xx.h"
#include "hello.h"

#include<stdint.h>

char choice;
int x, y;
//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------

int main(void)
{
    Sys_Init(); // This always goes at the top of main (defined in init.c)

    printf("\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout);

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOJ_CLK_ENABLE();

    GPIO_InitTypeDef InitStructA;
    GPIO_InitTypeDef InitStructJ;

    InitStructA.Pin = GPIO_PIN_0;
	InitStructA.Mode = GPIO_MODE_INPUT;
	InitStructA.Pull = GPIO_PULLDOWN;

    InitStructJ.Pin = GPIO_PIN_5;
	InitStructJ.Mode = GPIO_MODE_OUTPUT_PP;

	HAL_GPIO_Init(GPIOA, &InitStructA);
	HAL_GPIO_Init(GPIOJ, &InitStructJ);

    HAL_Delay(1000); // Pause for a second. This function blocks the program and uses the SysTick and
                     // associated handler to keep track of time.

    //2D array for wall detection
    char maze[15][13] ={{1,1,1,1,1,1,1,1,1,1,1,1,1},
    					{0,0,1,0,0,0,0,0,0,0,0,0,0},
						{1,0,1,0,1,0,1,1,1,1,1,1,1},
						{1,0,1,0,1,0,0,0,1,0,0,0,1},
						{1,0,1,1,1,1,1,0,1,1,1,0,1},
						{1,0,1,0,0,0,1,0,0,0,1,0,1},
						{1,0,0,0,1,0,1,0,0,1,1,0,1},
						{1,0,0,0,1,0,1,1,0,0,0,0,1},
						{1,0,1,1,1,0,0,1,1,1,1,0,1},
						{1,0,1,0,1,0,0,1,0,0,0,0,1},
						{1,0,1,0,1,1,0,1,1,1,0,1,1},
						{1,0,1,0,0,1,0,0,0,1,0,0,1},
						{1,0,1,1,0,1,1,1,0,1,1,0,1},
						{1,0,0,0,0,1,0,0,0,0,0,0,1},
						{1,1,1,1,1,1,1,1,1,1,1,1,1}};

    printf("\e[0m\033[1;0H-------------");
         printf("\033[2;0H  |          ");
         printf("\033[3;0H| | | ------|");
         printf("\033[4;0H| | |   |   |");
         printf("\033[5;0H| |---- --- |");
         printf("\033[6;0H| |   |   | |");
         printf("\033[7;0H|   | |  -| |");
         printf("\033[8;0H|   | |-    |");
         printf("\033[9;0H| --|  |--- |");
        printf("\033[10;0H| | |  |    |");
        printf("\033[11;0H| | -- |-- -|");
        printf("\033[12;0H| |  |   |  |");
        printf("\033[13;0H| |- |-- |- |");
        printf("\033[14;0H|    |      |");
        printf("\033[15;0H-------------");
    printf("\033[2;0H");
    fflush(stdout);
    x = 0;
    y = 1;
    while(1)
    {
        //Check to see if pushbutton is pressed
    	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0))
    	{
    		//reset maze, coords and cursor
    		x = 0;
    		y = 2;
			printf("\e[0m\033[1;0H-------------\033[2;0H  |          \033[3;0H| | | ------|\033[4;0H| | |   |   |");
			printf("\033[5;0H| |---- --- |\033[6;0H| |   |   | |\033[7;0H|   | |  -| |\033[8;0H|   | |-    |");
			printf("\033[9;0H| --|  |--- |\033[10;0H| | |  |    |\033[11;0H| | -- |-- -|\033[12;0H| |  |   |  |");
			printf("\033[13;0H| |- |-- |- |\033[14;0H|    |      |\033[15;0H-------------\033[2;0H");
			fflush(stdout);
    	}

    	choice = getchar();
        if(choice == '\167') //w
        {
        	if(y > 0)
        	{
        		if (maze[y-1][x] == 0) { //check to see if the destination is a wall
        			printf("\e[46m \e[0m\e[A\e[D");
        			y = y - 1;
        			maze[y][x] = 2;
        		}
        		else if (maze[y-1][x] == 2) { //check to see if the destination is a wall
        			printf("\e[49m \e[A\e[D");
        			y = y - 1;
        			maze[y][x] = 0;
        		}
        	}

        }
        else if(choice == '\141') //a
		{
        	if(x > 0)
        	{
        		if (maze[y][x-1] == 0) {
        			printf("\e[46m \e[0m\e[D\e[D");
        			x = x - 1;
        			maze[y][x] = 2;
        		}
        		else if (maze[y][x-1] == 2) {
        			printf("\e[49m \e[0m\e[D\e[D");
        			x = x - 1;
        			maze[y][x] = 0;
        		}
        	}
		}
        else if(choice == '\163') //s
        {
        	if(y < 14)
        	{
        		if (maze[y+1][x] == 0) {
        			printf("\e[46m \e[0m\e[B\e[D");
        			y = y + 1;
        			maze[y][x] = 2;
        		}
        		else if (maze[y+1][x] == 2) {
        			printf("\e[49m \e[0m\e[B\e[D");
        			y = y + 1;
        			maze[y][x] = 0;
        		}
        	}
        }
        else if(choice == '\144') //d
        {
        	if(x < 12)
        	{
        		if (maze[y][x+1] == 0) {
        			printf("\e[46m \e[0m");
        			x = x + 1;
        			maze[y][x] = 2;
        		}
        		else if (maze[y][x+1] == 2) {
        			printf("\e[49m \e[0m");
        			x = x + 1;
        			maze[y][x] = 0;
        		}
        	}
		}

        if (x == 12 && y == 1) { //reached end coordinate
        	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, GPIO_PIN_SET); //1
        } else {
        	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, GPIO_PIN_RESET); //0
        }

        fflush(stdout);
    }
}

