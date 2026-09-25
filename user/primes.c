#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// limites de la tarea
#define NUMERO_INICIAL 2
#define NUMERO_FINAL 35

// Función que filtra los números primos a través de la cadena de procesos
void seleccionar_primos(int tuberia_entrada[2]) {
  while (1) {
    // Cerramos el extremo de escritura, solo vamos a leer de la tubería izquierda
    close(tuberia_entrada[1]);

    int numero_primo;
    // Leemos desde el extremo de lectura el primer número disponible
    int bytes_leidos = read(tuberia_entrada[0], &numero_primo, sizeof(numero_primo));

    // Si no hay más datos o la tubería se cerró, finalizamos este proceso
    if (bytes_leidos <= 0) {
      close(tuberia_entrada[0]);
      exit(0);
    }

    // El primer número que recibe este proceso siempre es primo
    printf("prime %d\n", numero_primo);

    // Nueva tubería hacia el siguiente proceso hijo
    int tuberia_hacia_siguiente[2];
    pipe(tuberia_hacia_siguiente);

    int id_proceso_hijo = fork();
    if (id_proceso_hijo < 0) {
      printf("primes: error al crear al hijo\n");
      close(tuberia_entrada[0]);
      close(tuberia_hacia_siguiente[0]);
      close(tuberia_hacia_siguiente[1]);
      exit(1);
    }

    if (id_proceso_hijo == 0) {
      // El hijo no necesita leer de la tubería del abuelo
      close(tuberia_entrada[0]);

      // Reasigna la nueva tubería como su tubería de entrada
      tuberia_entrada[0] = tuberia_hacia_siguiente[0];
      tuberia_entrada[1] = tuberia_hacia_siguiente[1];

      // Continúa en la siguiente iteración del bucle como el nuevo filtro
      continue;
    } else {
      // Filtro: proceso actual solo escribirá hacia el hijo: cierra lectura (0)
      close(tuberia_hacia_siguiente[0]);

      int numero_candidato;
      // Leemos los números restantes del extremo de lectura (0)
      while (read(tuberia_entrada[0], &numero_candidato, sizeof(numero_candidato)) > 0) {
        // Si no es múltiplo del número primo actual, lo enviamos al extremo de escritura (1)
        if (numero_candidato % numero_primo != 0) {
          write(tuberia_hacia_siguiente[1], &numero_candidato, sizeof(numero_candidato));
        }
        // Si es divisible, simplemente se ignora y se descarta
      }

      // Cerramos los descriptores restantes tras procesar todos los números
      close(tuberia_entrada[0]);
      close(tuberia_hacia_siguiente[1]);

      // Esperamos a que el hijo (y toda la descendencia) termine antes de salir
      wait(0);
      exit(0);
    }
  }
}

int main(int argc, char *argv[]) {
  int tuberia_inicial[2];
  pipe(tuberia_inicial);

  int id_proceso_primer_hijo = fork();
  if (id_proceso_primer_hijo < 0) {
    printf("primes: error al crear el primer hijo\n");
    close(tuberia_inicial[0]);
    close(tuberia_inicial[1]);
    exit(1);
  }

  if (id_proceso_primer_hijo == 0) {
    // El primer hijo inicia la cadena de selección
    seleccionar_primos(tuberia_inicial);
  } else {
    // El proceso padre solo escribe números: cerramos lectura (0)
    close(tuberia_inicial[0]);

    // Enviamos la secuencia de números del 2 al 35 por el extremo de escritura (1)
    for (int numero_actual = NUMERO_INICIAL; numero_actual <= NUMERO_FINAL; numero_actual++) {
      write(tuberia_inicial[1], &numero_actual, sizeof(numero_actual));
    }

    // Cerramos escritura (1) para indicar fin de datos al primer hijo
    close(tuberia_inicial[1]);

    // Esperamos a que concluya toda la cadena
    wait(0);
    exit(0);
  }

  exit(0);
}