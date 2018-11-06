
//matriz.c

/*Realice la multiplicación en paralelo de una matriz de 3x3 por un vector de 3x1. 
Para ello cree tres hebras que se repartan las filas de la matriz y del vector. 
Cada hijo debe imprimir la fila que le ha tocado y el resultado final de la multiplicación la 
cual además envía al padre. El padre debe esperar por la terminación de cada hijo y mostrar 
el vector resultante.*/

//gcc -Wpedantic


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define sz 3



void rellenamatriz();
void rellenavector();

void vermatriz();
void vervector();

void *multiplica();

int m[sz][sz];
int v[sz];

int main(int argc, char const *argv[]){

	extern int m[sz][sz];
	extern int v[sz];

	int nhilos=sz;

	int status, i;
	long *res;

	pthread_t vthread[nhilos];

	srand(time(NULL));
	rellenamatriz(m);
	rellenavector(v);

	printf("La matriz szxsz es:\n");
	vermatriz(m);
	printf("El vector  sz es:\n");
	vervector(v);


	//creando hilos

	for (i = 0; i < nhilos; i++){

		if ((status = pthread_create(&vthread[i], NULL, multiplica, (void *) &i)))
	    
	    exit(status);
    }



    //recibiendo hilos

    for (i = 0; i < nhilos; i++) {
	  
       pthread_join(vthread[i], (void **) &res);// se espera a la finalizacion de los hijos y r_value es la direccion de memoria donde
       //se guardara el valor devuelto por el hijo al finalizar
	  
       printf("Value returned by %lu thread: %i\n", vthread[i], *res);

       //v[vthread[i]]=*res;
    }

    //printf("%s\n", );



    //free res;


	return 0;
}


void rellenamatriz(int m[sz][sz]){
	for(int i=0; i<sz; i++){
		for(int j=0; j<sz; j++){
			m[i][j]=rand()%50;
		}
	}
}

void rellenavector(int v[sz]){
	for(int i=0; i<sz; i++){
		
		v[i]=rand()%50;
	
	}
}

void vermatriz(int m[sz][sz]){
	for(int i=0; i<sz; i++){
		for(int j=0; j<sz; j++){
			printf("%i\t", m[i][j]);
		}
		printf("\n");
	}
}

void vervector(int v[sz]){

	for (int i = 0; i < sz; ++i)
	{
		printf("%i\t", v[i]);
		
	}

}

void *multiplica(void *thread){

	int *hilo = (int *)thread;
	extern int m[sz][sz];
	extern int v[sz];

	long *res=(long*)malloc(sizeof(long)); //Esta memoria hay que liberarla en el main despues de usarla en el pthread_join()
	*res=666;
	printf("La matriz szxsz sdfadfa sdfs:\n");
	/*
	for(int i=0; i<sz; i++){
		(*res) += (m[*hilo][i] + v[i]);
	}
		printf("xxxx:\n");

	*/
	pthread_exit((void*) res);


}