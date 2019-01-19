// Include the 'free' function here

#include "coroutines.h"

void CoKill(Coroutine* coroutine)
{
	free(coroutine);
}

uint32_t CoHasFinished(Coroutine* coroutine)
{
	return coroutine->coroutineSP == 0;
}

void CoEnd(Coroutine* coroutine)
{
	if (!CoHasFinished(coroutine)) *(int*)NULL = 1; // Fault: Coroutine must be finished to CoEnd it.
	free(coroutine);
	return;
}
