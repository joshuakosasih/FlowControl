#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include "dcomm.h"

/** GLOBAL VARIABLES **/
char lastByteReceived = XON;
int socket_desc;
int isMainUp = 1;
struct sockaddr_in server;
char str_to_send[2];


using namespace std;

void *XON_XOFF_HANDLER(void *args);

/**
 * MAIN function
 * sends bytes to receiver
 */
int main(int argc, char *argv[])
{

	FILE* file;
	int msg_len = 2;
	int sock;
  char* hostname = argv[1];
	char* port = argv[2];
	char* fileName = argv[3];


	/** Initialization **/
	hostname = argv[1];
	port = argv[2];
	fileName = argv[3];


	printf("----------------------------------\n");

	/** Socket **/

	// creates socket
    socket_desc = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }

    printf("Socket created\n");

    // initializes object server attributes
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(hostname);
    server.sin_port = htons(atoi(port));

    // binds the object server to the socket
    /*if(bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("Bind failed. Error");
        return 1;
    }

    printf("Bind done\n");
		*/ //yang dibind cuma port client receiver kok

	/** Initializes thread **/
	pthread_t xon_xoff_handler;

	if(pthread_create(&xon_xoff_handler, NULL, XON_XOFF_HANDLER, NULL) < 0)
	{
		perror("Could not create thread");
		return 1;
	}

	printf("XON/XOFF handler assigned\n");


	printf("----------------------------------\n\n");


	printf("Membuat socket untuk koneksi ke %s:%s ...\n", hostname, port);

	/** Sends bytes to receiver **/

	// open file
	file = fopen(fileName, "r");

	if (file == NULL) {
		perror("Could not open file");
		exit(1);
	}


	// reads a line from the file and stores it into str_to_send
	// reads (msg_len - 1) characters
	// msg_len is 2 so that it only reads one character
	int counter = 0;
	while (fgets(str_to_send, msg_len, file) != NULL) {

		if (lastByteReceived == XOFF) {

			printf("XOFF diterima.\n");

			// waiting for XON
			while (lastByteReceived == XOFF) {

					printf("Menunggu XON.\n");
			}

			printf("XON diterima.\n");
		}

		else {

			usleep(10000);
			// sending bytes (one character)
			counter++;
			printf("Mengirim byte ke-%d: '%s'\n", counter, str_to_send);
			sendto(socket_desc, str_to_send, strlen(str_to_send), 0,
					(struct sockaddr *)&server, sizeof(server));

			// reset string
			memset(str_to_send, 0, sizeof(str_to_send));

		}
	}

	isMainUp = 0;

	printf("Exit - Main");
	return 0;

}


/** THREAD
 * 	receives XON/XOFF
 */

void *XON_XOFF_HANDLER(void *args) {

	int rf;
	int server_len = sizeof(server);
	Byte recv_str[1];

	while (true) {

		// receiving XON/XOFF from the receiver
		//rf = recvfrom(socket_desc, recv_str, strlen(recv_str), 0, (struct sockaddr *)&server, (socklen_t*)&server_len);

		rf = recvfrom(socket_desc, recv_str, 1, 0, (struct sockaddr *)&server, (socklen_t*)&server_len);

		if (rf < 0) {

			perror ("ERROR receiving from socket");
			exit(1);

		}

		else {
			puts("Received XON/XOFF");
				printf("? %d\n", recv_str[0]);
			}

		// XON or XOFF
		lastByteReceived = recv_str[0];

		memset(recv_str, 0, sizeof(recv_str));

	}

	printf("Exit - XON/XOFF handler");
	pthread_exit(0);
}
