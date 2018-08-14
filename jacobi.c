/* CSCI 347 Assignment 5
 *
 * 16 MARCH 2018, Chad Schillinger and Elizabeth West
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>
#include "barrier.h"

/* CONSTANTS */
#define isSpace 0
#define notSpace 1
#define size 1024
#define threshhold .00001


struct th_args{
    int th_id;
	int num_threads;
	int *count;
	float (*master_matrix)[size];
	float (*fake)[size];
    float *deltas;
 };

struct barrier* barrier;

//prototypes
static void creat_matrix(FILE* matrix_file, float (*master_matrix)[size]);
void populate_fake(float (*fake)[size]);
char **arg_parse(char *line);
void print_inner(float **print_matrix);
float average(struct th_args *thread_args, int x_position , int y_position);
void *snake(void * info);
static void update_master(struct th_args *thread_args);



/* MAIN
 * parameters:
 * return:
 * notes:
 */
 int main(int argc, const char* argv[]) {
     if (argc < 3) {
         fprintf(stderr, "Usage: ./jacobi File-Name Number-of-Threads\n Exiting...\n");
         exit(1);
     } else {
         int numofthreads = atoi(argv[2]);
         FILE *matrix_file = fopen(argv[1], "r");
         if (matrix_file == NULL) {
             fprintf(stderr, "Matrix file not found. Exiting...\n");
             exit(1);
         }
         int count = 0;
         int done_threads = 0;
         float (*master_matrix)[size] = malloc(sizeof(float) * size * size);
         float (*fake)[size] = malloc(sizeof(float) * size * size);
         creat_matrix(matrix_file, master_matrix);
         populate_fake(fake);
         float deltas[numofthreads];
         pthread_t threads[numofthreads];
         struct th_args *t_structs[numofthreads];
         barrier_spawn(&barrier);

         for (int k = 0; k < numofthreads; k++) {
             t_structs[k] = malloc(sizeof(struct th_args));
             t_structs[k]->th_id = k;
             t_structs[k]->num_threads = numofthreads;
             t_structs[k]->master_matrix = master_matrix;
             t_structs[k]->fake = fake;
             t_structs[k]->deltas = deltas;
             pthread_create(&threads[k], NULL, &snake, t_structs[k]);
         }

         for (int i = 0; i < numofthreads; i++) {
             pthread_join(threads[i], NULL);
         }
     }
 }


/* name: creat_matrix
 * parameters: file to be read from, matrix to write to
 * return: void
 * notes:
 */
     static void creat_matrix(FILE *matrix_file, float (*master_matrix)[size]) {
         size_t bufsize = 0;
         char *line = NULL;
         for (int i = 0; i < size; i++) {
             getline(&line, &bufsize, matrix_file);
             char **this_is_just_one_single_parsed_row = arg_parse(line);
             for (int j = 0; j < size; j++) {
                 master_matrix[i][j] = atof(this_is_just_one_single_parsed_row[j]);
             }
         }
     }


/* name: populate_fake
 * parameters:
 * return: void
 * notes: fills matrix with 0 values
 */
     void populate_fake(float (*fake)[size]) {
         for (int i = 0; i < size; i++) {
             for (int j = 0; j < size; j++) {
                 fake[i][j] = 0.0;
             }
         }
     }


/* name: snake
 * parameters: thread struct
 * return: void
 * notes:
 */
     void *snake(void * info) {
         struct th_args *thread_args = info;
         int count = (size - 2) * (size - 2);
         float max = 0;
         while (count != 0) {
             max=0;
             for (int i = (thread_args->th_id)+1; i < size - 1; i = i+(thread_args->num_threads)) {
                 for (int j = 1; j < size - 1; j++) {
                     //need to test threshold value
                     (thread_args->fake)[i][j] = average(thread_args, i, j);
                     //printf("Threshold checking : fake[i][j] %f master[i][j] %f \n", (*fake)[i][j], (*master_matrix)[i][j]);

                     float faker = thread_args->fake[i][j];
                     float mast = thread_args->master_matrix[i][j];

                     if(((thread_args->fake)[i][j] - thread_args->master_matrix[i][j]) > max){
                         max = (thread_args->fake[i][j] - thread_args->master_matrix[i][j]);
                     }
                 }
             }

             thread_args->deltas[thread_args->th_id] = max;
             barrier_wait(barrier, thread_args->num_threads);
             float mostest_max = 0;
             for(int h = 0; h < thread_args->num_threads ; h ++){
                 //printf("delta %d: %f \n", h, thread_args->deltas[h]);
                 float temp = thread_args->deltas[h];
                 if(mostest_max < thread_args->deltas[h]){
                     mostest_max = thread_args->deltas[h];
                 }
             }
                 if(mostest_max <= threshhold){
                     count = 0;
                 }
             update_master(thread_args);
         }
         free(thread_args);
         barrier_demolish(barrier);
    return NULL;
     }


/* name: average
 * parameters: thread struct, i and j positions of index to be averaged
 * return: new value
 * notes: computes average of indices(4) directly surrounding index
 */
float average(struct th_args *thread_args, int x_position, int y_position) {
    float ave = 0.0;
    float ave1 = thread_args->master_matrix[x_position][y_position - 1];
    float ave2 = thread_args->master_matrix[x_position][y_position + 1];
    float ave3 = thread_args->master_matrix[x_position - 1][y_position];
    float ave4 = thread_args->master_matrix[x_position + 1][y_position];
    ave = (ave1 + ave2 + ave3 + ave4) / 4;
    return ave;
}


/* name: update_master
 * parameters: thread struct
 * return: void
 * notes:
 */
     static void update_master(struct th_args *thread_args) {

         for (int i = 1; i < size - 1; i++) {
             for (int j = 1; j < size - 1; j++) {
                 (thread_args->master_matrix)[i][j] = (thread_args->fake)[i][j];
             }
         }
        barrier_wait(barrier, thread_args->num_threads);
     }


/* name: arg_parse
 * parameters:
 * return: array of strings
 * notes: parsed/separated on spaces
 */
char **arg_parse(char *line) {
    int numOfArg = size;
    char **arguments = malloc(sizeof(char *) * (numOfArg + 1));
    int state = isSpace;
    int k = 0;
    while (*line != '\0') {
        switch (state) {
            case isSpace:
                if (!(isspace(*line))) {
                    state = notSpace;
                    arguments[k] = line;
                    k++;
                }
                break;

            case notSpace:
                if (isspace(*line)) {
                    state = isSpace;
                    *line = '\0';
                }
                break;
        }
        line++;
    }
    arguments[k] = '\0';
    return arguments;
}



