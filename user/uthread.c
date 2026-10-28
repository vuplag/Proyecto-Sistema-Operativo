#include "kernel/types.h"
#include "user/user.h"
#include "user/uthread.h"

static struct thread all_threads[MAX_THREAD];
struct thread *current_thread = 0;

void thread_init(void) {
  for (int i = 0; i < MAX_THREAD; i++) {
    all_threads[i].state = FREE;
    memset(&all_threads[i].context, 0, sizeof(all_threads[i].context));
  }
  // El slot 0 representa el contexto del hilo principal (main)
  current_thread = &all_threads[0];
  current_thread->state = RUNNING;
}

void thread_create(void (*func)()) {
  struct thread *t = 0;

  for (int i = 0; i < MAX_THREAD; i++) {
    if (all_threads[i].state == FREE) {
      t = &all_threads[i];
      break;
    }
  }

  if (t == 0) {
    printf("thread_create: no hay slots disponibles\n");
    return;
  }

  memset(&t->context, 0, sizeof(t->context));

  // ra apunta a la rutina asignada
  t->context.ra = (uint64)func;

  // En RISC-V la pila crece hacia abajo. Alineacion a 16 bytes.
  uint64 stack_top = (uint64)(t->stack + STACK_SIZE - 128);
  stack_top = stack_top & ~((uint64)0xF);

  t->context.sp = stack_top;
  t->context.s0 = stack_top;

  t->state = RUNNABLE;
}

void thread_yield(void) {
  if (current_thread != 0) {
    current_thread->state = RUNNABLE;
  }
  thread_schedule();
}

void thread_exit(void) {
  if (current_thread != 0) {
    current_thread->state = FREE;
  }
  thread_schedule();
}

void thread_schedule(void) {
  struct thread *t_next = 0;
  int current_idx = current_thread ? (int)(current_thread - all_threads) : 0;

  // Planificación cooperativa Round-Robin
  for (int i = 1; i <= MAX_THREAD; i++) {
    int idx = (current_idx + i) % MAX_THREAD;
    if (all_threads[idx].state == RUNNABLE) {
      t_next = &all_threads[idx];
      break;
    }
  }

  if (t_next == 0) {
    // Si no quedan hebras activas, finaliza el proceso de prueba
    exit(0);
  }

  if (t_next != current_thread) {
    struct thread *t_prev = current_thread;
    current_thread = t_next;
    current_thread->state = RUNNING;

    thread_switch(&t_prev->context, &t_next->context);
  }
}