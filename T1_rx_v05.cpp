/*
* File : T1_rx.cpp
* Author : Joshua K - 012, Luthfi K, Albertus K - 100
*/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#include "dcomm.h"

/* Delay to adjust speed of consuming buffer, in milliseconds */
#define DELAY 500

/* Define receive buffer size */
#define RXQSIZE 8

char *xonoff_message;
Byte rxbuf[RXQSIZE];
QTYPE rcvq = {0, 0, 0, RXQSIZE, rxbuf };
QTYPE *rxq = &rcvq;
Byte sent_xonxoff = XON;
Boolean send_xon = false, send_xoff = false;

/* Socket */
int sockfd; // listen on sock_fd

/* Functions declaration */
static Byte *rcvchar(int, sockfd, QTYPE *queue);
static Byte *q_get(QTYPE *, Byte *);

int main(int argc, char *argv[])
{
	struct sockaddr_in sserver, sclient;

	Byte c;
	/*
	Insert code here to bind socket to the port number given in argv[1].
	*/
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
    {
        puts("Could not create socket");
        return 1;
    }
  puts("Socket created");

	sserver.sin_family = AF_INET;
  sserver.sin_addr.s_addr = INADDR_ANY;
	sserver.sin_port = htons(atoi(argv[1]));

	if (bind(sockfd,(struct sockaddr *)&sserver, sizeof(sserver)) < 0)
    {
        perror("Bind failed. Error");
        return 1;
    }
  puts("Bind done");
	printf("Bind at port : %d\n",atoi(argv[1]));

	/* Initialize XON/XOFF flags */

	/* Create child process */

	/*** IF PARENT PROCESS ***/
	while (true) {
		c = *(rcvchar(sockfd, rxq));

		/* Quit on end of file */
		if (c == Endfile) {
		exit(0);
		}
	}

	/*** ELSE IF CHILD PROCESS ***/
	while (true) {
	/* Call q_get */
	Byte *res;
	res = q_get(rcvq)
	/* Can introduce some delay here. */
	}
}

static Byte *rcvchar(int sockfd, QTYPE *queue)
{
	/*
	Insert code here. Read a character from socket and put it to the receive buffer. If the number of characters in the
	* receive buffer is above certain level, then send XOFF and set a flag (why?). Return a pointer to the buffer where data is put.
	*/
	int r; //result of sending signal
	if (queue->count >= RXQSIZE) { //buffer full

		if(!send_xoff) { //XOFF not sent yet
		sent_xonxoff = XOFF;
		send_xon = false;
		send_xoff = true;

		xonoff_message = &sent_xonxoff;
		//TODO: send 'sent_xonxoff' via socket
		r = sendto(sockfd, xonoff_message, strlen(xonoff_message), 0,(struct sockaddr *)&cli_addr,sizeof(cli_addr));

		//if send succeed
		send_xoff = true;

		}
	}
	else {

		//TODO: read char from socket (receive)

	}
}

/* q_get returns a pointer to the buffer where data is read or NULL if
* buffer is empty.
*/

static Byte *q_get(QTYPE *queue, Byte *data)
{
	Byte *current;
	/* Nothing in the queue */
	if (!queue->count) return (NULL);
	/*
	Insert code here. Retrieve data from buffer, save it to "current" and "data" If the number of characters in the receive buffer
	* is below certain level, then send XON. Increment front index and check for wraparound.
	*/
}
