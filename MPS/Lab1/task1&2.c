
//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include "stm32f769xx.h"
#include "hello.h"

#include<stdint.h>

char choice;
int width;
int height;
int yes;
int no;
//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main(void)
{
    Sys_Init(); // This always goes at the top of main (defined in init.c)

    printf("\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout); // By default, the print buffer (stdout) is "LINE BUFFERED", that is
                    // it only prints when a line is complete, usually done by adding '\n' to the end.
                    // A partial line (without termination in a '\n') may be force to print using this command.
                    // For other labs, we will change the stdout behavior to print immediately after
                    // ANY printf() call, not just ones that contain a '\n'.


    printf("\033[33;44m\033[2;25HPRESS <ESC> OR <CTL>+[ TO QUIT");

    printf("\033[33;44m\033[4;0H--------------------------------------------------------------------------------");
    printf("\033[33;44m\033[14;0H--------------------------------------------------------------------------------");
    printf("\033[33;44m\033[22;0H# of Characters Received:");
    printf("\033[33;44m\033[23;0HPrintable     Non-Printable");
    printf("\033[5;13r\033[5;5H");
    fflush(stdout);
    while(1)
    {

        choice = getchar();
        if (choice == '\033')
            return 1;
        else if(choice < 27 || choice == 127) //127 is backspace
        {
        	printf("\e[s\e[5;31m\e[18;0HThe received value $%02X is \e[4m'not printable'\e[0m\e[33;44m.\e[u", choice);
        	fflush(stdout);
        	no = no + 1;
        	printf("\e[s\e[24;15H%d\e[u", no);
        	fflush(stdout);
        }
        else
        {
        	width = width + 1;

        	printf("%c", choice);
        	fflush(stdout);
        	yes = yes + 1;
        	printf("\e[s\e[24;0H%d\e[u", yes);
        	fflush(stdout);
        	if (width == 72)
        	{
        		printf("\e[B\e[72D");
        		width = 0;
        		height = height + 1;
        		if (height >= 9)
        		{
        			printf("\eD");
        		}
        	}
        }
    }
}

