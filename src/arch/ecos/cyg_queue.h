/*
Cubesat Space Protocol - A small network-layer protocol designed for Cubesats
Copyright (C) 2012 Gomspace ApS (http://www.gomspace.com)
Copyright (C) 2012 AAUSAT3 Project (http://aausat3.space.aau.dk) 
Copyright (C) 2015 Claus Lensbøl - s132308@student.dtu.dk

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

#ifndef _CYG_QUEUE_H_
#define _CYG_QUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

#include <csp/arch/csp_queue.h>
#include <cyg/kernel/kapi.h>

#define CYG_QUEUE_ERROR CSP_QUEUE_ERROR
#define CYG_QUEUE_EMPTY CSP_QUEUE_ERROR
#define CYG_QUEUE_FULL CSP_QUEUE_ERROR
#define CYG_QUEUE_OK CSP_QUEUE_OK

typedef struct cyg_queue_s {
	void * buffer;
	int size;
	int item_size;
	int items;
	int in;
	int out;
	cyg_mutex_t mutex;
	cyg_cond_t cond_full;
	cyg_cond_t cond_empty;
} cyg_queue_t;

cyg_queue_t * cyg_queue_create(int length, size_t item_size);
void cyg_queue_delete(cyg_queue_t * q);
int cyg_queue_enqueue(cyg_queue_t * queue, void * value, uint32_t timeout);
int cyg_queue_dequeue(cyg_queue_t * queue, void * buf, uint32_t timeout);
int cyg_queue_items(cyg_queue_t * queue);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // _CYG_QUEUE_H_

