#include <stdio.h> /* for printf() and fprintf() */
 #include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
 #include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
 #include <stdlib.h> /* for atoi() */
 #include <string.h> /* for memset() */
 #include <unistd.h> /* for close() */
 #include <errno.h>
 #include<netdb.h> //hostent
#include<arpa/inet.h>

 #define RCVBUFSIZE 32
 #define BUF_SIZE 1024
 char buffer[BUF_SIZE];
 size_t contentSize = 1;

/*MOST OF THIS CODE WAS TAKEN FROM THE TCP/IP BOOK*/
 
 int hostname_to_ip(char *  , char *);

/* Size of receive buffer */
/* Error handling function */
int main(int argc, char *argv[])
{
	int sock; /* Socket descriptor */
	struct sockaddr_in echoServAddr; /* Echo server address */
	unsigned short echoServPort; /* Echo server port */
	char *servIP; /* Server IP address (dotted quad) */
	char echoBuffer[RCVBUFSIZE]; /* Buffer for echo string */
	unsigned int echoStringLen; /* Length of string to echo */
	/* Bytes read in single recv()
	and total bytes read */
	int bytesRcvd, totalBytesRcvd;
	
	if (argc < 3) /* Test for correct number of arguments */
	{
		fprintf(stderr, "Wrong number of args %d \n", argc);
		exit(1);
	} 

	/*read from stdin: this stdin reading loop was taken from this StackOverflow post: 
	https://stackoverflow.com/questions/2496668/how-to-read-the-standard-input-into-string-variable-until-eof-in-c */
	char *echoString = malloc(sizeof(char) * BUF_SIZE);
	echoString[0] = '\0'; // make null-terminated
    while(fgets(buffer, BUF_SIZE, stdin))
    {
        char *old = echoString;
        contentSize += strlen(buffer);
        echoString = realloc(echoString, contentSize);
        if(echoString == NULL)
        {
            perror("Failed to reallocate content");
            free(old);
            exit(2);
        }
        strcat(echoString, buffer);
    }


	echoStringLen = strlen(echoString) ; /* Determine input length */
	echoServPort = atoi(argv[2]);
	
	char *hostname = argv[1];
	char ip[100];
	
	/*resolve hostname to ip*/
	hostname_to_ip(hostname , ip);
    servIP = ip;

	/* Create a reliable, stream socket using TCP */
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	    printf("error1: %s\n", strerror(errno));
	   

 	/* Construct the server address structure */
	memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
	echoServAddr.sin_family = AF_INET; /* Internet address family */
	echoServAddr.sin_addr.s_addr = inet_addr(servIP); /* Server IP address */
	echoServAddr.sin_port = htons(echoServPort); /* Server port */

	/* Establish the connection to the echo server */
	if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
		printf("error2: %s\n", strerror(errno));
		

 	/* Send the string to the server */
	if (send(sock, echoString, echoStringLen, 0) != echoStringLen)
		printf("error3: %s\n", strerror(errno));

       
	/* Receive the same string back from the server */
	totalBytesRcvd = 0;
	while (totalBytesRcvd < echoStringLen)
	{
	/* Receive up to the buffer size (minus i to leave space for
	a null terminator) bytes from the sender */
		if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
	    	printf("error4: %s\n", strerror(errno));
		totalBytesRcvd += bytesRcvd; /* Keep tally of total bytes */
		echoBuffer[bytesRcvd] = '\0'; /* Terminate the string! */
		//REPLACE THIS WITH STDOUT?
		fprintf(stdout,"%s", echoBuffer); /* Print the echo buffer */
	}

	close(sock);
	exit(0);
}


/*The following code to translate hostname to ip was found here: http://www.binarytides.com/hostname-to-ip-address-c-sockets-linux/*/
int hostname_to_ip(char * hostname , char* ip) {
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
         
    if ( (he = gethostbyname( hostname ) ) == NULL) 
    {
        // get the host info
        herror("gethostbyname");
        return 1;
    }
 
    addr_list = (struct in_addr **) he->h_addr_list;
     
    for(i = 0; addr_list[i] != NULL; i++) 
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }
     
    return 1;
}