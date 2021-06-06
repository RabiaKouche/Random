
/****------------------    KOUCHE Rabia   -------------------------******/
/*----  mini projet "l'équilibrage de la fonction random du c-----*/
/*------------------ L3 Informatique 2019/2020 ---------------------*/

#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#define RAND_MX 32767


float VARIATION(int *tab)
{
    float somme = 0.0, moyenne, resultat = 0.0;
    int i;
    for(i=0; i<RAND_MX; ++i)
    {
        somme += tab[i];
    }
    moyenne = somme/RAND_MX;
    for(i=0; i<RAND_MX; ++i)
        resultat += pow(tab[i] - moyenne, 2);
    return (sqrt(resultat/RAND_MX)/moyenne);
}

void initialiserTab(int *tab)
{
  int i;
  for(i=0; i<RAND_MX; ++i)
  {
    tab[i]=0;
  }
}


void afficherTab(int *tab)
{
  int i;
  for(i=0; i<RAND_MX; ++i)   printf(" Tab[%d] = %d \n",i+1,tab[i]);
  printf("\n");
}



int main(void)
{
  /* Identifiant de segment de la mémoire partager */
   int idshm,i,j,l;
   int *tabPartager;
   int *tabDestination;



   /* Création de la mémoire partager à partager entre les 4 processus */
   idshm = shmget(IPC_PRIVATE,RAND_MX * sizeof(int),0644|IPC_CREAT|IPC_EXCL);

    sem_t *semaphore = sem_open("test_semaphore", O_CREAT|O_EXCL, 0, 1);
    sem_unlink("test_semaphore");

    tabPartager= shmat(idshm , NULL , 0);
    tabDestination =(int*) malloc( RAND_MX * sizeof(int));


    srand(time(NULL));

    int fk=0;
    while(fork())
    {
       if(fk==4) goto fin;
       fk++;
     }


     sem_wait(semaphore);
     for(i = 1;i<=100000000;++i)
     {
         tabDestination[rand() % RAND_MX]++;
          if(i%10000 == 0){
              for(j = 0;j<RAND_MX;++j)
              {
                  tabPartager[j] += tabDestination[j];

                }
                  initialiserTab(tabDestination);
                }
              }
      sem_post(semaphore);

      exit(0);

      fin:
      for (l = 0; l <4 ; ++l)   wait(NULL);


      /* calcule de la somme */
      int s=0;

      for(i = 0;i < RAND_MX;++i)
      {
        s+= tabPartager[i];
      }



      afficherTab(tabPartager);
      printf(" La somme des valeurs de tableau principale est : %d",s);
      printf("\n Les coefficients de variation egale à : %f \n",VARIATION(tabPartager));
        if (VARIATION(tabPartager)<0.15) printf(" La fonction random du C est équilibrée.\n\n");
        else printf(" La fonction random du C n'est pas équilibrée.\n");


      shmdt(tabPartager);
      shmctl(idshm,IPC_RMID,NULL);

  	  exit(EXIT_SUCCESS);
}
