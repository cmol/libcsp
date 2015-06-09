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

#include <stdint.h>
#include <stdlib.h>
#include <cyg/kernel/kapi.h>

/* CSP includes */
#include <csp/csp.h>

#include <csp/arch/csp_thread.h>

int csp_thread_create(csp_thread_return_t (* routine)(void *), const signed char * const thread_name, unsigned short stack_depth, void * parameters, unsigned int priority, csp_thread_handle_t * handle) {
  // Create kernel space and thread stack
  cyg_thread *thread = malloc(sizeof(cyg_thread));

  // TODO: This memory assignment will not be free'd on tread destruction.
  //       Find out if freeing the memory is needed, and make a fix for it if so.
  //       The allocated memory assumes a 16-bit wide stack
  unsigned char *stack = malloc(sizeof(unsigned char) * stack_depth * 2);

  // Initialize thread
  cyg_thread_create(priority,(csp_thread_return_t) *routine, (cyg_addrword_t) parameters,
    (char *)thread_name, (void *) stack, stack_depth * 2,
    handle, thread);

  // Resume thread (starts suspended)
  cyg_thread_resume(*handle);
  return 0;
}
