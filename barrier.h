/* CSCI 347 Assignment 5
 *
 * 16 MARCH 2018, Chad Schillinger and Elizabeth West
 */

#ifndef _BARRIER_H_
#define _BARRIER_H_

struct barrier;

/* name: barrier_spawn
 * parameters: none
 * return: barrier struct
 * notes:
 */
void barrier_spawn(struct barrier **str_barrier);

/* name: barrier_wait
 * parameters: barrier struct, number of threads
 * return: void
 * notes:
 */
void barrier_wait(struct barrier *b_struct, int num_threads);

/* name: barrier_demolish
 * parameters: barrier struct
 * return: void
 * notes: waits until barrier locks are free, then destroys
 */
void barrier_demolish(struct barrier *b_struct);

#endif