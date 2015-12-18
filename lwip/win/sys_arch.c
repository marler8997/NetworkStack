#include <arch/sys_arch.h>
#include <lwip/err.h>
#include <lwip/sys.h>

#include <windows.h>


static LARGE_INTEGER frequency;
static LARGE_INTEGER sys_init_time;

static CRITICAL_SECTION sys_critical_section;

void sys_init(void)
{
	// TODO: seed the random number generator
	//srand(time(0));
	if (QueryPerformanceFrequency(&frequency) == 0)
	{
		LWIP_PLATFORM_DIAG(("QueryPerformanceFrequency failed (e=%d)", GetLastError()));
		LWIP_PLATFORM_ASSERT("QueryPerformanceFrequency");
	}
	if (QueryPerformanceCounter(&sys_init_time) == 0)
	{
		LWIP_PLATFORM_DIAG(("QueryPerformanceCounter failed (e=%d)", GetLastError()));
		LWIP_PLATFORM_ASSERT("QueryPerformanceCounter");
	}
	InitializeCriticalSection(&sys_critical_section);
}

/*
This optional function returns the current time in milliseconds (don't care
for wraparound, this is only used for time diffs).
Not implementing this function means you cannot use some modules (e.g. TCP
timestamps, internal timeouts for NO_SYS==1).
*/
u32_t sys_now(void)
{
	LARGE_INTEGER now;
	if (QueryPerformanceCounter(&now) == 0)
	{
		LWIP_PLATFORM_DIAG(("QueryPerformanceCounter failed (e=%d)", GetLastError()));
		LWIP_PLATFORM_ASSERT("QueryPerformanceCounter");
	}
	//return (now.QuadPart * 1000 / frequency.QuadPart) - sys_init_time;
	return (u32_t)((now.QuadPart - sys_init_time.QuadPart) * 1000 / frequency.QuadPart);
}




#if SEMAPHIRE_USING_SRWLOCK
/*
Creates a new semaphore. The semaphore is allocated to the memory that 'sem'
points to (which can be both a pointer or the actual OS structure).
The "count" argument specifies the initial state of the semaphore (which is
either 0 or 1).
If the semaphore has been created, ERR_OK should be returned. Returning any
other error will provide a hint what went wrong, but except for assertions,
no real error handling is implemented.
*/
err_t sys_sem_new(sys_sem_t *sem, u8_t initial_lock)
{
	InitializeSRWLock(sem);
	if (initial_lock)
	{
		AcquireSRWLockExclusive(sem);
	}
	return ERR_OK;
}
/*
Deallocates a semaphore.
*/
void sys_sem_free(sys_sem_t *sem)
{
	// Don't need to destroy an srwlock
}
/*
Signals a semaphore.
*/
void sys_sem_signal(sys_sem_t *sem)
{

}
/*
Blocks the thread while waiting for the semaphore to be
signaled. If the "timeout" argument is non-zero, the thread should
only be blocked for the specified time (measured in
milliseconds). If the "timeout" argument is zero, the thread should be
blocked until the semaphore is signalled.

If the timeout argument is non-zero, the return value is the number of
milliseconds spent waiting for the semaphore to be signaled. If the
semaphore wasn't signaled within the specified time, the return value is
SYS_ARCH_TIMEOUT. If the thread didn't have to wait for the semaphore
(i.e., it was already signaled), the function may return zero.

Notice that lwIP implements a function with a similar name,
sys_sem_wait(), that uses the sys_arch_sem_wait() function.
*/
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
	AcquireSRWLockExclusive(sem);
	//LWIP_PLATFORM_ASSERT("___ not implemented");
	return 0;
}
#endif

