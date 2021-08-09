#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <wait.h>


typedef struct info {int pid ; int rang ; int val ;};
#define Nombre 6
#define Mutex 0

int sem_mutex;
int sem_tab_mutex;

int memoire_p1 ;
int memoire_p2 ;



int main(int argc, char const *argv[]) {



  key_t cle;
  key_t Key;
  struct info *Acces_w1 ;
  int *cpt;
  int * v1;


  union semun{ int val; struct semid_ds*buf; unsigned short*array;
  } Smutex_init ,STab_Mutex_inite;

  //* Creation des semaphores*/

  sem_mutex = semget(ftok("Pgme.c",5),1,IPC_CREAT | 0666);
  if(sem_mutex ==-1) {printf("\n Erreur de création des semaphores"); exit(1);}


  Smutex_init.val = 1;//Initialisation
  semctl(sem_mutex,Mutex,SETVAL,Smutex_init);



  sem_tab_mutex = semget(ftok("Pgme.c",6),Nombre,IPC_CREAT | 0666);
  if(sem_tab_mutex ==-1) {printf("\n Erreur de création des tableau semaphores"); exit(1);}

  for(int i=0;i<Nombre-1;i++){
    STab_Mutex_inite.val = 0;//Initialisation
    semctl(sem_tab_mutex,i,SETVAL,STab_Mutex_inite);

  }
  STab_Mutex_inite.val = 1;//Initialisation
  semctl(sem_tab_mutex,Nombre-1,SETVAL,STab_Mutex_inite);

  //* Creation des segment partagées*/
  cle = ftok("Pgme.c", 3);
  memoire_p1 = shmget(cle, Nombre*sizeof(struct info),IPC_CREAT | 0666);
  Acces_w1 = shmat(memoire_p1,0,0);

  cle = ftok("Pgme.c", 4);
  memoire_p2 = shmget(cle, 2*sizeof(int),IPC_CREAT | 0666);
  cpt = shmat(memoire_p2,0,0);

/**Initialisation des variable cpt et v1*/
  cpt[0]= 0;//cpt
  cpt[1]=0;//v1

/*Detachement des segment */
  shmdt(cpt);
  shmdt(Acces_w1);

  int p;

  int err ;

  /**Creation de 6 Processus */
  for (int i = 0; i < Nombre; i++) {
    p=fork();

    if (p== -1 ){printf("Erreur de creation\n");exit(3);}

    if (p == 0) {
        err = execl("./Pgme_processus" ,"Pgme_processus",NULL);
        exit(4);
    }
  }

  p = wait(NULL);
  while (p != -1) {
    printf("Fils termnee %d \n",p );
    p = wait(NULL);
  }


  /*Destruction des semaphores et des memeoires*/

  semctl(sem_mutex,IPC_RMID,0);
  semctl(sem_tab_mutex,IPC_RMID,0);
  shmctl(memoire_p1, IPC_RMID,0);
  shmctl(memoire_p2, IPC_RMID,0);



  sleep(2);

  printf("FIN \n");



  return 0;
}
