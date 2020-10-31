#include <stdio.h>          /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 1024

int checkMessage(char *buff, char *letStr, char *numStr);

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("The number of arguments is incorrect\n");
		return 0;
	} 
	
	int server_sock; /* file descriptors */
	char buff[BUFF_SIZE];
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;

	//Step 1: Construct a UDP socket
	if ((server_sock=socket(AF_INET, SOCK_DGRAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		exit(0);
	}

	//Step 2: Bind address to socket
	server.sin_family = AF_INET;         
	server.sin_port = htons(atoi(argv[1]));   /* Convert multi-byte integer types from host byte order to network byte order  */
	server.sin_addr.s_addr = INADDR_ANY;  /* INADDR_ANY puts your IP address automatically */   
	bzero(&(server.sin_zero),8); /* zero the rest of the structure */


	if(bind(server_sock,(struct sockaddr*)&server,sizeof(struct sockaddr))==-1){ /* calls bind() */
		perror("\nError: ");
		exit(0);
	}     

	printf("Server is running in port %d...\n\n", atoi(argv[1]));
	//Step 3: Communicate with clients
	while(1){
		char numStr[100], letStr[100], message[300];

		sin_size=sizeof(struct sockaddr_in);

		bytes_received = recvfrom(server_sock, buff, BUFF_SIZE-1, 0, (struct sockaddr *) &client, &sin_size);

		if (bytes_received < 0)
			perror("\nError: ");
		else {
			buff[bytes_received] = '\0';
			// TODO
			// printf("[%s:%d]: %s", inet_ntoa(client.sin_addr), ntohs(client.sin_port), buff);
			
			int result = checkMessage(buff,letStr,numStr);
			if (result == 0) {
				printf("Error: Invalid character\n\n");
				strcpy(message, "Error: Invalid character");
				sendto(server_sock, message, strlen(message), MSG_CONFIRM, (struct sockaddr *) &client, sin_size );
			} else {
				strcpy(message, "String 1: ");
				strcat(message, letStr);
				strcat(message, "\nString 2: ");
				strcat(message, numStr);
				sendto(server_sock, message, strlen(message), MSG_CONFIRM, (struct sockaddr *) &client, sin_size );
			}
		}
					
	}

	close(server_sock);
	return 0;
}

int checkMessage(char *buff, char *letStr, char *numStr){
	int k=0;
	int j=0;

	for(int i = 0; i < strlen(buff);i++){
		char ch = buff[i];
		if (ch == '\0' || ch == '\n') break;
		if(ch >= '0' && ch <= '9'){
			numStr[j] = ch;
    	   	j++;
   		}
   		else if ((ch >= 'a' && ch <= 'z') || (ch == ' ')){
   			letStr[k] = ch;
   			k++;
   		}
   		else{
   			return 0;
   		}
   	}
   	numStr[j] = '\0'; 
	letStr[k] = '\0'; 
	return 1;
}