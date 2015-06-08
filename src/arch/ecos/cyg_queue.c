/*
Cubesat Space Protocol - A small network-layer protocol designed for Cubesats
Copyright (C) 2012 Gomspace ApS (http://www.gomspace.com)
Copyright (C) 2012 AAUSAT3 Project (http://aausat3.space.aau.dk) 

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
Inspired by c-pthread-queue by Matthew Dickinson
http://code.google.com/p/c-pthread-queue/
*/

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <cyg/kernel/kapi.h>

/* CSP includes */
#include "cyg_queue.h"

cyg_queue_t * cyg_queue_create(int length, size_t item_size) {

	cyg_queue_t * q = malloc(sizeof(cyg_queue_t));

	if (q != NULL) {
		q->buffer = malloc(length*item_size);
		if (q->buffer != NULL) {
			q->size = length;
			q->item_size = item_size;
			q->items = 0;
			q->in = 0;
			q->out = 0;

      // As the cyg_ versions of the conditions and mutexes has no way of providing
      // feedback on creation, we just have to trust that it works.
			cyg_mutex_init(&(q->mutex));
      cyg_cond_init(&(q->cond_full), &(q->mutex));
      cyg_cond_init(&(q->cond_empty), &(q->mutex));
		} else {
			free(q);
			q = NULL;
		}
	}

	return q;

}

void cyg_queue_delete(cyg_queue_t * q) {

	if (q == NULL)
		return;

	free(q->buffer);
	free(q);

	return;

}


int cyg_queue_enqueue(cyg_queue_t * queue, void * value, uint32_t timeout) {

	cyg_bool_t ret;

	/* Calculate timeout */
  // TODO: Again, set tick time on compile, not as magic 10ms
  uint32_t wait_time = cyg_current_time() + timeout / 10;

	/* Get queue lock */
	cyg_mutex_lock(&(queue->mutex));
	while (queue->items == queue->size) {
		ret = cyg_cond_timedwait(&(queue->cond_full), wait_time);
		if (!ret) {
			cyg_mutex_unlock(&(queue->mutex));
			return CYG_QUEUE_FULL;
		}
	}

	/* Coby object from input buffer */
	memcpy(queue->buffer+(queue->in * queue->item_size), value, queue->item_size);
	queue->items++;
	queue->in = (queue->in + 1) % queue->size;
	cyg_mutex_unlock(&(queue->mutex));
	
	/* Nofify blocked threads */
	cyg_cond_broadcast(&(queue->cond_empty));

	return CYG_QUEUE_OK;

}

int cyg_queue_dequeue(cyg_queue_t * queue, void * buf, uint32_t timeout) {

	cyg_bool_t ret;
	
	/* Calculate timeout */
  // TODO: Again, set tick time on compile, not as magic 10ms
  uint32_t wait_time = cyg_current_time() + timeout / 10;
	
	/* Get queue lock */
	cyg_mutex_lock(&(queue->mutex));
	while (queue->items == 0) {
		ret = cyg_cond_timedwait(&(queue->cond_empty), wait_time);
		if (!ret) {
			cyg_mutex_unlock(&(queue->mutex));
			return CYG_QUEUE_EMPTY;
		}
	}

	/* Coby object to output buffer */
	memcpy(buf, queue->buffer+(queue->out * queue->item_size), queue->item_size);
	queue->items--;
	queue->out = (queue->out + 1) % queue->size;
	cyg_mutex_unlock(&(queue->mutex));
	
	/* Nofify blocked threads */
	cyg_cond_broadcast(&(queue->cond_full));

	return CYG_QUEUE_OK;
	
}

int cyg_queue_items(cyg_queue_t * queue) {

	cyg_mutex_lock(&(queue->mutex));
	int items = queue->items;
	cyg_mutex_unlock(&(queue->mutex));

	return items;

}
