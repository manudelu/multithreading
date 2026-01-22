# Concurrency

* A *Critical Section* piece of code that accesses a shared resource, usually a variable or data structure.
* A *Race Condition* arises if multiple threads of execution enter the critical section at roughly the same time; both attempt to update the shared data structure, leading to surprising (and perhaps undesirable) outcome.
* An *Indeterminate Program* consists of one or more race conditions; the output of the program varies from run to run, depending on which threads ran when. The outcome is thus not deterministic, something we usually expect from computer systems.
* To avoid these problems, threads should use some kind of *mutual exclusion* primitives; doing so guarantees that only a single thread ever enters a critical section, thus avoiding races, and rsulting in deterministic program outputs.

*Note:* Each thread has its own stack! Thus, if you have a locally-allocated variable inside of some function a thread is executing, it is essentially private to that thread; no other thread can (easily) access it. To share data between threads, the values must be in the heap or otherwise some locale that is globally accessible.

## Thread API

### Thread Creation

```c
#include <pthread.h>
int pthread_create(pthread_t            *thread,
                   const pthread_attr_t *attr,
                   void                 *(*start_routine)(void*),
                   void                 *arg
);
```

* *thread* is a pointer structure of type *pthread_t*; we'll use this structure to interact with this thread, and thus we need to pass it to *pthread_create()* in order to initialize it.
* *attr* is uses to specify any attributes this thread might have. E.g. information about the scheduling priority of the thread. An attribute is initialized with a separate call to *pthread_attr_init()*.
* The third argument is a *function pointer* that expects a function name which is passed a a single argument of type *void* * and which returns a value of type *void* * (*void pointer*).
* *arg* is the argument to be passed to the function where the thread begins execution.

## Thread Completion

```c
int pthread_join(pthread_t thread, void **value_ptr);
```

* *thread* is used to specify which thread to wait for.
* *value_ptr* is a pointer to the return value you expect to get back.

### Examples:

```c
#include <stdio.h>
#include <cstdlib>
#include <pthread.h>

typedef struct { int a; int b; } myarg_t;
typedef struct { int x; int y; } myret_t;

void *mythread(void *arg) {
    myret_t *rvals = new myret_t;
    rvals->x = 1;
    rvals->y = 2;
    return (void *) rvals;
}

int main(int argc, char *argv[]) {
    pthread_t p;
    myret_t *rvals;
    myarg_t args = { 10, 20 };
    if (pthread_create(&p, NULL, mythread, &args) != 0)
        exit(EXIT_FAILURE);
    if (pthread_join(p, (void **) &rvals) != 0)
        exit(EXIT_FAILURE);
    printf("returned %d %d\n", rvals->x, rvals->y);
    delete rvals;
    return 0;
}
```

```c
#include <stdio.h>
#include <cstdlib>
#include <pthread.h>

void *mythread(void *arg) {
    long long int value = (long long int) arg;
    printf("%lld\n", value);
    return (void *) (value + 1);
}

int main(int argc, char *argv[]) {
    pthread_t p;
    long long int rvalue;
    if (pthread_create(&p, NULL, mythread, (void *) 100) != 0)
        exit(EXIT_FAILURE);
    if (pthread_join(p, (void **) &rvalue) != 0)
        exit(EXIT_FAILURE);
    printf("returned %lld\n", rvalue);
    return 0;
}
```

## Locks

One of the most useful set of functions are those for providing mutual exclusion to a critical section via *locks*. 

```c
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
```

There are two ways to initialize locks. One way to do this is to use *PTHREAD_MUTEX_INITIALIZER*, as follows:

```c
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
```

Doing so sets the lock to the default values and thus makes the lock usable. The dynamic way to do it is to make a call to *pthread_mutex_init()*, as follows:

```c
int rc = pthread_mutex_init(&lock, NULL);
```

The first address of this routine is the address of the lock itself, whereas the second is an optional set of attributes. Note that a corresponding call to:

```c
pthread_mutex_destroy(&lock);
```

should also be made, when you are done with the lock.


The lock and unlock routines are not the only routines within the pthreads library to interact with locks. Two other routines of interest: 

