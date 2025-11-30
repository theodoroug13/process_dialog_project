#include"../include/thread_funcs.h"
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include "../include/dialog.h"

volatile int running=1;

void * sender_thread(void*arg){
    client_context_t *context=(client_context_t *)arg;
    char buffer[MAX_MSG_LENGTH];  
    
    

    printf("[process %d (sender)] Type message (or TERMINATE)\n",context->pid);

    while(running){
        if (fgets(buffer,sizeof(buffer),stdin)==NULL){
            break;
        }

        //για να φεύγουμε το enter
        int len=strlen(buffer);
        if(len>0 && buffer[len-1]=='\n'){
            buffer[len-1]='\0';
            len--;
        }

        //σημαίνει έχουμε άδεια γραμμή
        if (len==0){
            continue;
        }

        if (send_message(context->data,context->dialog_id, context->pid, buffer)==-1){
            fprintf(stderr,"[process %d (sender)] Failed to send message\n",context->pid);
            running=0;
            break;
        }

        if(strcmp("TERMINATE", buffer)==0){
            printf("[process %d (sender)] TERMINATING exit sender\n", context->pid);
            running=0;
            break;
        }

    }
    return NULL;
}

void *receiver_thread(void *arg){
    client_context_t *context=(client_context_t*)arg;
    message_t message;


    while (running){
        int result=receive_message(context->data, context->dialog_id,context->pid, &message);
        if(result==0){
            printf("\n[Dialog %d][from p:%d] %s", message.dialog_id, message.sender_id,message.text);
            printf("[process: %d] >",context->pid);
            fflush(stdout);
            if(strcmp("TERMINATE",message.text)==0){
                printf("[process %d (receiver)] TERMINATING exit receiver\n", context->pid);
                running=0;
                break;
            }
        }
        
        else{
            sleep(1) ;//για να αποφύγουμε busy waiting
        }
    }
    return NULL;
}