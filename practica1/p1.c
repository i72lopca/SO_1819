
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <math.h>
#include <time.h>

#define CHILDREN 3

void functions(int id, int* memoria ){
  int indice,valor;
  switch (id) {
    case 0:
      for(int i = 0; i< 10; i++){
        printf("Introduce el indice\n");
        scanf("%d",&indice );
        printf("Introduce el valor\n");
        scanf("%d",&valor );
        memoria[indice]=valor;
      }
      break;
    case 1:
      srand(time(NULL));
      for (int j = 0; j<100;j++){
        indice  = rand()%100;
        valor  = rand()%999;
        memoria[indice] = valor;
        sleep(1);
      }
      break;
    case 2:
    for (int k = 0; k<5;k++){
      int aux = 0;
      for (int l = 0;l<100;l++){
        aux = aux + memoria[k];
      }
      printf("Valor: %d\n",aux );
      sleep(5);
    }
      break;
  }

}

int main(int argc, char const *argv[]) {
  //**//
  // Pasamos a reservar un espacio de memoria para que trabajen los diferentes procesos
  key_t claveMemoria;
  int id_memoria,i=0,j=0,status;
  int* espacioMemoria;

  //Generamos la claveMemoria
  claveMemoria = ftok("/bin/ls",45);
  if (claveMemoria == -1){
    printf("Fallo al obtener la clave de memoria\n");
    exit(-1);
  }
  //Creamos la meemoria
  id_memoria =shmget(claveMemoria,sizeof(int)*100, 0777 | IPC_CREAT);
  if (id_memoria == -1){
    printf("Fallo al obtener la clave de memoria\n" );
    exit(-1);
  }
  // Reservamos la Memoria
  espacioMemoria =(int*)shmat (id_memoria, (char *)0,0);
  if (espacioMemoria == NULL){
    printf("Fallo al obtener memoria compartida\n");
    exit(-1);
  }
  //**//

  //Cremos los procesos hijos
  for (i = 0; i<CHILDREN;i++){
    if (!fork()){
      functions(i,espacioMemoria);
      exit(0);
    }
  }
  //Esperamos a que los hijos terminen el trabajo
  for (j=0; j<CHILDREN;j++){
    pid_t pid = wait(&status);
    printf("Hijo: %d finalizado con el estado: %d\n",pid,WEXITSTATUS(status));
  }
  // Liberamos la memoria compartida usada
  shmdt((char *)espacioMemoria);
  shmctl (id_memoria,IPC_RMID,(struct shmid_ds *)NULL);
  return 0;
}