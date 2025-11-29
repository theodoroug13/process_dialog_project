
#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <semaphore.h>
#include<sys/types.h>
#define MAX_DIALOGS 32
#define MAX_MSG_LENGTH 512
#define MAX_MESSAGES 256
#define MAX_PROCESSES 16

typedef struct{
    int active;
    int dialog_id;

    pid_t process[MAX_PROCESSES];
    int num_processes;
}dialog_t;

typedef struct{
    int exists;
    int msg_id;
    int dialog_id;
    pid_t sender_id;
    int readers_left;
    char text[MAX_MSG_LENGTH];
    
}message_t;

typedef struct{
    sem_t mutex;
    dialog_t dialogs[MAX_DIALOGS];
    message_t messages[MAX_MESSAGES];
    int next_dialog_id;
    int next_msg_id;
}shared_data_t;

#endif