/*
  Creates a new semaphore. The semaphore is allocated to the memory that 'sem'
  points to (which can be both a pointer or the actual OS structure).
  The "count" argument specifies the initial state of the semaphore (which is
  either 0 or 1).
  If the semaphore has been created, ERR_OK should be returned. Returning any
  other error will provide a hint what went wrong, but except for assertions,
  no real error handling is implemented.
*/
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
	*sem = CreateSemaphore(NULL, count, 1, NULL);
	if (*sem == NULL)
	{
		LWIP_PLATFORM_DIAG(("CreateSemaphore failed (e=%d)", GetLastError()));
		LWIP_PLATFORM_ASSERT("CreateSemaphore");
	}
	return ERR_OK;
}

/*
  Deallocates a semaphore.
*/
void sys_sem_free(sys_sem_t *sem)
{
	CloseHandle(*sem);
	*sem = NULL;
}

/*
  Signals a semaphore.
*/
void sys_sem_signal(sys_sem_t *sem)
{
	LWIP_PLATFORM_ASSERT("___ not implemented");
}

/*
  Blocks the thread while waiting for the semaphore to be
  signaled. If the "timeout" argument is non-zero, the thread should
  only be blocked for the specified time (measured in
  milliseconds). If the "timeout" argument is zero, the thread should be
  blocked until the semaphore is signalled.

  If the timeout argument is non-zero, the return value is the number of
  milliseconds spent waiting for the semaphore to be signaled. If the
  semaphore wasn't signaled within the specified time, the return value is
  SYS_ARCH_TIMEOUT. If the thread didn't have to wait for the semaphore
  (i.e., it was already signaled), the function may return zero.

  Notice that lwIP implements a function with a similar name,
  sys_sem_wait(), that uses the sys_arch_sem_wait() function.
*/
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
	LWIP_PLATFORM_ASSERT("___ not implemented");
	return 0;
}

/*
  Returns 1 if the semaphore is valid, 0 if it is not valid.
  When using pointers, a simple way is to check the pointer for != NULL.
  When directly using OS structures, implementing this may be more complex.
  This may also be a define, in which case the function is not prototyped.
*/
int sys_sem_valid(sys_sem_t *sem)
{
	LWIP_PLATFORM_ASSERT("___ not implemented");
	return 0;
}

/*
  Invalidate a semaphore so that sys_sem_valid() returns 0.
  ATTENTION: This does NOT mean that the semaphore shall be deallocated:
  sys_sem_free() is always called before calling this function!
  This may also be a define, in which case the function is not prototyped.
*/
void sys_sem_set_invalid(sys_sem_t *sem)
{
	LWIP_PLATFORM_ASSERT("___ not implemented");
}

err_t sys_mutex_new(sys_mutex_t *mutex)
{
	//LWIP_PLATFORM_DIAG(("sys_mutex_new: called\n"));
	InitializeSRWLock(mutex);
	return ERR_OK;
}
void sys_mutex_lock(sys_mutex_t *mutex)
{
	//LWIP_PLATFORM_DIAG(("sys_mutex_lock: called\n"));
	AcquireSRWLockExclusive(mutex);
}
void sys_mutex_unlock(sys_mutex_t *mutex)
{
	//LWIP_PLATFORM_DIAG(("sys_mutex_unlock: called\n"));
	ReleaseSRWLockExclusive(mutex);
}



