#include "lock.h"

// -------------------------------------------
// TEST AND SET EXPONENTIAL
// -------------------------------------------
void TAS_EXP_Lock::lock()
{
	int time = 1;
	register unsigned char _res = 1;
	while (_res != 0)
	{
		 __asm__ __volatile__(
			"xchg %0,%1"
			: "+q" (_res)
			: "m" (l)
			: );
		if (_res == 1) {
			time *= 2;
			for (int i=0; i<time; i++) {
				__asm__ __volatile__("rep;nop");
			}
		}
	}
}

bool TAS_EXP_Lock::try_lock() 
{ 
	unsigned char res = 1;
	__asm__ __volatile__(
		"xchgb %0,%1"
		: "+q" (res)
		: "m" (l)
		: );
	return !(res);
}

void TAS_EXP_Lock::unlock() 
{ 
	l = 0;
	__asm__ __volatile__(
		"" 
		:
		:
		: "memory" );
}

// -------------------------------------------
// TEST AND SET
// -------------------------------------------
void TAS_Lock::lock()
{
	register unsigned char _res = 1;
	while (_res != 0)
	{
		__asm__ __volatile__(
			"xchg %0,%1"
			: "+q" (_res)
			: "m" (l)
			: );
	}
}

bool TAS_Lock::try_lock() 
{ 
	unsigned char res = 1;
	__asm__ __volatile__(
		"xchgb %0,%1"
		: "+q" (res)
		: "m" (l)
		: );
	return !(res);
}

void TAS_Lock::unlock() 
{
	l = 0;
	__asm__ __volatile__(
		"" 
		:
		:
		: "memory" );
}


void TATAS_Lock::lock() {
	unsigned char res = 1;
	while (l != 0);
	while (res != 0)
	{
		__asm__ __volatile__(
			"xchgb %0,%1"
			: "+q" (res)
			: "m" (l)
			: );
	}
}

bool TATAS_Lock::try_lock() { 
	unsigned char res = 1;
	__asm__ __volatile__(
		"xchgb %0,%1"
		: "+q" (res)
		: "m" (l)
		: );
	return !(res);
}

void TATAS_Lock::unlock() { 
	l = 0;
	__asm__ __volatile__(
		"" 
		:
		:
		: "memory" );
}


// -------------------------------------------
// OMP Lock
// -------------------------------------------
void OMP_Lock::lock() {
	omp_set_lock(&l);
}

bool OMP_Lock::try_lock() {
	return omp_test_lock(&l);
}

void OMP_Lock::unlock() {
	omp_unset_lock(&l);
}

