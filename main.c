#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUMBER_OF_PHILOSHOPERS 5
#define MAX_NUMBER_OF_DINNERS 6

static sem_t mutex;											// Semáforo para comensales que sentarse para cenar
static sem_t chopstick[NUMBER_OF_PHILOSHOPERS];				// Semáforo de los tenedores para cenar
static int philosopher_identifiers[NUMBER_OF_PHILOSHOPERS]; // Arreglo para almacenar los identificadores de cada filósofo
static int philosopher_counters[NUMBER_OF_PHILOSHOPERS];	// Arreglo para almacenar la cantidad de veces que cada filósofo ha comido

void print_philosopher(int, const char *);
void *philosopher_handler(void *);
void print_philosophers_count();

int main()
{
	pthread_t philosophers[NUMBER_OF_PHILOSHOPERS]; // Hilos para cada filósofo

	sem_init(&mutex, 0, NUMBER_OF_PHILOSHOPERS - 1); // Inicializando el mutex con 4, esto para evitar la condición de interbloqueo
	for (int i = 0; i < NUMBER_OF_PHILOSHOPERS; ++i)
	{
		sem_init(&chopstick[i], 0, 1);	// Inicializando el semáforo de cada tenedor con 1 para que solo un hilo lo pueda usar a la vez
		philosopher_identifiers[i] = i; // Inicializando los identicadores de cada filósofo
		philosopher_counters[i] = 0;	// Inicializando las veces que cada filósofo ha cenado a 0
	}

	bool should_keep_going = true; // Condición para llevar control de cuantas veces ejectuar nuestro ciclo

	while (should_keep_going)
	{
		for (int i = 0; i < NUMBER_OF_PHILOSHOPERS; i++)
			pthread_create(&philosophers[i], NULL, philosopher_handler, (void *)&philosopher_identifiers[i]); // Creamos cada filósofo (hilo) y le pasamos su identificador
		for (int i = 0; i < NUMBER_OF_PHILOSHOPERS; i++)
			pthread_join(philosophers[i], NULL); // Esperamos a que cada filósofo termine
		print_philosophers_count();				 // Mostramos el recuento de las veces que cada filósofo ha cenado
		sleep(0.5);								 // Dormimos por medio segundo

		int counter = 0; // Contador
		for (int i = 0; i < NUMBER_OF_PHILOSHOPERS; ++i)
			if (philosopher_counters[i] == MAX_NUMBER_OF_DINNERS)					  // Verificamos si el filósofo ya comio 6 veces
				++counter;										  // Incrementamos contador si es el caso
		should_keep_going = !(counter == NUMBER_OF_PHILOSHOPERS); // Continuamos si alguno de los filósofos no ha comido 6 veces
	}

	return 0;
}

void *philosopher_handler(void *arg)
{
	int id = *(int *)arg; // Obtenemos el identificador del filósofo

	if (philosopher_counters[id] == MAX_NUMBER_OF_DINNERS) // Si comio 6 veces, este filósofo ya no hace nada
		goto final;

	sem_wait(&mutex); // Esperamos a que el filósofo pueda entrar a cenar
	print_philosopher(id, "Ha llegado");
	sem_wait(&chopstick[id]);								 // Esperamos a que el filósofo pueda agarrar su tenedor
	sem_wait(&chopstick[(id + 1) % NUMBER_OF_PHILOSHOPERS]); // Esperamos a que el filósofo pueda agarrar el tenedor de al lado (derecha)

	print_philosopher(id, "Ha empezado a comer");
	++philosopher_counters[id]; // Incrementamos la cantidad de veces que el filósofo ha comido
	sleep(1);					// Dormimos por 1 segundo
	print_philosopher(id, "Ha terminado de comer");
	sem_post(&chopstick[(id + 1) % NUMBER_OF_PHILOSHOPERS]); // Liberamos el tenedor de al lado
	sem_post(&chopstick[id]);								 // Liberamos nuestro tenedor
	sem_post(&mutex);										 // Dejamos el cuarto para cenar

final:
}

// Muestra la tabla con el recuento de cenas por filósofo
void print_philosophers_count()
{
	printf("\n[Contadores]: ======\n");
	printf("[Contadores]: Recuento de cenas por filosofo\n");
	for (int i = 0; i < NUMBER_OF_PHILOSHOPERS; ++i)
	{
		int dinners = philosopher_counters[i]; // Obtenemos el número de cenas
		printf("[Contadores]: Filosofo %d ha cenado %d %s\n", i, dinners, dinners == 1 ? "vez" : "veces");
	}
	printf("[Contadores]: ======\n\n");
}

// Función de ayuda para imprimer mensajes a nombre de cierto filósofo
void print_philosopher(int id, const char *msg)
{
	printf("[Filosofo %d]: %s\n", id, msg);
}
