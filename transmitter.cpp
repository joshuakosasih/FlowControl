#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>

#define XON (0x11)
#define XOFF (0x13)

/** GLOBAL VARIABLES **/ 
char lastByteReceived = XON;
int socket_desc;
int isMainUp = 1;
struct sockaddr_in server;


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
    char str_to_send[2];
    char* hostname = argv[0];
	char* port = argv[1];
	char* fileName = argv[2];
	
	
	/** Initialization **/
	hostname = argv[0];
	port = argv[1];
	fileName = argv[2];
	
	
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
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(port));
    
    // binds the object server to the socket
    if(bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("Bind failed. Error");
        return 1;
    }
    
    printf("Bind done\n");
	
	
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
				
				printf("Menunggu XON...\n");
				
			}
			
			printf("XON diterima.\n");
		}
		
		else {
			
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
	char recv_str[2];
	
	while (isMainUp == 1) {
		
		// receiving XON/XOFF from the receiver
		rf = recvfrom(socket_desc, recv_str, strlen(recv_str), 0, (struct sockaddr *)&server, (socklen_t*)&server_len);
		
		if (rf < 0) {
			
			perror ("ERROR receiving from socket");
			exit(1);
			
		}
		
		// XON or XOFF
		lastByteReceived = recv_str[0];
	}
	
	printf("Exit - XON/XOFF handler");
	pthread_exit(0);
}
