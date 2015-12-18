#ifndef _SYSARCH_H_
#define _SYSARCH_H_

// NOTE: we cannot include <windows.h> because
//       it has conflicts with lwip

// Note: windef.h requires either _X86_ or _AMD64_ to be defined

#define _X86_

#include <windef.h>
#include <WinBase.h>

#include <lwip/err.h>

/*  Arch dependent types for the following objects:
    sys_sem_t, sys_mbox_t, sys_thread_t,
  And, optionally:
    sys_prot_t

  Defines to set vars of sys_mbox_t and sys_sem_t to NULL.
    SYS_MBOX_NULL NULL
    SYS_SEM_NULL NULL
*/


/*
typedef SRWLOCK sys_sem_t;
#define sys_mem_valid(sem) (*(sem) != NULL)
#define sys_sem_set_invalid(sem) memset(sem, 0, sizeof(SRWLOCK))
*/
typedef HANDLE sys_sem_t;

typedef SRWLOCK sys_mutex_t;
typedef HANDLE sys_thread_t;
typedef void* sys_prot_t;



//
// The thread_gate extension
//
typedef HANDLE sys_thread_gate_t;
void sys_thread_gate_new(sys_thread_gate_t* gate, u8_t manualClose, u8_t initialIsOpen);
err_t sys_thread_gate_trynew(sys_thread_gate_t* gate, u8_t manualClose, u8_t initialIsOpen);
void sys_thread_gate_free(sys_thread_gate_t* gate);
void sys_thread_gate_open(sys_thread_gate_t* gate);
void sys_thread_gate_close(sys_thread_gate_t* gate);
void sys_thread_gate_wait(sys_thread_gate_t* gate);
// returns 0 if gate was opened, 1 if it wasn't
u8_t sys_thread_gate_wait_until(sys_thread_gate_t* gate, u32_t timeout);




typedef struct {
	CRITICAL_SECTION critical_section;
	sys_thread_gate_t thread_gate;
	void** queue;
	void** limit;
	void** front;
	void** back;
} sys_mbox_t;



#endif
