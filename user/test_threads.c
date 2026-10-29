#include "kernel/types.h"
#include "user/user.h"
#include "user/uthread.h"

void thread_a(void) {
  for (int i = 0; i < 5; i++) {
    printf("Hebra A: iteracion %d\n", i);
    printf("SO \n");
    thread_yield();
  }
  thread_exit();
}

void thread_b(void) {
  for (int i = 0; i < 5; i++) {
    printf("Hebra B: iteracion %d\n", i);
    printf("es la \n");
    thread_yield();
  }
  thread_exit();
}

void thread_c(void) {
  for (int i = 0; i < 5; i++) {
    printf("Hebra C: iteracion %d\n", i);
    printf("cumbia\n");
    thread_yield();
  }
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