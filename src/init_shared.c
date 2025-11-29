#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "../include/shared_data.h"

#define KEY_FILE "../files/ftok_key"
#define PROJECT_ID 'A'
#define SHM_SIZE sizeof(shared_data_t)

int main(void){

    key_t key;
    int shmid;
    shared_data_t *data;

    key=ftok(KEY_FILE,PROJECT_ID);
    if(key==-1){
        perror("ftok blabla debug");
        exit(1);
    }
    printf("[init] Shared memory key: %d\n", key);

    shmid=shmget(key,SHM_SIZE, IPC_CREAT | IPC_EXCL | 0666);
    if(shmid==-1){
        perror("shmget");
        exit(1);
    }
    printf("[init] Created shared memory with id: %d\n", shmid);

    data=(shared_data_t * )shmat(shmid,NULL,0);
    if(data==(void *)-1){
        perror("shmat");
        exit(1);
    }
    

    memset(data,0, SHM_SIZE);
    
    if( sem_init(&data->mutex,1,1)==-1){
        perror("sem_init");
        exit(1);
    }
    printf("[init] Initialized semaphore in shared data\n");

    data->next_dialog_id=1;
    data->next_msg_id=1;

    printf("[init] Initialization of shared memory was successful\n");

    if(shmdt(data)==-1){
        perror("shmdt");
        exit(1);
    }
    
    return 0;

    



}
