			AREA	CoroutineCodeArea, CODE, READONLY, ALIGN=3


	; Coroutine* CoInit(Coroutine* (*coroutineFunction)(Coroutine* coroutine, uint32_t runValue), uint32_t stackSizeBytes);
CoInit
	EXPORT CoInit
	
	; Save the caller's r4-r6, lr
	PUSH { r4-r6, lr }
	
	; r4 = function
	MOV r4, r0
	
	; Allocation size:
	; + 8 bytes for coroutineSP and callerSP
	; + user-provided stackSizeBytes
	; + 44 extra bytes to the user-provider stack size so we can store r4-r12, lr, pc   (11 regs * 4 bytes)
	; r5 = 8 + stackSizeBytes + 44 = allocation size
	ADD r5, r1, #52   
	
	; Coroutine* r0 = Malloc(r5) = Malloc(8 + stackSizeBytes + 44)
	MOV r0, r5
	IMPORT malloc
	LDR r2, =malloc
	; LDR r2, [r2]	; indirection: uncomment this if your 'malloc' label points to a function pointer instead of directly to the function
	BLX r2
	
	; Temporarily go to the coroutine's stack.
	MOV r2, sp		; Save our own stack pointer in r2
	ADD r3, r0, r5	; the Malloc'ed pointer (r0) + the malloc'ed size (r5) is 1 word below the bottom of the stack.
	SUB r3, r3, #4	; go up 1 word
	MOV sp, r3		; switch to the coroutine's stack
	
	; Push their initial registers and their initial LR & PC onto their stack
	LDR r6, =CoReturn
	PUSH { r4 }		; pc				pc=function
	PUSH { r6 }		; lr		where:	lr=CoReturn
	PUSH { r4-r12 }	; r4-r12			these contain garbage. it doesn's matter.
	
	; ((Coroutine*)r0)->coroutineSP = sp;
	STR sp, [r0]
	
	; Go back to our own stack
	MOV sp, r2
	
	POP { r4-r6, lr }	; Restore the caller's r4-r6, lr
	
	; return ((Coroutine*)r0);
	BX lr


; IMPORTANT TO UNDERSTAND:
; These code segments will be executed in this order:
; 1. CoroutineRun
; 2. ReturnFromCoYield
; 3. CoYield	OR		CoReturn
; 4. ReturnFromCoroutineRun
; 5. back to 1. CoroutineRun


	;   uint32_t CoRun(uint32_t value, Coroutine* coroutine);
CoRun
	EXPORT CoRun
	
	; Save the caller's registers
	LDR r2, =ReturnFromCoRun
	PUSH { r2 }		; pc
	PUSH { lr }		; lr
	PUSH { r4-r12 }	; r4-r12
	
	; Switch from caller stack to coroutine stack
	STR sp, [r1, #4]	; ((Coroutine*)r1)->callerSP = sp
	LDR sp, [r1, #0]	; sp = ((Coroutine*)r1)->coroutineSP
	
	; Restore the coroutine's registers
	POP { r4-r12 }	; r4-r12
	POP { lr }		; lr
	POP { pc }		; pc
	
	
ReturnFromCoYield
	BX lr




CoYield		;	uint32_t CoYield(uint32_t value, Coroutine* coroutine);
	EXPORT CoYield
	
	; Save the coroutine's registers
	LDR r2, =ReturnFromCoYield
	PUSH { r2 }		; pc
	PUSH { lr }		; lr
	PUSH { r4-r12 }	; r4-12
	
	; Switch from coroutine stack to caller stack
	STR sp, [r1, #0]			; ((Coroutine*)r1)->coroutineSP = sp
	LDR sp, [r1, #4]			; sp = ((Coroutine*)r1)->callerSP
	
	; Restore the caller's registers
	POP { r4-r12 }
	POP { lr }
	POP { pc }

ReturnFromCoRun
	BX lr





CoReturn	;	this runs when the coroutine function does a BX LR
	; Move to the caller's stack
	LDR sp, [r0, #4]			; sp = ((Coroutine*)r0)->callerSP
	
	; Setting coroutineSP to 0 notifies the caller that this coroutine is finished
	MOV r1, #0
	STR r1, [r0]				; ((Coroutine*)r0)->coroutineSP = 0
	
	; Restore the caller's registers
	POP { r4-r12 }
	POP { lr }
	POP { pc }


			END
	
