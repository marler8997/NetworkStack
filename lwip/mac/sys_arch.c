#include <sys/time.h>
#include <lwip/err.h>
#include <lwip/sys.h>
#include <arch/sys_arch.h>
#include <mach/mach_error.h>
#include <mach/task.h>
#include <mach/mach_init.h>

static struct timeval sys_init_time;

void sys_init(void)
{
  gettimeofday(&sys_init_time, NULL);
}

/*
  This optional function returns the current time in milliseconds (don't care
  for wraparound, this is only used for time diffs).
  Not implementing this function means you cannot use some modules (e.g. TCP
  timestamps, internal timeouts for NO_SYS==1).
*/
u32_t sys_now(void)
{
  struct timeval tv;
  gettimeofday(&tv, NULL);

  return (u32_t)(tv.tv_sec - sys_init_time.tv_sec) * 1000 +
    (u32_t)(tv.tv_usec - sys_init_time.tv_usec) / 1000;
}

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
  kern_return_t result;

#ifdef KERNEL_MODE
  sem->task = current_task();
#else
  sem->task = mach_task_self();
#endif

  result = semaphore_create(sem->task, &sem->sem, SYNC_POLICY_FIFO, count);
  if(result != KERN_SUCCESS) {
    LWIP_PLATFORM_DIAG(("semaphore_create failed <%d,%s>\n", result, mach_error_string(result)));
    return ERR_UNKNOWN;
  }
  return ERR_OK;
}

/*
  Deallocates a semaphore.
*/
void sys_sem_free(sys_sem_t *sem)
{
  kern_return_t result;
  result = semaphore_destroy(sem->task, sem->sem);
  if(result != KERN_SUCCESS) {
    LWIP_PLATFORM_DIAG(("semaphore_destroy failed <%d,%s>\n", result, mach_error_string(result)));
    LWIP_PLATFORM_ASSERT("semaphore_destory failed");
  }
}

/*
  Signals a semaphore.
*/
void sys_sem_signal(sys_sem_t *sem)
{
  kern_return_t result;
  result = semaphore_signal(sem->sem);
  if(result != KERN_SUCCESS) {
    LWIP_PLATFORM_DIAG(("semaphore_signal failed <%d,%s>\n", result, mach_error_string(result)));
    LWIP_PLATFORM_ASSERT("semaphore_signal failed");
  }
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
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeoutMillis)
{
  kern_return_t result;
  u32_t start;

  if(timeoutMillis == 0) {

    start = sys_now();
    result = semaphore_wait(sem->sem);
    if(result == KERN_SUCCESS) {
      return sys_now() - start;
    }

    LWIP_PLATFORM_DIAG(("semaphore_wait failed <%d,%s>\n", result, mach_error_string(result)));
    LWIP_PLATFORM_ASSERT("semaphore_wait failed");

  } else {
    mach_timespec_t ts;

    ts.tv_sec = timeoutMillis / 1000;
    ts.tv_nsec = (timeoutMillis % 1000) * 1000000;

    start = sys_now();
    result = semaphore_timedwait(sem->sem, ts);
    if(result == KERN_SUCCESS) {
      return sys_now() - start;
    }
    if(result == KERN_OPERATION_TIMED_OUT) {
      return SYS_ARCH_TIMEOUT;
    }

    LWIP_PLATFORM_DIAG(("semaphore_timedwait failed <%d,%s>\n", result, mach_error_string(result)));
    LWIP_PLATFORM_ASSERT("semaphore_timedwait failed");
  }
}

/*
  Returns 1 if the semaphore is valid, 0 if it is not valid.
  When using pointers, a simple way is to check the pointer for != NULL.
  When directly using OS structures, implementing this may be more complex.
  This may also be a define, in which case the function is not prototyped.
*/
int sys_sem_valid(sys_sem_t *sem)
{
  LWIP_PLATFORM_ASSERT("sys_sem_valid not implemented");
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
  LWIP_PLATFORM_ASSERT("sys_sem_set_invalid not implemented");
}


err_t sys_mutex_new(sys_mutex_t *mutex)
{
  LWIP_PLATFORM_ASSERT("sys_mutex_new not implemented");
  return ERR_MEM; // not implemented
}
void sys_mutex_lock(sys_mutex_t *mutex)
{
  LWIP_PLATFORM_ASSERT("sys_mutex_lock not implemented");
}
void sys_mutex_unlock(sys_mutex_t *mutex)
{
  LWIP_PLATFORM_ASSERT("sys_mutex_unlock not implemented");
}



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
  LWIP_PLATFORM_ASSERT("sys_mbox_new not implemented");
  return ERR_MEM; // not implemented
}


/*
  Deallocates a mailbox. If there are messages still present in the
  mailbox when the mailbox is deallocated, it is an indication of a
  programming error in lwIP and the developer should be notified.
*/
void sys_mbox_free(sys_mbox_t *mbox)
{
  LWIP_PLATFORM_ASSERT("sys_mbox_free not implemented");
}


/*
  Posts the "msg" to the mailbox. This function have to block until
  the "msg" is really posted.
*/
void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
  LWIP_PLATFORM_ASSERT("sys_mbox_post not implemented");
}


/*
  Try to post the "msg" to the mailbox. Returns ERR_MEM if this one
  is full, else, ERR_OK if the "msg" is posted.
*/
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
  LWIP_PLATFORM_ASSERT("sys_mbox_trypost not implemented");
  return ERR_MEM; // not implemented
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
  LWIP_PLATFORM_ASSERT("sys_mbox_fetch not implemented");
  return 0;
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
  LWIP_PLATFORM_ASSERT("sys_mbox_tryfetch not implemented");
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
  LWIP_PLATFORM_ASSERT("sys_mbox_valid not implemented");
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
  LWIP_PLATFORM_ASSERT("sys_mbox_set_invalid not implemented");
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
  LWIP_PLATFORM_ASSERT("sys_thread_new not implemented");
  return 0;
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
  LWIP_PLATFORM_ASSERT("sys_arch_protect not implemented");
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
  LWIP_PLATFORM_ASSERT("sys_arch_unprotect not implemented");
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