//
// The thread_gate extension
//
void sys_thread_gate_new(sys_thread_gate_t* gate, u8_t manualClose, u8_t initialIsOpen)
{
	*gate = CreateEvent(0, manualClose, initialIsOpen, NULL);
	if (*gate == NULL)
	{
		LWIP_PLATFORM_DIAG(("CreateEvent failed (e=%d)", GetLastError()));
		LWIP_PLATFORM_ASSERT("CreateEvent");
	}
}
err_t sys_thread_gate_trynew(sys_thread_gate_t* gate, u8_t manualClose, u8_t initialIsOpen)
{
	*gate = CreateEvent(0, manualClose, initialIsOpen, NULL);
	if (*gate == NULL)
	{
		return ERR_UNKNOWN;
	}
	return ERR_OK;
}
void sys_thread_gate_free(sys_thread_gate_t* gate)
{
	CloseHandle(*gate);
	*gate = NULL; // Should I do this?
}
void sys_thread_gate_open(sys_thread_gate_t* gate)
{
	SetEvent(*gate);
}
void sys_thread_gate_close(sys_thread_gate_t* gate)
{
	ResetEvent(*gate);
}
void sys_thread_gate_wait(sys_thread_gate_t* gate)
{
	DWORD result;
	result = WaitForSingleObject(*gate, INFINITE);
	if (result != WAIT_OBJECT_0)
	{
		LWIP_PLATFORM_DIAG(("WaitForSingleObject failed (result=%d, e=%d)", result, GetLastError()));
		LWIP_PLATFORM_ASSERT("WaitForSingleObject");
	}
}
// returns 0 if gate was opened, 1 if it wasn't
u8_t sys_thread_gate_wait_until(sys_thread_gate_t* gate, u32_t timeout)
{
	DWORD result;

	LWIP_ASSERT("timeout cannot be 0", timeout != 0);

	result = WaitForSingleObject(*gate, timeout);
	if (result == WAIT_OBJECT_0)
	{
		return 0;
	}
	if (result == WAIT_TIMEOUT)
	{
		return 1;
	}
	
	LWIP_PLATFORM_DIAG(("WaitForSingleObject failed (result=%d, e=%d)", result, GetLastError()));
	LWIP_PLATFORM_ASSERT("WaitForSingleObject");
}




// mbox
// empty: front==back
// add: back = value
//      back++


/*
  Creates an empty mailbox for maximum "size" elements. Elements stored
  in mailboxes are pointers. You have to define macros "_MBOX_SIZE"
  in your lwipopts.h, or ignore this parameter in your implementation
  and use a default size.
  If the mailbox has been created, ERR_OK should be returned. Returning any
  other error will provide a hint what went wrong, but except for assertions,
  no real error handling is implemented.
*/
err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
	ZeroMemory(mbox, sizeof(sys_mbox_t));

	InitializeCriticalSection(&mbox->critical_section);
	sys_thread_gate_new(&mbox->thread_gate, 1, 1);

	mbox->queue = malloc(sizeof(void*)* size);
	if (mbox->queue == NULL)
	{
		LWIP_PLATFORM_DIAG(("malloc failed in sys_mbox_new (e=%d)", GetLastError()));
		sys_mbox_free(mbox);
		return ERR_MEM;
	}
	mbox->limit = mbox->queue + size;
	mbox->front = mbox->queue;
	mbox->back = mbox->queue;
	return ERR_OK;
}


/*
  Deallocates a mailbox. If there are messages still present in the
  mailbox when the mailbox is deallocated, it is an indication of a
  programming error in lwIP and the developer should be notified.
*/
void sys_mbox_free(sys_mbox_t *mbox)
{
	if (!mbox)
	{
		LWIP_PLATFORM_ASSERT("sys_mbox_free called with null mbox");
	}
	if (mbox->queue)
	{
		free(mbox->queue);
		mbox->queue = NULL;
		mbox->front = NULL; // Will force segfault on reuse
		mbox->back = NULL; // Will force segfault on reuse
	}
	sys_thread_gate_free(&mbox->thread_gate);
	DeleteCriticalSection(&mbox->critical_section);
}

/*
Try to post the "msg" to the mailbox. Returns ERR_MEM if this one
is full, else, ERR_OK if the "msg" is posted.
*/
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
	EnterCriticalSection(&mbox->critical_section);
	void** nextBack = mbox->back + 1;
	if (nextBack >= mbox->limit)
	{
		nextBack = mbox->queue;
	}

	if (nextBack == mbox->front)
	{
		LeaveCriticalSection(&mbox->critical_section);
		return ERR_MEM;
	}

	*mbox->back = msg;
	mbox->back = nextBack;
	LeaveCriticalSection(&mbox->critical_section);
	return ERR_OK;
}



