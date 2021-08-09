#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <wait.h>

#define Mutex 0
#define Nombre 6 //nombre de Processus
#define Tab_Mutex 1

typedef struct info {int pid ; int rang ; int val ;};

struct sembuf PMutex = {Mutex,-1,0};
struct sembuf VMutex = {Mutex,1,0};



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

  sem_mutex = semget(ftok("Pgme.c",5),1,IPC_CREAT | 0666);

  if(sem_mutex ==-1) {printf("\n Erreur de création des semaphores"); exit(1);}

  sem_tab_mutex = semget(ftok("Pgme.c",6),Nombre,IPC_CREAT | 0666);
  if(sem_tab_mutex ==-1) {printf("\n Erreur de création des tableau semaphores"); exit(1);}

  cle = ftok("Pgme.c", 3);
  memoire_p1 = shmget(cle, Nombre*sizeof(struct info),IPC_CREAT | 0666);
  Acces_w1 = shmat(memoire_p1,0,0);

  cle = ftok("Pgme.c", 4);
  memoire_p2 = shmget(cle, 2*sizeof(int),IPC_CREAT | 0666);
  cpt = shmat(memoire_p2,0,0);

  int j, i ;
  struct info element ;

  semop(sem_mutex,&PMutex, 1);//p(mutex)
  j = cpt[0];
  cpt[0] = cpt[0] + 1 ;
  printf("Processus N %d , de PID %d \n" ,j,getpid() );

  semop(sem_mutex,&VMutex, 1);//v(mutex)

  struct sembuf PTab_Mutex = {j,-1,0};
  struct sembuf VTab_Mutex = {(j+Nombre-1)%Nombre,1,0};
  semop(sem_tab_mutex,&PTab_Mutex,1);///p(t[j])


 cpt[1] = cpt[1] + j ;//cpt=cpt+j
 element.pid = getpid();
 element.rang = j;
 element.val = cpt[1];//element.val=v1

 Acces_w1[j] = element;
 semop(sem_tab_mutex,&VTab_Mutex,1);///v(t[(j+n-1)mod n])


 if( j == 0 ) {
    printf("les resultats d'acces a la variable V1 :\n");
    for (int i = Nombre - 1 ; i >= 0; i-- ){
       printf("Acces N %d  par le Processus de PID= %d  de rang %d  v1=%d \n",Nombre -i + 1, Acces_w1[i].pid, Acces_w1[i].rang , Acces_w1[i].val);
    }
  }

  return 0;
}
