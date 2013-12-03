#ifndef __LOCK_H__
#define __LOCK_H__

#include <omp.h>

class Own_Lock
{
	private:
		volatile char l;

	public:

		Own_Lock() { l = 0;};
		~Own_Lock() {};

		inline void lock() {
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
		};

		inline bool try_lock() { 
			unsigned char res = 1;
			__asm__ __volatile__(
				"xchgb %0,%1"
				: "+q" (res)
				: "m" (l)
				: );
			return !(res);
		};

		inline void unlock() { 
			l = 0;
		    __asm__ __volatile__(
				"" 
				:
				:
				: "memory" );
		};
};

class Omp_Lock
{
	private: 
		omp_lock_t l;
	
	public:
		Omp_Lock() { omp_init_lock(&l); };
		~Omp_Lock() { omp_destroy_lock(&l); };

		inline void lock() {
			omp_set_lock(&l);
		};

		inline bool try_lock() {
			return omp_test_lock(&l);
		};

		inline void unlock() {
			omp_unset_lock(&l);
		};
};

typedef Own_Lock lock_t;


#endif
