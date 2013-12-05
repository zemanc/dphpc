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

		void lock();
		bool try_lock();
		void unlock();
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

		void lock();
		bool try_lock();
		void unlock();
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

		void lock();
		bool try_lock();
		void unlock();
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

		void lock();
		bool try_lock();
		void unlock();
};

// typedef OMP_Lock lock_t;
typedef TAS_EXP_Lock lock_t;


#endif
