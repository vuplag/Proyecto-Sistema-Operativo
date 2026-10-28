#ifndef _UTHREAD_H_
#define _UTHREAD_H_

#include "kernel/types.h"

#define STACK_SIZE  4096
#define MAX_THREAD  4

// Estados de la hebra (Requerimiento A)
enum thread_state {
  FREE,
  RUNNING,
  RUNNABLE
};

// Registros callee-saved de RISC-V (14 registros = 112 bytes)
struct context {
  uint64 ra;
  uint64 sp;
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
};

// Estructura de la hebra con su pila exclusiva (Requerimiento A)
struct thread {
  char            stack[STACK_SIZE];
  enum thread_state state;
  struct context  context;
};

// Funciones de la biblioteca
void thread_init(void);
void thread_create(void (*func)());
void thread_yield(void);
void thread_schedule(void);
void thread_exit(void);

// Rutina en ensamblador RISC-V
extern void thread_switch(struct context *old_ctx, struct context *new_ctx);

#endif