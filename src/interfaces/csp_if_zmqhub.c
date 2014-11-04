/*
Cubesat Space Protocol - A small network-layer protocol designed for Cubesats
Copyright (C) 2012 GomSpace ApS (http://www.gomspace.com)
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

#include <assert.h>

/* CSP includes */
#include <csp/csp.h>
#include <csp/csp_interface.h>
#include <csp/arch/csp_thread.h>
#include <csp/interfaces/csp_if_zmqhub.h>

/* ZMQ */
#include <zmq.h>

static void * context;
static void * publisher;
static void * subscriber;

/**
 * Interface transmit function
 * @param packet Packet to transmit
 * @param timeout Timout in ms
 * @return 1 if packet was successfully transmitted, 0 on error
 */
int csp_zmqhub_tx(csp_iface_t * interface, csp_packet_t * packet, uint32_t timeout) {

	assert(publisher);

	/* Send envelope */
	char satid = (char) csp_rtable_find_mac(packet->id.dst);
	if (satid == (char) 255)
		satid = packet->id.dst;
	int result = zmq_send(publisher, &satid, 1, ZMQ_SNDMORE);
	if (result < 0)
		printf("ZMQ send error: %u %s\r\n", result, strerror(result));

	result = zmq_send(publisher, &packet->id, packet->length + sizeof(packet->id), 0);
	if (result < 0)
		printf("ZMQ send error: %u %s\r\n", result, strerror(result));

	csp_buffer_free(packet);

	return CSP_ERR_NONE;

}

CSP_DEFINE_TASK(csp_zmqhub_task) {

	while(1) {
		zmq_msg_t msg;
		zmq_msg_init_size(&msg, 1024);

		/* Receive header */
		zmq_msg_recv(&msg, subscriber, 0);
		//char id = ((char *) zmq_msg_data(&msg))[0];
		//printf("ZMQ received ID: %hhu ", id);

		/* Receive data */
		zmq_msg_recv(&msg, subscriber, 0);
		int datalen = zmq_msg_size(&msg);
		//printf("Datalen %u\r\n", datalen);

		/* Create new csp packet */
		csp_packet_t * packet = csp_buffer_get(256);
		packet->length = datalen - 4;
		memcpy(&packet->id, zmq_msg_data(&msg), datalen);

		/* Queue up packet to router */
		csp_qfifo_write(packet, &csp_if_zmqhub, NULL);
	}

	return CSP_TASK_RETURN;

}

int csp_zmqhub_init(char addr, char * host) {

	printf("INIT ZMQ with addr %hhu to server %s\r\n", addr, host);

	context = zmq_ctx_new();
	assert(context);

	char url[100];

	/* Publisher (TX) */
    publisher = zmq_socket(context, ZMQ_PUB);
    assert(publisher);
    sprintf(url, "tcp://%s:6000", host);
    int result = zmq_connect(publisher, url);
    if (result < 0)
    	printf("ZMQ bind error %s\r\n", strerror(result));

    /* Subscriber (RX) */
    subscriber = zmq_socket(context, ZMQ_SUB);
    sprintf(url, "tcp://%s:7000", host);
	assert(zmq_connect(subscriber, url) == 0);

	if (addr == (char) 255) {
		assert(zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "", 0) == 0);
	} else {
		assert(zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, &addr, 1) == 0);
	}

	/* Start RX thread */
	static csp_thread_handle_t handle_subscriber;
	int ret = csp_thread_create(csp_zmqhub_task, (signed char *) "ZMQ", 10000, NULL, 0, &handle_subscriber);
	printf("Task start %d\r\n", ret);

	/* Regsiter interface */
	csp_iflist_add(&csp_if_zmqhub);

	return CSP_ERR_NONE;

}

/* Interface definition */
csp_iface_t csp_if_zmqhub = {
	.name = "ZMQHUB",
	.nexthop = csp_zmqhub_tx,
};