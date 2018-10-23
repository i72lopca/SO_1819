/*practica1.c

En esta práctica nos familiarizamos con el uso del comando fork() para la creación de procesos y 
el uso de recursos compartidos. Usaremos la función ftok, shmget, shmat,shmdt, shmctl  para la gestión de la memoria
Son útiles las funciones para identificar los procesos getpid y getppid
Hay que usar control de errores.


*/
#include <unistd.h> //Para fork()
#include <sys/shm.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h> //control errores
#include <sys/wait.h> 
#include <time.h>
#include <sys/ipc.h>//permisos para memoria


#define CHILDREN 3 // número de procesos hijos
#define ELEMENTOS 100 //número elementos del vector
#define RANDOM 20 //para rellenar aleatoriamente valores del vector con valores de 1-20
#define ITER1 10// veces que se repite proceso 1
#define ITER2 100// veces que se repite proceso 2
#define ITER3 5// veces que se repite proceso 3

/*

Nombre de la función:  funcion

Descripción: Función que asignará a cada proceso hijo una tarea distinta que realizar

Tipo de función:  void  

Parámetros:

	int proceso. Número de proceso que se acaba de crear
	int pidHijo. pid del proceso que se acaba de crear

*/

void funcion(int proceso, int pidHijo);





int main()
{
	int i,childFinish=0;// chidlFinish para controlar los procesos que terminan
    key_t key;
  	int *v;// vector que se compartirá por todos los procesos
  	int shmid,status, childpid; 
    pid_t pid;
    
   	//Memoria compartida por los procesos: Vector de 100 elementos Estamos en proceso padre hasta que no creemos los hijos.
    key = ftok("practica.c", 3);//he tenido problemas usando practica1.c y un 1 como identificador de proyecto de la función ftok
    if ((shmid = shmget(key, sizeof(int)*ELEMENTOS, IPC_CREAT | 0777)) == -1){
		exit(1);
	}	

   	v = (int*)shmat(shmid, NULL,0 );

    //rellenamos vector con valores aleatorios y lo mostramos

	for (int i = 0; i < ELEMENTOS; ++i){
		v[i]=1+rand()%RANDOM;
	}

	for (int i = 0; i < ELEMENTOS; ++i){
		printf("v[%d]: %d\n",i, v[i] );
	}

    //Creamos los hijos
    
    for (int i = 0; i < CHILDREN; i++) {
		pid=fork();
		switch(pid){
        	case -1:
            	perror("fork error");
            	printf("errno value= %d\n", errno);
            	exit(EXIT_FAILURE); 
            
       		case 0:{ //proceso hijo
            	printf("\nProceso hijo: %d- Proceso padre: %d\n",getpid(), getppid() );
            	funcion(i, getpid());    //se ejecuta la función para cada hijo, pasándole un entero para controlar la función que le corresponde y el propio pid del proceso
           		exit(EXIT_SUCCESS);//finalizamos el hijo al acabar con la función    
           		}break;     			
     	}
     	/*
		if(pid==0){

			printf("\nProceso hijo: %d- Proceso padre: %d\n",getpid(), getppid() );
            funcion(i, getpid());    //se ejecuta la función para cada hijo, pasándole un entero para controlar la función que le corresponde y el propio pid del proceso
          	exit(EXIT_SUCCESS);//finalizamos el hijo al acabar con la función  
           	break;

		}
		*/


    }
   
	 //El proceso padre espera a la finalización de los hijos

    for (i = 0; i < CHILDREN; i++) {
		childpid = wait(&status);
    	childFinish++;//para contar los procesos que van finalizando
		printf("\nChild %d finished with status %d hijos terminados: %d \n", childpid, WEXITSTATUS(status), childFinish);
    }	
		
    if (childFinish==CHILDREN){// si han finalizado todos los hijos indicamos que todos han finalizado
    	printf("Todos los procesos hijos han acabado\n" );
    	
    }

	if (childpid==-1 && errno==ECHILD) {
    	printf("Error en la invocacion de wait o la llamada ha sido interrumpida por una señal.\n");
    	exit(EXIT_FAILURE);
  	}
  	else {
    	printf("No hay más hijos que esperar \n\n");
    	
  	}

  	//Resultado final. Estamos en proceso padre y antes de finalizar imprimimos de nuevo el vector después de haber terminado todos los procesos.

  	for (int i = 0; i < ELEMENTOS; ++i){
		printf("v[%d]: %d\n",i, v[i] );
	}

	//libera variable de la memoria

	shmdt(v);

	//Memoria liberada

   	shmctl(shmid, IPC_RMID, NULL);

   	//Finalizamos el proceso Padre

    exit(EXIT_SUCCESS);//

	return 0;
}



void funcion(int proceso, int pidHijo){

	int shmid, *v, suma, indice, valor, j;
	key_t key;

	key = ftok("practica.c", 3);
    if ((shmid = shmget(key, sizeof(int)*ELEMENTOS,IPC_CREAT | 0777)) == -1){
		printf("Error reserva memoria compartida");
		exit(EXIT_FAILURE);
	}
	// variable desde la que se accede a la memoria
   	v = (int *) shmat(shmid, NULL, 0);
	

	if (proceso==0){
   		printf("\nRellenar manualmente elementos del vector - Proceso: %d\n", pidHijo);//identificamos el proceso que se lanza con su pid

		for (int i = 0; i < ITER1; ++i){
			printf("\n%d- Indique un índice del vector donde guardar un valor: ", i+1);
			scanf("%d", &indice);
			printf("\n%d- Indique el valor que guardar en el vector: ", i+1);
			scanf("%d", &valor);
			v[indice]=valor;
		}	
	}


	if (proceso==1){
   		printf("\nRellenar aleatoriamente - Proceso: %d\n", pidHijo);//identificamos el proceso que se lanza con su pid
			
		for (int i = 0; i < ITER2; ++i){
			j=rand()%ELEMENTOS;
			v[j]=1+rand()%RANDOM;
			printf("\n%d: v[%d]= %d\n",i, j, v[j] );
			sleep(1);
		}
	}


	if (proceso==2){
		
		for (int i = 0; i < ITER3; ++i){
			suma=0;
			for (int i = 0; i < ELEMENTOS; ++i){
				suma=suma+v[i];
			}
		
   		printf("\nCalcular suma elementos vector - Proceso %d\n", pidHijo);//identificamos el proceso que se lanza con su pid

		printf("\n%d- Suma de los elementos del vector: %d\n",i+1, suma);
		sleep(1);// después de la última suma espera 30 segundos antes de acabar el proceso

		}
		
	}

	 //libera variable de la memoria
	shmdt(v);
}

/*

Observaciones ejercicio

- No hay un orden de inicio prestalecido de los procesos
- Se va interrupiendo la ejecución de unos procesos con otros, hasta que finalizan de acuerdo a las iteraciones y tiempos de espera establecidos
- El proceso padre espera a que finalicen todos los hijos.

*/
	


