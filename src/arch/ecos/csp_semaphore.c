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

#include <sys/time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include <cyg/kernel/kapi.h>

/* CSP includes */
#include <csp/csp.h>

#include <csp/arch/csp_semaphore.h>

int csp_mutex_create(csp_mutex_t * mutex) {
	csp_log_lock("Mutex init: %p", mutex);
	cyg_semaphore_init(mutex, 1);
	return CSP_SEMAPHORE_OK;
}

int csp_mutex_remove(csp_mutex_t * mutex) {
	return cyg_semaphore_destroy(mutex);
}

int csp_mutex_lock(csp_mutex_t * mutex, uint32_t timeout) {
	return csp_bin_sem_wait(mutex, timeout);
}

int csp_mutex_unlock(csp_mutex_t * mutex) {
	return csp_bin_sem_post(mutex);
}

int csp_bin_sem_create(csp_bin_sem_handle_t * sem) {
	csp_log_lock("Semaphore init: %p", sem);
	cyg_semaphore_init(sem, 1)
	return CSP_SEMAPHORE_OK;
}

int csp_bin_sem_remove(csp_bin_sem_handle_t * sem) {
	cyg_semaphore_destroy(sem);
  return CSP_SEMAPHORE_OK;
}

int csp_bin_sem_wait(csp_bin_sem_handle_t * sem, uint32_t timeout) {
	csp_log_lock("Wait: %p", sem);

  cyg_bool_t ret;

	if (timeout == CSP_MAX_DELAY) {
    ret = cyg_semaphore_wait(sem);
  } else {
  // TODO: do not use a magic number for tick time (10ms)
		timeout = cyg_current_time() + timeout / 10;
    ret = semaphore_timed_wait(*sem, timeout);
  }

  return ret ? CSP_SEMAPHORE_OK : CSP_SEMAPHORE_ERROR;
}

int csp_bin_sem_post(csp_bin_sem_handle_t * sem) {
	CSP_BASE_TYPE dummy = 0;
	return csp_bin_sem_post_isr(sem, &dummy);
}

int csp_bin_sem_post_isr(csp_bin_sem_handle_t * sem, CSP_BASE_TYPE * task_woken) {
	csp_log_lock("Post: %p", sem);
	*task_woken = 0;

	int value;
	cyg_semaphore_peek(sem, &value);
	if (value > 0)
		return CSP_SEMAPHORE_OK;

  cyg_semaphore_post(sem);
	return CSP_SEMAPHORE_OK;
}
