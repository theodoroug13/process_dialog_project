#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "../include/shared_data.h"

#define KEY_FILE "../files/ftok_key"
#define PROJECT_ID 'A'
#define SHM_SIZE sizeof(shared_data_t)

