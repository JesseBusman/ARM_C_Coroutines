#include <MK60F12.H>

typedef struct {
	uint32_t coroutineSP;	// <-- if 0, the coroutine has return;'ed
	uint32_t callerSP;	// <-- this one should never be 0
	uint8_t stack[1];	// <-- this is not really 1 byte. &stack[0] just marks the top of the stack.
} Coroutine;

Coroutine* CoInit(Coroutine* (*coroutineFunction)(uint32_t runValue, Coroutine* coroutine), uint32_t stackSizeBytes);
uint32_t CoRun(uint32_t runValue, Coroutine* coroutine);
uint32_t CoYield(uint32_t yieldValue, Coroutine* coroutine);
void CoReturn(Coroutine* coroutine);
uint32_t CoHasReturned(Coroutine* coroutine);
void CoEnd(Coroutine* coroutine);
void CoKill(Coroutine* coroutine);