```c
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_timedlock(pthread_mutex_t *mutex, struct timespec *abs_timeout);
```

These two calls are used to lock in acquisition. The *trylock* version returns failure if the lock is already held; the *timedlock* version of acquiring a lock returns after a timeout or after acquiring the lock, whichever happens first. Thus, the timedlock with a timeout of zero degenerates to the trylock case. Both of these versions should generally be avoided; however, there are a few cases where avoiding getting stuck (perhaps indefinitely) in a lock acquisition routine can be useful.

### Examples

```c
#include <stdio.h>
#include <cstdlib>
#include <pthread.h>

static volatile int counter = 0;
pthread_mutex_t g_num_mutex;

void *mythread(void *arg) {
    printf("%s: begin\n", (char *) arg);
    int i;
    for (i = 0; i < 1e7; i++) {
        pthread_mutex_lock(&g_num_mutex);
        counter = counter + 1;  // Critical Section
        pthread_mutex_unlock(&g_num_mutex);
    }
    printf("%s: done\n", (char *) arg);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t p1, p2;
    if (pthread_mutex_init(&g_num_mutex, NULL) != 0)
        exit(EXIT_FAILURE);

    printf("main: begin (counter = %d)\n", counter);

    if (pthread_create(&p1, NULL, mythread, (void*)"A") != 0)
        exit(EXIT_FAILURE);
    if (pthread_create(&p2, NULL, mythread, (void*)"B") != 0)
        exit(EXIT_FAILURE);

    if (pthread_join(p1, NULL) != 0)
        exit(EXIT_FAILURE);
    if (pthread_join(p2, NULL) != 0)
        exit(EXIT_FAILURE);

    printf("main: done with both (counter = %d)\n", counter);
    pthread_mutex_destroy(&g_num_mutex);

    return 0;
}
```

The intent of the code is as follows: if no other thread holds the lock when *pthread_mutex_lock()* is called, the thread will acquire the lock and enter the critical section. If another thread does hold the lock, the thread trying to grab the lock will not return from the call until it has acquired the lock. Of course, many threads may be stuck waiting inside the lock acquisition function at a given time; only the thread with the lock acquired, however, should call unlock. If you try to run the code without the lock it is not guaranteed to obtain the correct result due to race conditions.

## Condition Variables

The other major component of any threads library is the presence of a condition variable. Condition variables are useful when some kind of signaling must take place between threads, if one thread is waiting for another to do something before it can continue. Two primary routines are used by programs wishing to interact in this way:

```c
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_signal(pthread_cond_t *cond);
```

To use a condition variable, one has to in addition have a lock that is associated with this condition. When calling either of the above routines, this lock should be held.

The first routine, *pthread_cond_wait()*, puts the calling thread to sleep, and thus waits for some other thread to signal it, usually when something in the program has changed that the now-sleeping thread might care about. A typical usage looks like this:

```c
/*Note that you can use *pthread_cond_init()* and *pthread_cond_destroy()*, instead of *PTHREAD_COND_INITIALIZER*.*/
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_lock(&lock);
while (ready == 0)
    pthread_cond_wait(&cond, &lock);
pthread_mutex_unlock(&lock);
```

In this code, after initialization of the relevant lock and condition, a thread checks to see if the variable *ready* has yet been set to something other than zero. If not, the thread simply calls the wait routine in order to sleep until some other thread wakes it.

The code to wake a thread, which would run in some other thread, looks like this:

```c
pthread_mutex_lock(&lock);
ready = 1;
pthread_cond_signal(&cond);
pthread_mutex_unlock(&lock);
```

A few things to note about this code sequence. First, when signaling (as well as when modifying the global variable ready), we always make sure to have the lock held. This ensures that we don’t accidentally introduce a race condition into our code. Second, the wait call takes a lock as its second parameter, whereas the signal call only takes a condition. The reason for this difference is that the wait call, in addition to putting the calling thread to sleep, releases the lock when putting said caller to sleep. However, before returning after being woken, the *pthread_cond_wait()* re-acquires the lock, thus ensuring that any time the waiting thread is running between the lock acquire at the beginning of the wait sequence, and the lock release at the end, it holds the lock.
