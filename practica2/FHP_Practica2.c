#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ITER    10
#define NHILOS  4

double counter = 0;

int Numero[NHILOS];
int Eligiendo[NHILOS];


void lock(int thread);//algorigmo de lamport
void unlock(int thread);
void *adder(void *);
int max_vector(int *number);

int main(){
    pthread_t hilos[NHILOS];
    int status, i, v[NHILOS];
    extern double counter;

    double *r_value;

    //memset((int*)Numero, 0, sizeof(Numero));
    //memset((bool*)Eligiendo, false, sizeof(Eligiendo));

    for (int i = 0; i < NHILOS; ++i){
        Numero[i]=0;
        Eligiendo[i]=false;
    }

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

int max_vector(int *number) {
  int higher = 0;
  for(int i = 0; i < NHILOS; i++) {
    if (number[i] > higher) {
      higher = number[i];
    }    
  }
  return higher;  
}

void lock(int thread) {

    //extern int Eligiendo[NHILOS];
    //extern int Numero[NHILOS];    

    for (int i = 0; i < NHILOS; ++i) {
        Eligiendo[thread] = 1;//cuando esta eligiendo numero su Eligiendo es true o 1
        //int max_ticket = 0;
        //int ticket = Numero[i];
        //max_ticket = ticket > max_ticket ? ticket : max_ticket;
        //if(ticket>max_ticket){
        //    max_ticket=ticket;
        //}
        Numero[thread] = max_vector(Numero) + 1;
        Eligiendo[thread] = 0;
        for (int j = 0; j < NHILOS; ++j) {
            while (Eligiendo[j]) { }
            while (Numero[j] != 0 &&
               (Numero[j] < Numero[thread] ||
                (Numero[j] == Numero[thread] && j < thread))) { }
        }
    }
}

void unlock(int thread) {
   
    Numero[thread] = 0;
}

void *adder(void *p){
    double l; //variable local donde se almacena el contador individual de cada hilo
    //si fuera la variable que se devolviera por el hilo, el resultado seria impreciso ya que esta se destruye
    //al finalizar el hijo
    double *to_return;//variable que recoge el valor que devolvera el hijo al finalizar
    extern double counter;//variable externa donde se va almacenando el contador pthread_exit
    int *id; //variable a la que asignamos el valor recibido porla funcion
    id = (int *) p;// se hace un casting (int*) al valor recibido por la funcion que es de tipo void

    for (int i = 0; i < ITER; i++) {
    //seccion critica del hilo
	   lock(*id);
       l = counter;
	   //fprintf(stdout, "Hilo %d: %f\n", *id, counter);
	   l++;
	   counter = l;
       unlock(*id);

    }

    to_return = malloc(sizeof(double));// es preciso reservar memoria para la viable que se crea que devolvera el hijo para que no se destruya al finalizar.
    *to_return = counter;
    pthread_exit((void *) to_return);// al salir devuleve

}
