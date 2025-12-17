#include"../include/thread_funcs.h"
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include "../include/dialog.h"

volatile int running=1;

void * sender_thread(void*arg){
    client_context_t *context=(client_context_t *)arg;
    char buffer[MAX_MSG_LENGTH];  
    
    

    printf("[process %d (sender)] Type message (or TERMINATE)\n [YOU (%d)]",context->pid, context->pid);

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
            sem_wait(&context->data->mutex);
            dialog_t * dialog=NULL;
            int slot=-1;
            for (int i=0;i<MAX_DIALOGS;i++){
                if(context->data->dialogs[i].active && context->data->dialogs[i].dialog_id ==context->dialog_id){
                    dialog= &context->data->dialogs[i];
                     break;
                 }
            }
            if(dialog!=NULL){
               for(int i=0;i<MAX_PROCESSES;i++){
                   if (dialog->process[i]==context->pid){
                        slot=i;
                        break;
                    }   
                }
            }     
            sem_post(&context->data->mutex);
            if(dialog!=NULL && slot!=-1){
                sem_post(&dialog->dialog_mutex[slot]);
            }
            break;
        }
        
        printf("[YOU (%d)] >",context->pid);
        fflush(stdout);
    }
    return NULL;
}

void *receiver_thread(void *arg){
    client_context_t *context=(client_context_t*)arg;
    message_t message;
    int slot=-1;
    dialog_t *dialog=NULL;
    
    sem_wait(&context->data->mutex);
    for (int i=0;i<MAX_DIALOGS;i++){
        if(context->data->dialogs[i].active && context->data->dialogs[i].dialog_id ==context->dialog_id){
            dialog= &context->data->dialogs[i];
            break;
        }
    }
    if(dialog!=NULL){
        for(int i=0;i<MAX_PROCESSES;i++){
            if (dialog->process[i]==context->pid){
                slot=i;
                break;
            }
        }
    }



    sem_post(&context->data->mutex);
    if(dialog==NULL  || slot == -1){
        fprintf(stderr, "[process %d (receiver)] Dialog not found\n", context->pid);
        return NULL;
    }

    while (running){
        sem_wait(&dialog->dialog_mutex[slot]);
        if(!running){
            break;
        }
        int result=receive_message(context->data, context->dialog_id,context->pid, &message);
        if(result==0){
            printf("\n[Dialog %d][from p:%d] %s\n", message.dialog_id, message.sender_id,message.text);
            printf("[YOU (%d)] >",context->pid);
            fflush(stdout);
            if(strcmp("TERMINATE",message.text)==0){
                printf("[process %d (receiver)] TERMINATING exit receiver\n", context->pid);
                running=0;
                pthread_cancel(context->sender_thread);
                break;
            }

        }
        
    }
    return NULL;
}