/*
  Posts the "msg" to the mailbox. This function have to block until
  the "msg" is really posted.
*/
void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
	err_t result;

	result = sys_mbox_trypost(mbox, msg);
	if (result == ERR_MEM)
	{
		// I'll probably need to use CreateEvent for this
		LWIP_PLATFORM_ASSERT("sys_mbox_post on full is not implemented");
	}
}



/*
  Blocks the thread until a message arrives in the mailbox, but does
  not block the thread longer than "timeout" milliseconds (similar to
  the sys_arch_sem_wait() function). If "timeout" is 0, the thread should
  be blocked until a message arrives. The "msg" argument is a result
  parameter that is set by the function (i.e., by doing "*msg =
  ptr"). The "msg" parameter maybe NULL to indicate that the message
  should be dropped.

  The return values are the same as for the sys_arch_sem_wait() function:
  Number of milliseconds spent waiting or SYS_ARCH_TIMEOUT if there was a
  timeout.

  Note that a function with a similar name, sys_mbox_fetch(), is
  implemented by lwIP. 
*/
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
	u32_t start = sys_now();

	//
	// Wait for a message
	//
	while (1)
	{
		EnterCriticalSection(&mbox->critical_section);
		if (mbox->front != mbox->back)
		{
			break; // There is a message to get
		}

		sys_thread_gate_close(&mbox->thread_gate);
		LeaveCriticalSection(&mbox->critical_section);

		// TODO: Can sys_now roll?
		if (timeout == 0)
		{
			LWIP_PLATFORM_DIAG(("[SYSARCH-DEBUG] mbox_fetch: waiting forever...\n"));
			sys_thread_gate_wait(&mbox->thread_gate);
		}
		else
		{
			u32_t elapsed = sys_now() - start;
			if (elapsed >= timeout)
			{
				return SYS_ARCH_TIMEOUT;
			}
			LWIP_PLATFORM_DIAG(("[SYSARCH-DEBUG] mbox_fetch: waiting for %d millis...\n", timeout - elapsed));
			if (sys_thread_gate_wait_until(&mbox->thread_gate, timeout - elapsed))
			{
				return SYS_ARCH_TIMEOUT;
			}
		}
	}

	//Note: we are inside the critical section
	if (*msg)
	{
		*msg = *mbox->front;
	}

	mbox->front++;
	if (mbox->front >= mbox->limit)
	{
		mbox->front = mbox->queue;
	}

	LeaveCriticalSection(&mbox->critical_section);
	return ERR_OK;
}


/*
  This is similar to sys_arch_mbox_fetch, however if a message is not
  present in the mailbox, it immediately returns with the code
  SYS_MBOX_EMPTY. On success 0 is returned.

  To allow for efficient implementations, this can be defined as a
  function-like macro in sys_arch.h instead of a normal function. For
  example, a naive implementation could be:
    #define sys_arch_mbox_tryfetch(mbox,msg) \
      sys_arch_mbox_fetch(mbox,msg,1)
  although this would introduce unnecessary delays.
*/
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
	LWIP_PLATFORM_ASSERT("___ not implemented");
	return 0;
}


/*
  Returns 1 if the mailbox is valid, 0 if it is not valid.
  When using pointers, a simple way is to check the pointer for != NULL.
  When directly using OS structures, implementing this may be more complex.
  This may also be a define, in which case the function is not prototyped.
*/
int sys_mbox_valid(sys_mbox_t *mbox)
{
	LWIP_PLATFORM_ASSERT("___ not implemented");
	return 0;
}


/*
  Invalidate a mailbox so that sys_mbox_valid() returns 0.
  ATTENTION: This does NOT mean that the mailbox shall be deallocated:
  sys_mbox_free() is always called before calling this function!
  This may also be a define, in which case the function is not prototyped.

If threads are supported by the underlying operating system and if
such functionality is needed in lwIP, the following function will have
to be implemented as well:
*/
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
	LWIP_PLATFORM_ASSERT("___ not implemented");
}


