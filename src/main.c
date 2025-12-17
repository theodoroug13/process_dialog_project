#include"../include/thread_funcs.h"
#include"../include/dialog.h"
#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include<unistd.h>
#include<pthread.h>

#define KEY_FILE "../files/ftok_key"
#define PROJECT_ID 'A'
#define SHM_SIZE sizeof(shared_data_t)

int main(int argc, char** argv){
    key_t key;
    int shmid;
    shared_data_t *data;
    int dialog_id;

    key=ftok(KEY_FILE,PROJECT_ID);
    if(key==-1){
        perror("ftok");
        exit(1);
    }

    shmid=shmget(key,SHM_SIZE,0666);
    if(shmid==-1){
        perror("shmget");
        exit(1);
    }

    data=(shared_data_t*)shmat(shmid,NULL,0);
    if (data==(void*)-1){
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

        //προέκυψε από debugging για να δουλεύει σωστά η fgets στο sender_thead
        int c;
        while((c= getchar())!='\n' && c!= EOF) { }

    }

    pid_t pid=getpid();
    int slot;
    int joined_id=join_dialog(data,dialog_id,pid,&slot);
    if (joined_id==-1){
        fprintf(stderr, "[process %d] Failed to join dialog %d\n",pid, dialog_id);
        shmdt(data);
        exit(1);
    }
    else{
        printf("[process:%d] Joined dialog %d\n",pid, joined_id);
    }

    client_context_t context;
    context.data=data;
    context.dialog_id=joined_id;
    context.pid=pid;
    context.slot_idx=slot;
    running=1;

    pthread_t thread_sender_id, thread_receiver_id;
    if(pthread_create(&thread_sender_id,NULL,sender_thread,&context)!=0){
        perror("pthread sender create failed");
        shmdt(data);
        exit(1);
    }

    context.sender_thread=thread_sender_id;
    if(pthread_create(&thread_receiver_id,NULL,receiver_thread,&context)!=0){
        perror("pthread receiver create failed");
        running=0;
        pthread_join(thread_sender_id,NULL);
        shmdt(data);
        exit(1);
    }

    pthread_join(thread_sender_id,NULL);
    pthread_join(thread_receiver_id,NULL);

    if(shmdt(data)==-1){
        perror("shmdt");
        exit(1);
    }

    printf("[process: %d] Client exiting\n",pid);

    return 0;

}