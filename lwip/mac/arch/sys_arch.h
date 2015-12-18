#ifndef _SYSARCH_H_
#define _SYSARCH_H_

#include <mach/semaphore.h>

/*  Arch dependent types for the following objects:
    sys_sem_t, sys_mbox_t, sys_thread_t,
  And, optionally:
    sys_prot_t

  Defines to set vars of sys_mbox_t and sys_sem_t to NULL.
    SYS_MBOX_NULL NULL
    SYS_SEM_NULL NULL
*/
typedef struct {
  mach_port_t task;
  semaphore_t sem;
} sys_sem_t;

typedef void* sys_mutex_t;
typedef void* sys_mbox_t;
typedef void* sys_thread_t;
typedef void* sys_prot_t;

#endif
