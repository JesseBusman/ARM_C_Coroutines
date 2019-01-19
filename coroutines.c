#include <MK60F12.H>
#include "..\U1_4_net\ENET_MKxx.h"
#include "..\U1_shared\MarRtosSub.h"
#include "..\U1_shared\bl_pgr.h"  /* standard variables in project global ram */

#include "coroutines.h"

uint32_t CoKill(Coroutine* coroutine)
{
	Free(coroutine);
	return 0;
}

uint32_t CoHasFinished(Coroutine* coroutine)
{
	return coroutine->coroutineSP == 0;
}

void CoEnd(Coroutine* coroutine)
{
	if (!CoHasFinished(coroutine))
	{
		C_putline("ERROR: coroutine Return() called, but not inside coroutine.\r\n");
		while(1) SysTimeWait(1000*1000);
	}
	CoKill(coroutine);
	return;
}


Coroutine* CounterCoroutine(uint32_t value, Coroutine* co)
{
	uint32_t runningCount;
	runningCount = value;
	while (1)
	{
		C_printf("CounterCoroutine yielding a running count of %i8\r\n", runningCount); SysTimeWait(20*1000);
		runningCount += CoYield(runningCount, co);
	}
}



Coroutine* FibonacciCoroutine(uint32_t depth, Coroutine* co)
{
	uint32_t a, i;
	uint32_t b;
	uint32_t temp;
	Coroutine* subCoroutine;
	C_putline("\r\nStart of FibonacciCoroutine\r\n"); SysTimeWait(20*1000);
	a = 1;
	b = 0;
	while(1)
	{
		C_putline("Init CounterCoroutine\r\n"); SysTimeWait(20*1000);
		subCoroutine = CoInit(&CounterCoroutine, 256);
		C_putline("Run CounterCoroutine\r\n"); SysTimeWait(20*1000);
		for (i=0; i<5; i++)
			C_printf("yielded: %i8\r\n", CoRun(15, subCoroutine));
		CoKill(subCoroutine);
		
		/*subCoroutine = CoInit(&FibonacciCoroutine, 64);
		C_putline("\r\nSubcoroutine: ");SysTimeWait(20*1000);
		for (i=0; i<a && i<25; i++)
		{
			C_printf("%i8 ", CoRun(0, subCoroutine)); SysTimeWait(20*1000);
		}
		C_putline(" - done");SysTimeWait(20*1000);
		CoKill(subCoroutine);
		C_putline("\r\n");SysTimeWait(20*1000);*/
		
		
		CoYield(b, co);
		temp = a + b;
		b = a;
		a = temp;
	}
}

void RunCoroutineTest(void)
{
	uint32_t i;
	uint32_t val;
	Coroutine* gen;
	
	//while(1) SysTimeWait(100*1000);
	
	while(1)
	{
		C_putline("Initializing coroutine...\r\n"); SysTimeWait(20*1000);
		gen = CoInit(&FibonacciCoroutine, 256);
		C_putline("Starting Run loop...\r\n"); SysTimeWait(20*1000);
		for (i=0; i<25; i++)
		{
			//C_putline("Coroutine yielded:\r\n");SysTimeWait(20*1000);
			val = CoRun(0, gen);
			C_printf(" %i8\r\n", val);//SysTimeWait(20*1000);
			if (CoHasFinished(gen))
			{
				C_putline("Coroutine returned!\r\n");SysTimeWait(20*1000);
				break;
			}
		}
		C_putline("Killing Coroutine...\r\n"); SysTimeWait(20*1000);
		CoKill(gen);
		C_putline("Coroutine test finished!\r\n");
		
		SysTimeWait(2000*1000);
	}
}





