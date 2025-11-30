#ifndef DIALOG_H
#define DIALOG_H

#include<stdlib.h>
#include "shared_data.h"

//Είσοδος proccess στον διάλογο(επιστρέφει: -1=αποτυχία ή id διαλόγου )
int join_dialog(shared_data_t *data, int dialog_id, pid_t pid);

//Αποστολή μηνύματος σε συγκεκριμμένο διάλογο
//επιστρέφει -1 για FAIL ή 1 για SUCCESS
int send_message(shared_data_t *data,int dialog_id, pid_t sender, char *text);

//Λήψη μηνύματος σε συγκεκριμένο διάλογο
//επιστρέφει -1 για FAIL ή 0 για SUCCESS
int receive_message(shared_data_t *data, int dialog_id, pid_t reciever, message_t *text);

#endif