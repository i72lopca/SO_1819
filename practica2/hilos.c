#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

double counter = 0;

#define ITER	10
#define NHILOS	3

void *adder(void *);


int main(){
    pthread_t hilos[NHILOS];
    int status, i, v[NHILOS];
    extern double counter;
    double *r_value;

    // Create NHILOS threads
    for (i = 0; i < NHILOS; i++) {
	v[i] = i;
	if ((status = pthread_create(&hilos[i], NULL, adder, (void *) &v[i])))
	    exit(status);
    }

    // Wait threads
    for (i = 0; i < NHILOS; i++) {
	  
       pthread_join(hilos[i], (void **) &r_value);// se espera a la finalizacion de los hijos y r_value es la direccion de memoria donde
       //se guardara el valor devuelto por el hijo al finalizar
	  
       printf("Value returned by %lu thread: %lf\n", hilos[i], *r_value);
    }

    // Final result
    fprintf(stdout, "final result: %f\n", counter);

    return 0;
}

void *adder(void *p)
{
    double l; //variable local donde se almacena el contador individual de cada hilo
    //si fuera la variable que se devolviera por el hilo, el resultado seria impreciso ya que esta se destruye
    //al finalizar el hijo
    double *to_return;//variable que recoge el valor que devolvera el hijo al finalizar
    extern double counter;//variable externa donde se va almacenando el contador pthread_exit
    int *id; //variable a la que asignamos el valor recibido porla funcion
    id = (int *) p;// se hace un casting (int*) al valor recibido por la funcion que es de tipo void



    for (int i = 0; i < ITER; i++) {
    //seccion critica del hilo
	   l = counter;
	   fprintf(stdout, "Hilo %d: %f\n", *id, counter);
	   l++;
	   counter = l;

    }

    to_return = malloc(sizeof(double));// es preciso reservar memoria para la viable que se crea que devolvera el hijo para que no se destruya al finalizar.
    *to_return = counter;
    pthread_exit((void *) to_return);// al salir devuleve

}
