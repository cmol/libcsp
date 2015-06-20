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

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <csp/csp.h>
#include <csp/csp_error.h>

#include <csp/arch/csp_system.h>

int csp_sys_tasklist(char * out) {
  /* TODO: Fix tasklist on eCos */
	strcpy(out, "Tasklist not available on ECOS");
	return CSP_ERR_NONE;
}

int csp_sys_tasklist_size(void) {
  /* TODO: Fix size tasklist on eCos */
	//strcpy(out, "Tasklist size not available on ECOS");
	return CSP_ERR_NONE;
}

uint32_t csp_sys_memfree(void) {
	uint32_t total = 0;
	struct mallinfo info;
  info = mallinfo();
	total = info.fordblks;
	return total;
}

int csp_sys_reboot(void) {
  /* TODO: Fix reboot on eCos */
	//strcpy(out, "Reboot is not available on ECOS");
	return CSP_ERR_INVAL;
}

void csp_sys_set_color(csp_color_t color) {
  /* TODO: Add eCos color output here */
}
