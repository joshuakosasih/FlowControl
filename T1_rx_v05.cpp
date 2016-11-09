/*
* File : T1_rx.cpp
* Author : Joshua K - 012, Luthfi K -102, Albertus K - 100
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
#define LOWERLIMIT 5

char xonoff_message[2];
Byte rxbuf[RXQSIZE];
QTYPE rcvq = {0, 0, 0, RXQSIZE, rxbuf };
QTYPE *rxq = &rcvq;
Byte sent_xonxoff = XON;
bool send_xon = false, send_xoff = false;
/*server client addres */
struct sockaddr_in sserver, sclient;

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

		xonoff_message[0] = sent_xonxoff;
		//TODO: send 'sent_xonxoff' via socket
		r = sendto(sockfd, xonoff_message, strlen(xonoff_message), 0, (struct sockaddr *)&sclient,sizeof(sclient));

		//if send succeed
		send_xoff = true;
		//check error on signal
		if (r > 0)
			puts("Buffer > minimum upperlimit. Mengirim XOFF.");
		else
			puts("XOFF gagal dikirim");
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
	if ((queue->count <= LOWERLIMIT) && (!send_xon)){
		sent_xonxoff = XON;
		send_xon = true;
		send_xon = false;
		
		xonoff_message[0] = sent_xonxoff;
		int r;
		r = sendto(sockfd, xonoff_message, strlen(xonoff_message), 0, (struct sockaddr *)&sclient,sizeof(sclient));

		if (r > 0)
			puts("Buffer < maximum lowerlimit. Mengirim XON.");
		else
			puts("XON gagal dikirim");
	}
	
	//select current data from buffer
	current = &queue->data[queue->front];
	
	//move pointer for current data in buffer
	queue->front = queue->front + 1;
	if (queue->front >= RXQSIZE) queue->front -= RXQSIZE;
	
	//keep track of reduced buffer content size
	queue->count -= 1;
	
	return current;
}
