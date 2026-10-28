#include "kernel/types.h"
#include "user/user.h"
#include "user/uthread.h"

void thread_a(void) {
  for (int i = 0; i < 5; i++) {
    printf("Hebra A: iteracion %d\n", i);
    thread_yield();
  }
  printf("Hebra A: terminada\n");
  thread_exit();
}

void thread_b(void) {
  for (int i = 0; i < 5; i++) {
    printf("Hebra B: iteracion %d\n", i);
    thread_yield();
  }
  printf("Hebra B: terminada\n");
  thread_exit();
}

void thread_c(void) {
  for (int i = 0; i < 5; i++) {
    printf("Hebra C: iteracion %d\n", i);
    thread_yield();
  }
  printf("Hebra C: terminada\n");
  thread_exit();
}

int main(int argc, char *argv[]) {
  printf("Iniciando prueba de hebras de usuario...\n");

  thread_init();

  thread_create(thread_a);
  thread_create(thread_b);
  thread_create(thread_c);

  // El main cede el turno para que comience la rotación de las 3 hebras
  thread_exit();

  exit(0);
}