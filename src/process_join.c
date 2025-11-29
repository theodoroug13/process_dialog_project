#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include<unistd.h>
#include"../include/dialog.h"
#include "../include/shared_data.h"

#define KEY_FILE "../files/ftok_key"
#define PROJECT_ID 'A'
#define SHM_SIZE sizeof(shared_data_t)



int main(int argc, char **argv){

    key_t key;
    int shmid;
    shared_data_t *data;
    int dialog_id;
    pid_t pid;
    int result;

    key=ftok(KEY_FILE,PROJECT_ID);
    if(key==-1){
        perror("ftok");
        exit(1);
    }

    shmid=shmget(key,SHM_SIZE, 0666);
    if(shmid==-1){
        perror("shmget");
        exit(1);
    }

    data=(shared_data_t * )shmat(shmid,NULL,0);
    if(data==(void *)-1){
        perror("shmat");
        exit(1);
    }
    
    if(argc>=2){
        dialog_id=atoi(argv[1]);
    }
    else{
        printf("Give dialog id, (-1 for new)\n");
        if(scanf("%d",&dialog_id)!=1){
            fprintf(stderr,"wrong input\n");
            shmdt(data);
            exit(1);
        }
    }

    pid=getpid();
    result=join_dialog(data,dialog_id,pid);
    if (result==-1){
        fprintf(stderr,"[process:%d]Failed to join dialog\n",pid);
    }
    else{
        printf("[process:%d] Joined dialog %d\n",pid, result);
    }

    if(shmdt(data)==-1){
        perror("shmdt");
        exit(1);
    }

    return 0;

}