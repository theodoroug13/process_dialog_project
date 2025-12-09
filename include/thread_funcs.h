#ifndef THREAD_FUNCS_H
#define THREAD_FUNCS_H

#include"shared_data.h"
#include<pthread.h>

typedef struct{
    shared_data_t *data;
    int dialog_id;
    pid_t pid;
    pthread_t sender_thread ;//για να μπορώ να τον κάνω cancel από το reciever για επδ μπλοκαριζόταν μετά από το TERMINATE
    
}client_context_t;

extern volatile int running;

//Συνάρτηση για το sender thread
void* sender_thread(void *arg);

//Συναάρτηση γιΑ το receiver thread
//
void* receiver_thread(void* arg);



#endif