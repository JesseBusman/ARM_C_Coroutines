#include "coroutines.h"

// This coroutine will return 1, 2, 3, then stop itself.
Coroutine* OneTwoThree(uint32_t runValue, Coroutine* co)
{
	CoYield(1, co);
	CoYield(2, co);
	CoYield(3, co);
	CoReturn(co); // same as 'return co;'
}

// CounterCoroutine consumes its value and adds it to an internal running count.
// It will yield the current running count on every run.
Coroutine* CounterCoroutine(uint32_t runValue, Coroutine* co)
{
	uint32_t runningCount;
	runningCount = value;
	while (1)
	{
		runningCount += CoYield(runningCount, co);
	}
}

// FibonacciCoroutine does not consume any value.
// It will yield the next fibonacci number on every run.
Coroutine* FibonacciCoroutine(uint32_t runValue, Coroutine* co)
{
	uint32_t a, b, temp;
	a = 1;
	b = 0;
	while(1)
	{
		CoYield(b, co);
		temp = a + b;
		b = a;
		a = temp;
	}
}

void Test()
{
	Coroutine* gen;
	Coroutine* adder;
	uint32_t i, yieldValue;
	
	gen = CoInit(&FibonacciCoroutine, 64);
	adder = CoInit(&CounterCoroutine, 64);

	for (i=0; i<25; i++)
	{
		yieldValue = CoRun(0, gen);
		printf("Fibonacci number: %i\r\n", yieldValue);
		printf("Sum so far: %i\r\n", CoRun(yieldValue, adder));
	}
	CoKill(gen);
	CoKill(adder);


	gen = CoInit(&OneTwoThree, 64);
	while (1)
	{
		yieldValue = CoRun(0, gen);
		if (CoHasFinished(gen)) break; // OneTwoThree coroutine can return, so first check if it returned.
		printf("%i\r\n", yieldValue);
	}
	CoEnd(gen);
}
