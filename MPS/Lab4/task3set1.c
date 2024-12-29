#include "init.h"

int32_t A = 5;
int32_t B = 2;
int32_t C;

int32_t G = 0;

int32_t var = 0;


// Main Execution Loop
int main(void)
{
	Sys_Init();

	printf("\033[2J\033[;H"); // Erase screen & move cursor to home position
	fflush(stdout);

	asm("LDR r3, =0x00000001");
	asm("LDR r4, =0x00000002");
	asm("ADD r2, r3 , r4"); //1 + 2 = 3
	asm("STR r2,%0" : "=m" (var));
	printf("#1: %lu \r\n", var);

	asm("MUL %[out],%[in1],%[in2]" :[out] "+r" (C) :[in1] "r" (A), [in2] "r" (B)); // 5 * 2 = 10
	printf("#2: %ld \r\n", C);

	asm("MUL %[out],%[in1],%[in2]" :[out] "+r" (C) :[in1] "r" (C), [in2] "r" (B)); // 10 * 2 = 20
	asm("SDIV %[out],%[in1],%[in2]" :[out] "+r" (C) :[in1] "r" (C), [in2] "r" (var)); //20 / 3 = 6
	asm("ADD %[out],%[in1],%[in2]" :[out] "+r" (C) :[in1] "r" (C), [in2] "r" (A)); // 6 + 5 = 11
	printf("#3: %ld \r\n", C);

	asm("SDIV %[out],%[in1],%[in2]" :[out] "+r" (C) :[in1] "r" (C), [in2] "r" (var)); // 11 / 3 = 3
	asm("MLA %[out],%[in1],%[in2],%[in3]" :[out] "+r" (G) :[in1] "r" (C), [in2] "r" (B), [in3] "r" (A)); // 3 * 2 + 5 = 11
	printf("#5: %ld \r\n", G);
}
