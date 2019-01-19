// Include the 'free' function here

#include "coroutines.h"

void CoKill(Coroutine* coroutine)
{
	free(coroutine);
}

uint32_t CoHasReturned(Coroutine* coroutine)
{
	return coroutine->coroutineSP == 0;
}

void CoEnd(Coroutine* coroutine)
{
	if (!CoHasReturned(coroutine)) *(int*)NULL = 1; // Fault: Coroutine must be finished to CoEnd it.
	free(coroutine);
	return;
}
