#ifndef THREAD_FUNCS_H
#define THREAD_FUNCS_H

#include"shared_data.h"

typedef struct{
    shared_data_t *data;
    int dialog_id;
    pid_t pid;
    
}client_context_t;

extern volatile int running;

//Συνάρτηση για το sender thread
void* sender_thread(void *arg);

//Συναάρτηση γιΑ το receiver thread
//
void* receiver_thread(void* arg);



#endif