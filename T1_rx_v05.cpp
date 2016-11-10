/*
* File : T1_rx.cpp
* Author : Joshua K - 012, Albertus K - 100, Luthfi K -102
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
#define UPPERLIMIT 7
#define LOWERLIMIT 5

/* Message */
char x_msg[2];
char r_msg[1];

/* Buffer */
Byte rxbuf[RXQSIZE];
QTYPE rcvq = {0, 0, 0, RXQSIZE, rxbuf };
QTYPE *rxq = &rcvq;
Byte sent_xonxoff = XON;
bool send_xon = false, send_xoff = false;

/* Server-Client socket address */
struct sockaddr_in sserver, sclient;
socklen_t cli_len = sizeof(sclient);

/* Socket */
int sockfd; // listen on sock_fd

/* Functions declaration */
static Byte *rcvchar(int sockfd, QTYPE *queue);
static Byte *q_get(QTYPE *, Byte *);

int main(int argc, char *argv[])
{
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
	sent_xonxoff = XON;
	send_xon = true, send_xoff = false;

	/* Create child process */
	int j=0;
	/*** IF PARENT PROCESS ***/
	while (true) {
		c = *(rcvchar(sockfd, rxq));
		if (!c) {
			j = 1;
		} else {
			j++;
		}
		/* Quit on end of file */
		if (c == Endfile) {
			exit(0);
		}
		printf("Menerima byte ke-%d.\n", j);
	}

	/*** ELSE IF CHILD PROCESS ***/
	while (true) {
	/* Call q_get */
	/* Can introduce some delay here. */
	}
}

static Byte *rcvchar(int sockfd, QTYPE *queue)
{
	/*
	Insert code here. Read a character from socket and put it to the receive buffer. If the number of characters in the
	* receive buffer is above certain level, then send XOFF and set a flag (why?). Return a pointer to the buffer where data is put.
	*/
	if ((queue->count >= UPPERLIMIT)&&(!send_xoff)) { //buffer full
		sent_xonxoff = XOFF;
		send_xon = false;
		send_xoff = true;

		x_msg[0] = sent_xonxoff;
		
		// send 'sent_xonxoff' via socket
		if (sendto(sockfd, x_msg, strlen(x_msg), 0, (struct sockaddr *)&sclient,sizeof(sclient)) > 0)
			puts("Buffer > minimum upperlimit. Mengirim XOFF.");
		else
			puts("XOFF gagal dikirim");
	}

	// read char from socket (receive)
	if (recvfrom(sockfd, r_msg, RXQSIZE, 0, (struct sockaddr *)&sclient, &cli_len) < 0)
		puts("Receive byte failed");
	else {
		// check end of file
		if(int(r_msg[0]) != Endfile){
			printf("Menerima byte: %c\n",r_msg[0]);
		}
		else
			return NULL;
	}
	// transfer from received msg to buffer
	queue->data[queue->rear] = r_msg[0];

	// increment rear index, check for wraparound, increased buffer content size
	queue->rear += 1;
	if (queue->rear >= RXQSIZE) queue->rear -= RXQSIZE;
	queue->count += 1;

	return queue->data;
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
	if ((queue->count <= LOWERLIMIT) && (!send_xon)){
		sent_xonxoff = XON;
		send_xon = true;
		send_xon = false;

		x_msg[0] = sent_xonxoff;

		if (sendto(sockfd, x_msg, strlen(x_msg), 0, (struct sockaddr *)&sclient,sizeof(sclient)) > 0)
			puts("Buffer < maximum lowerlimit. Mengirim XON.");
		else
			puts("XON gagal dikirim");
	}
	//select current data from buffer
	current = &queue->data[queue->front];

	//increment front index, check for wraparound, reduced buffer content size
	queue->front += 1;
	if (queue->front >= RXQSIZE) queue->front -= RXQSIZE;
	queue->count -= 1;

	return current;
}
