//practica.c

#include <unistd.h> //Para fork()
#include <sys/shm.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h> 
#include <time.h>
#include <sys/ipc.h>//permisos para memoria


#define CHILDREN 3 // número de procesos hijos
#define ELEMENTOS 100 //número elementos del vector
#define RANDOM 20 //valores hasta 100

void funcion(int proceso);

int main()
{
	int i,status, childpid, childFinish=0;
    key_t key;
  	int *v, shmid;
    pid_t pid;


    
   	//Memoria compartida por los procesos
    key = ftok("practica1.c", 3);
    if ((shmid = shmget(key, sizeof(int)*ELEMENTOS, IPC_CREAT | 0777)) == -1){
		printf("Error reserva memoria compartida main");
		exit(EXIT_FAILURE);
	}

	// variable desde la que se accede a la memoria
	
   	v = (int*)shmat(shmid, NULL, 0);

	
    //rellenamos vector con valores aleatorios y lo mostramos


		for (int i = 0; i < ELEMENTOS; ++i)
		{
			v[i]=1+rand()%RANDOM;
		}



		for (int i = 0; i < ELEMENTOS; ++i)
		{
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
            
       		case 0: /* proceso hijo */
           		funcion(i);
            	printf("Hola\n");
            	
           		exit(EXIT_SUCCESS);         	
     	}
    }
   

	 // Wait to finish
    for (i = 0; i < CHILDREN; i++) {
		childpid = wait(&status);
		printf("\nChild %d finished with status %d\n", childpid, WEXITSTATUS(status));
    }	
		
    if (childFinish==CHILDREN){
    	printf("Todos los procesos hijos han acabado\n" );
    	
    }

	if (childpid==-1 && errno==ECHILD) {
    	printf("Error en la invocacion de wait o la llamada ha sido interrumpida por una señal.\n");
    	exit(EXIT_FAILURE);
  	}
  	else {
    	printf("No hay más hijos que esperar \n");
    	//exit(EXIT_SUCCESS);
  	}

  	//Resultado final

  	printf("Final de los procesos\n");
    exit(EXIT_SUCCESS);



  	//libera variable de la memoria
	shmdt(v);

	//Memoria liberada
   	shmctl(shmid, IPC_RMID, NULL);

	return 0;
}



void funcion(int proceso){

	int shmid, *v;
	key_t key;
	
	key = ftok("practica1.c", 3);
    if ((shmid = shmget(key, sizeof(int)*ELEMENTOS,0)) == -1)
		printf("Error reserva memoria compartida");
		exit(1);

	// variable desde la que se accede a la memoria
   	v = (int *) shmat(shmid, NULL, 0);
	
	if (proceso==0){



		printf("proceso 1\n");
	}


	if (proceso==1){

		printf("proceso 2\n");
	}


	if (proceso==2){

		printf("proceso 3\n");
	}

	 //libera variable de la memoria
	shmdt(v);


	

}
	


