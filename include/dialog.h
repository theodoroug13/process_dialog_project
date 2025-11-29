#ifndef DIALOG_H
#define DIALOG_H

#include<stdlib.h>
#include "shared_data.h"

//Είσοδος proccess στον διάλογο(επιστρέφει: -1=αποτυχία ή id διαλόγου )
int join_dialog(shared_data_t *data, int dialog_id, pid_t pid);

#endif