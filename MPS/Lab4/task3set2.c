#include <stdio.h>

// Define global constants
int a = 5;       // First integer
int b = 10;      // Second integer
int result_1;

float c = 5.5;
float d = 2.0;
float result_2;

float e = 3;
float f = 5;
float result_3;


float result_4 = 5;
float g = 0.6666667;

int main() {

	Sys_Init();

	printf("\033[2J\033[;H");
	fflush(stdout);
	//Task1
	asm("LDR r1, =0x1");
	asm("LDR r2, =0x2");
	asm("ADD r3, r1, r2");
	//asm("LDR r3, = result_1");
	asm("STR r3, %0" :  "=m" (result_1));


	printf("Task1 Result = %d\r\n", result_1);


	//Task2
	//result_2 = c * d;
	asm("VMUL.F32 %[dest],%[fac1],%[fac2]"
	                             : [dest] "+w" (result_2)
	                             : [fac1] "w" (c) , [fac2] "w" (d));
	//asm("VMUL r3, %0" :  "=m" (result_1));
	printf("Task2 Result = %f\r\n", result_2);


	//Task3
	//evaluate the equation 2x/3 + 5 through floating point addition (VADD), multiplication (VMUL), and division (VDIV).

	asm("VMUL.F32 %[dest],%[fac1],%[fac2]": [dest] "+w" (result_3): [fac1] "w" (result_2) , [fac2] "w" (d));
	asm("VDIV.F32 %[dest],%[fac1],%[fac2]": [dest] "+w" (result_3): [fac1] "w" (result_3) , [fac2] "w" (e));
	asm("VADD.F32 %[dest],%[fac1],%[fac2]": [dest] "+w" (result_3): [fac1] "w" (result_3) , [fac2] "w" (f));

	printf("Task3 Result = %f\r\n", result_3);


	//Task4
	asm("VMLA.F32 %[dest],%[fac1],%[fac2]": [dest] "+w" (result_4): [fac1] "w" (e) , [fac2] "w" (g));
	printf("Task4 Result = %f\r\n", result_4);


	return 0;
}
