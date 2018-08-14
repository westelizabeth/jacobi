/* CSCI 347 Assignment 5
 *
 * 16 MARCH 2018, Chad Schillinger and Elizabeth West
 */

#include <stdlib.h>
#include <pthread.h>
#include "barrier.h"


struct barrier{
	pthread_mutex_t gate;
    pthread_cond_t release;
	int count;
};


/* name: barrier_spawn
 * parameters: none
 * return: barrier struct
 * notes:
 */
void barrier_spawn(struct barrier **str_barrier){

	int count = 0;
	pthread_mutex_t gate;
	pthread_cond_t release;
	pthread_mutex_init(&gate, NULL);
	pthread_cond_init(&release, NULL);
    *str_barrier =  malloc(sizeof(struct barrier));
    (*str_barrier)->count = count;
    (*str_barrier)->gate = gate;
    (*str_barrier)->release = release;
}


/* name: barrier_wait
 * parameters: barrier struct, number of threads
 * return: void
 * notes:
 */
void barrier_wait(struct barrier *b_struct, int num_threads){

    pthread_mutex_lock(&(b_struct->gate));
    b_struct->count++;
    if(b_struct->count != num_threads) {
        pthread_cond_wait(&(b_struct->release), &(b_struct->gate));
    }
    else{
        b_struct->count = 0;
        pthread_cond_broadcast(&(b_struct->release));
    }
    pthread_mutex_unlock(&(b_struct->gate));
}

/* name: barrier_demolish
 * parameters: barrier struct
 * return: void
 * notes: waits until barrier locks are free, then destroys
 */
void barrier_demolish(struct barrier *b_struct){
   //do nothing, just like BSD :)
}


