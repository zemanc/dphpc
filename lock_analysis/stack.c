#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

// helper function: difference between timestamps obtained by clock_gettime()
// the return value is in nanoseconds

double timediff(struct timespec* start, struct timespec* stop) {
    double stoptime = stop->tv_nsec + stop->tv_sec * 1e9;
    double starttime = start->tv_nsec + start->tv_sec * 1e9;
    return stoptime - starttime;
}

// the different locks...

void lock_tas_asm(volatile int* lock) {

    register unsigned char _res = 1;
    while (_res != 0) {
     __asm__ __volatile__(
        "xchg %0,%1"
       :"+q" (_res), "+m" (*lock)
       : 
       : );
    }

}

void lock_tatas_asm(volatile int* lock) {

    register unsigned char _res = 1;
    while (*lock == 1) { }
    while (_res != 0) {
         __asm__ __volatile__(
        "xchg %0,%1"
        :"+q" (_res), "+m" (*lock)
        : 
        : );
    }

}

void lock_tas_expbo_asm(volatile int* lock) {

    int time = 1;
    register unsigned char _res = 1;
    while (_res != 0) {
         __asm__ __volatile__(
        "xchg %0,%1"
        :"+q" (_res), "+m" (*lock)
        : 
        : "memory");
        if (_res == 1) {
            time *= 2;
            for (int i=0; i<time; i++) {
                __asm__ __volatile__("nop");
            }
        }
    }

}

void unlock_tas_asm(volatile int* lock) {
    __asm__ __volatile__(
        "mfence" 
       :
       :
       :);
        *lock = 0;
}

void unlock_tatas_asm(volatile int* lock) {
    __asm__ __volatile__(
        "mfence" 
       :
       :
       : );
    *lock = 0;
}

void unlock_tas_expbo_asm(volatile int* lock) {
    __asm__ __volatile__(
        "mfence" 
       :
       :
       : );
    *lock = 0;
}

// the stack benchmarks


typedef struct {
    int* data;
    unsigned long int head;
    unsigned long int space;
    void (*lock)();
    void (*unlock)();
    volatile int* lockval;
} stack_t;

typedef struct {
    int tid;
    int ops;
    stack_t* stack;
} thread_args_t; 

void push(stack_t* stack, int elem) {
    stack->lock(stack->lockval);
    if (stack->head >= stack->space) {
        stack->space = stack->space * 2; 
        stack->data = realloc(stack->data, stack->space * sizeof(int));
        assert(stack->data != NULL);
    }
    stack->data[stack->head] = elem;
    stack->head++;
    stack->unlock(stack->lockval);
}

int pop(stack_t* stack, int* val) {
    stack->lock(stack->lockval);
    if (stack->head == 0) {
        stack->unlock(stack->lockval);
        return -1;
    }
    *val = stack->data[stack->head-1];
    stack->head--;
    stack->unlock(stack->lockval);
    return 0;
}

void* bench_stack(void* args) {

    thread_args_t* ta = args;
    int val;

    for (int i=0; i<ta->ops; i++) {
        push(ta->stack, i);
        pop(ta->stack, &val);
    }

    pthread_exit(NULL);

}

void init_stack(stack_t* stack, void (*lockfunc)(), void (*unlockfunc)()) {

    stack->space = 1024;
    stack->data = malloc(sizeof(int) * stack->space);
    assert(stack->data != NULL);
    stack->head = 0;
    stack->lockval = malloc(sizeof(int));
    assert(stack->lockval != NULL);
    *(stack->lockval) = 0;
    stack->lock = lockfunc;
    stack->unlock = unlockfunc;

}

void stack_finalize(stack_t* stack) {

    stack->space = 0;
    stack->head = 0;
    free(stack->data);
    free((void*)(stack->lockval));

}

void perform_test(void (*lock_func)(), void (*unlock_func)(), char* lockname,
                  int ops_per_thread, int num_runs, int max_threads) {

    static int firstprint = 1; // so that we know when to print header

    for (int num_threads=1; num_threads<=max_threads; num_threads++) {

      for (int run=0; run<num_runs; run++) {
    
        // alocate space for thread management datastructure
        pthread_t* threads = malloc(num_threads*sizeof(pthread_t));
        assert(threads != NULL);

        // allocate space for thread arguments
        thread_args_t* thread_args = malloc(num_threads*sizeof(thread_args_t));
        assert(thread_args != NULL);

        // threads should be joinable
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
 
        stack_t stack;
        init_stack(&stack, lock_func, unlock_func); 

        struct timespec time1, time2;
        clock_gettime(CLOCK_REALTIME, &time1);

        // init thread args
        for (int t=0; t<num_threads; t++) {
            thread_args[t].tid = t;
            thread_args[t].stack = &stack;
            thread_args[t].ops = ops_per_thread;
        }

        // create num_thread threds
        for(int t=0; t<num_threads; t++){
            int rc = pthread_create(&threads[t], &attr, bench_stack, 
                                (void *) &thread_args[t]);
            assert(rc == 0);
        }

        // wait for threads to complete
        pthread_attr_destroy(&attr);
        for(int t=0; t<num_threads; t++) {
            int rc = pthread_join(threads[t], NULL);
            assert(rc == 0);
        }

        clock_gettime(CLOCK_REALTIME, &time2);

        stack_finalize(&stack);

        double time_per_op = timediff(&time1, &time2) / (ops_per_thread*2);
        if (firstprint) {
            firstprint = 0;
            printf("%10s %20s %20s\n", "threads", "lock", "time");
        }
        printf("%10i %20s %20.6lf\n", num_threads, lockname, time_per_op);

        //finalize_func(thread_args, num_threads);
        free(threads);
        free(thread_args);
      }
    }

}


int main(int argc, char** argv) {

    perform_test(lock_tas_asm, unlock_tas_asm, "TAS", 100000, 5, 8);
    perform_test(lock_tatas_asm, unlock_tatas_asm, "TATAS", 100000, 5, 8);
    perform_test(lock_tas_expbo_asm, unlock_tas_expbo_asm, "TAS_EXPBO", 100000, 5, 8);

    pthread_exit(NULL);

}

