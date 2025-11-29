#include "../include/dialog.h"
#include "../include/shared_data.h"
#include<stdio.h>
#include<stdlib.h>
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
                result_id=dialog->dialog_id;
                break;
            }
        }
    }
    else{
        for(int i=0;i<MAX_DIALOGS;i++){
            dialog_t *dialog=&data->dialogs[i];
            if(dialog->active && dialog_id==dialog->dialog_id){
                int free_slot=-1;
                for(int j=0;j<MAX_PROCESSES;j++){
                    //ελέγχω αν είναι ήδη μέσα το process
                    if(dialog->process[j]==pid){
                        result_id=dialog_id;
                        if (sem_post(&data->mutex) == -1) {
                            perror("sem_post");
                            return result_id;
                        }
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
                    result_id=dialog_id;
                }
                break;
            
            }
        }
    }
    if (sem_post(&data->mutex) == -1) {
        perror("sem_post");
    }
    return result_id;
}