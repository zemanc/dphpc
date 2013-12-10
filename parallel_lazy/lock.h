#ifndef __LOCK_H__
#define __LOCK_H__

#include <omp.h>

// -------------------------------------------
// TEST AND SET EXPONENTIAL
// -------------------------------------------
class TAS_EXP_Lock
{
	private:
		volatile char l;

	public:

		TAS_EXP_Lock() { l = 0;};
		~TAS_EXP_Lock() {};

		inline void lock() {
				int time = 1;
				unsigned char res = 1;
				while (res != 0)
				{
						 __asm__ __volatile__(
								"xchgb %b0,%1"
								: "+q" (res)
								: "m" (l)
								: );
						if (res == 1) {
								time *= 2;
								for (int i=0; i<time; i++) {
										__asm__ __volatile__("nop");
								}
						}
				}
		};

		inline bool try_lock() {
				unsigned char res;
				__asm__ __volatile__(
						"xchgb %b0,%1"
						: "=q" (res), "=m"(l)
						: "0" (1)
						: );
				return !(res);
		};

		inline void unlock() {
			 __asm__ __volatile__(
							"movb $0,%0"
							: "=m" (l)
							:
							: "memory" );
		};
};


// -------------------------------------------
// TEST AND SET
// -------------------------------------------
class TAS_Lock
{
	private:
		volatile char l;

	public:

		TAS_Lock() { l = 0;};
		~TAS_Lock() {};

		inline void lock() {
			unsigned char res = 1;
			while (res != 0)
			{
				__asm__ __volatile__(
					"xchgb %b0,%1"
					: "+q" (res)
					: "m" (l)
					: "memory" );
			}
		};

		inline bool try_lock() {
				unsigned char res;
				__asm__ __volatile__(
						"xchgb %b0,%1"
						: "=q" (res), "=m"(l)
						: "0" (1)
						: );
				return !(res);
		};

		inline void unlock() {
			 __asm__ __volatile__(
							"movb $0,%0"
							: "=m" (l)
							:
							: "memory" );
		};
};


// -------------------------------------------
// TEST AND TEST AND SET
// -------------------------------------------
class TATAS_Lock
{
	private:
		volatile char l;

	public:

		TATAS_Lock() { l = 0;};
		~TATAS_Lock() {};

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
				unsigned char res;
				__asm__ __volatile__(
						"xchgb %b0,%1"
						: "=q" (res), "=m"(l)
						: "0" (1)
						: );
				return !(res);
		};

		inline void unlock() {
			 __asm__ __volatile__(
							"movb $0,%0"
							: "=m" (l)
							:
							: "memory" );
		};
};


// -------------------------------------------
// OMP Lock
// -------------------------------------------
class OMP_Lock
{
	private: 
		omp_lock_t l;
	
	public:
		OMP_Lock() { omp_init_lock(&l); };
		~OMP_Lock() { omp_destroy_lock(&l); };

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

typedef TAS_EXP_Lock lock_t;


#endif
