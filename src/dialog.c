#include "../include/dialog.h"
#include "../include/shared_data.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


int join_dialog(shared_data_t *data, int dialog_id, pid_t pid){
    if(sem_wait(&data->mutex)==-1){
        perror("sem_wait");
        exit(1);
    }

    int result_id=-1;
    if(dialog_id<0){
        for(int i=0;i<MAX_DIALOGS;i++){
            if(data->dialogs[i].active==0){
                dialog_t *dialog=&data->dialogs[i];
                dialog->active=1;
                dialog->dialog_id=data->next_dialog_id;
                data->next_dialog_id++;
                
                for (int j=0;j <MAX_PROCESSES; j++)
                {
                    dialog->process[j] = 0;
                }
                
                dialog->process[0]=pid;
                dialog->num_processes=1;
                result_id=dialog->dialog_id;
                break;
            }
        }
    }
    else{
        int found=0;
        for(int i=0;i<MAX_DIALOGS;i++){
            dialog_t *dialog=&data->dialogs[i];
            if(dialog->active && dialog_id==dialog->dialog_id){
                found=1;
                int free_slot=-1;
                for(int j=0;j<MAX_PROCESSES;j++){
                    //ελέγχω αν είναι ήδη μέσα το process
                    if(dialog->process[j]==pid){
                        result_id=dialog_id;
                        if (sem_post(&data->mutex) == -1) {
                            perror("sem_post");
                            return -1;
                        }
                        return result_id;
                    }
                    else if(dialog->process[j]==0 && free_slot==-1){
                        free_slot=j;
                    }
                }
                if(free_slot==-1){
                    fprintf(stderr, "[process:%d] Dialog %d is full\n", pid, dialog_id);
                    result_id=-1;
                }
                else{
                    dialog->process[free_slot]=pid;
                    dialog->num_processes++;
                    result_id=dialog_id;
                }
                break;
            
            }

        }
        if(found==0){
            fprintf(stderr,"[process: %d] Dialog %d wasnt found\n", pid, dialog_id);
        }
    }
    if (sem_post(&data->mutex) == -1) {
        perror("sem_post");
    }
    return result_id;
}

int send_message(shared_data_t *data,int dialog_id, pid_t sender, char *text){
    
    if(sem_wait(&data->mutex)==-1){
        perror("semwait failed in send");
        exit(1);
    }

    dialog_t *dialog=NULL;
    for(int i=0;i<MAX_DIALOGS;i++){
        if(data->dialogs[i].active && data->dialogs[i].dialog_id==dialog_id){
            dialog=&data->dialogs[i];
            break;
        }
    }

    if(dialog==NULL){
        fprintf(stderr,"[process:%d] Dialog %d not found\n", sender, dialog_id);
        sem_post(&data->mutex);
        return -1;  
    }

    int process_in_dialog=0;
    for(int i=0;i<MAX_PROCESSES;i++){
        if(dialog->process[i]==sender){
            process_in_dialog=1;
            break;
        }
    }
    if(process_in_dialog==0){
        fprintf(stderr, "[process:%d] Not part of the dialog\n", sender);
        sem_post(&data->mutex);
        return -1;
    }

    int free_slot=-1;
    for(int i=0;i<MAX_MESSAGES;i++){
        if(data->messages[i].exists==0){
            free_slot=i;
            break;
        }
    }
    if(free_slot==-1){
        fprintf(stderr, "[process:%d] No space for new messages in dialog %d\n", sender,dialog_id);
        sem_post(&data->mutex);
        return -1;
    }
    
    message_t *msg = &data->messages[free_slot];

    msg->exists=1;
    msg->dialog_id=dialog_id;
    msg->msg_id=data->next_msg_id++;
    msg->readers_left=dialog->num_processes-1;
    msg->sender_id=sender;
    for(int i=0;i<MAX_PROCESSES;i++){
        msg->read_by[i]=0;
    }
    strncpy(msg->text,text,MAX_MSG_LENGTH-1);
    msg->text[MAX_MSG_LENGTH-1]='\0'; 

    int result_id= sender;
    if(sem_post(&data->mutex)==-1){
        perror("sem_post");
        return -1;
    }

    return result_id;



}


int receive_message(shared_data_t *data, int dialog_id, pid_t receiver, message_t *text){

    if (sem_wait(&data->mutex)==-1){
        perror("semwait");
        exit(1);
    }

    dialog_t *dialog=NULL;
    for(int i=0;i<MAX_DIALOGS;i++){
        if (data->dialogs[i].active && data->dialogs[i].dialog_id==dialog_id ){
            dialog=&data->dialogs[i];
            break;
        }
    }
    if(dialog==NULL){
        fprintf(stderr,"[process:%d] Dialog %d not found\n", receiver, dialog_id);
        sem_post(&data->mutex);
        return -1;  
    }

    int receiver_idx=-1;
    for(int i=0;i<MAX_PROCESSES;i++){
        if(dialog->process[i]==receiver){
            receiver_idx=i;
            break;
        }
    }
    if(receiver_idx==-1){
        fprintf(stderr, "[process:%d] Not part of the dialog\n", receiver);
        sem_post(&data->mutex);
        return -1;
    }

    int msg_idx=-1;
    for (int i=0; i<MAX_MESSAGES;i++){
        message_t *message= &data->messages[i];
        if(message->exists && message->dialog_id==dialog_id && message->sender_id!=receiver &&message->readers_left>0
        && message->read_by[receiver_idx]==0){  //δεν ειμαι σίγουρος αν δεν πρεπει ο sender να διαβάζει τα μηνμυματα του
            msg_idx=i;
            break;
        }
    }
    if(msg_idx==-1){
        sem_post(&data->mutex);
        fprintf(stderr, "[process: %d] No message for me\n", receiver);
        return -1;
    }

    message_t *message=&data->messages[msg_idx];
    *text=*message;
    message->readers_left--;
    message->read_by[receiver_idx]=1;
    if (message->readers_left==0){
        message->exists=0;
        message->dialog_id=-1;
        message->msg_id=-1;
        message->sender_id=-1;
        message->text[0]='\0';
    }
    
    if(sem_post(&data->mutex)==-1){
        perror("sempost");
        return -1;
    }

    return 0;


}