/*
  Starts a new thread named "name" with priority "prio" that will begin its
  execution in the function "thread()". The "arg" argument will be passed as an
  argument to the thread() function. The stack size to used for this thread is
  the "stacksize" parameter. The id of the new thread is returned. Both the id
  and the priority are system dependent.
*/
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
	// TODO: set the name
	// TODO: set the priority
	sys_thread_t threadHandle = CreateThread(0, stacksize, (LPTHREAD_START_ROUTINE)thread, arg, 0, NULL);
	if (threadHandle == NULL)
	{
		LWIP_PLATFORM_DIAG(("CreateThread failed (e=%d)", GetLastError()));
		LWIP_PLATFORM_ASSERT("CreateThread");
	}
	return threadHandle;
}


/*
  This optional function does a "fast" critical region protection and returns
  the previous protection level. This function is only called during very short
  critical regions. An embedded system which supports ISR-based drivers might
  want to implement this function by disabling interrupts. Task-based systems
  might want to implement this by using a mutex or disabling tasking. This
  function should support recursive calls from the same task or interrupt. In
  other words, sys_arch_protect() could be called while already protected. In
  that case the return value indicates that it is already protected.

  sys_arch_protect() is only required if your port is supporting an operating
  system.
*/
sys_prot_t sys_arch_protect(void)
{
	LWIP_PLATFORM_ASSERT("___ not implemented");
	return 0;
}


/*
  This optional function does a "fast" set of critical region protection to the
  value specified by pval. See the documentation for sys_arch_protect() for
  more information. This function is only required if your port is supporting
  an operating system.
*/
void sys_arch_unprotect(sys_prot_t pval)
{
	LWIP_PLATFORM_ASSERT("___ not implemented");
}


/*
Note:

Be carefull with using mem_malloc() in sys_arch. When malloc() refers to
mem_malloc() you can run into a circular function call problem. In mem.c
mem_init() tries to allcate a semaphore using mem_malloc, which of course
can't be performed when sys_arch uses mem_malloc.

-------------------------------------------------------------------------------
Additional files required for the "OS support" emulation layer:
-------------------------------------------------------------------------------

cc.h       - Architecture environment, some compiler specific, some
             environment specific (probably should move env stuff 
             to sys_arch.h.)

  Typedefs for the types used by lwip -
    u8_t, s8_t, u16_t, s16_t, u32_t, s32_t, mem_ptr_t

  Compiler hints for packing lwip's structures -
    PACK_STRUCT_FIELD(x)
    PACK_STRUCT_STRUCT
    PACK_STRUCT_BEGIN
    PACK_STRUCT_END

  Platform specific diagnostic output -
    LWIP_PLATFORM_DIAG(x)    - non-fatal, print a message.
    LWIP_PLATFORM_ASSERT(x)  - fatal, print message and abandon execution.
    Portability defines for printf formatters:
    U16_F, S16_F, X16_F, U32_F, S32_F, X32_F, SZT_F

  "lightweight" synchronization mechanisms -
    SYS_ARCH_DECL_PROTECT(x) - declare a protection state variable.
    SYS_ARCH_PROTECT(x)      - enter protection mode.
    SYS_ARCH_UNPROTECT(x)    - leave protection mode.

  If the compiler does not provide memset() this file must include a
  definition of it, or include a file which defines it.

  This file must either include a system-local <errno.h> which defines
  the standard *nix error codes, or it should #define LWIP_PROVIDE_ERRNO
  to make lwip/arch.h define the codes which are used throughout.


perf.h     - Architecture specific performance measurement.
  Measurement calls made throughout lwip, these can be defined to nothing.
    PERF_START               - start measuring something.
    PERF_STOP(x)             - stop measuring something, and record the result.

sys_arch.h - Tied to sys_arch.c

  Arch dependent types for the following objects:
    sys_sem_t, sys_mbox_t, sys_thread_t,
  And, optionally:
    sys_prot_t

  Defines to set vars of sys_mbox_t and sys_sem_t to NULL.
    SYS_MBOX_NULL NULL
    SYS_SEM_NULL NULL
*/


