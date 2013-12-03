#ifndef __LOCK_H__
#define __LOCK_H__

#include <omp.h>

class Own_Lock
{
	private:
		volatile int l;

	public:

		Own_Lock() { l = 0;};
		~Own_Lock() {};

		inline void lock() {
			register unsigned char res = 1;
			while (res != 0)
			{
				__asm__ __volatile__(
					"xchg %0,%1"
					: "+q" (res), "+m" (l)
					:
					: );
			}
		};

		inline bool try_lock() { 
			register unsigned char res = 1;
			__asm__ __volatile__(
				"xchg %0,%1"
				: "+q" (res), "+m" (l)
				:
				: );
			return !static_cast<bool>(res);
		};

		inline void unlock() { 
			l = 0;
		    __asm__ __volatile__(
				"mfence" 
				:
				:
				:);
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

typedef Omp_Lock lock_t;


#endif
