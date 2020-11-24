#include <stdio.h>          /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define BACKLOG 2   /* Number of allowed connections */
#define BUFF_SIZE 1024

int checkMessage(char *buff, char *letStr, char *numStr);

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("The number of arguments is incorrect\n");
		return 0;
	} 
 
	int listen_sock, conn_sock; /* file descriptors */
	char recv_data[BUFF_SIZE];
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;
	
	//Step 1: Construct a TCP socket to listen connection request
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}
	
	//Step 2: Bind address to socket
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(atoi(argv[1]));   /* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY puts your IP address automatically */   
	if(bind(listen_sock, (struct sockaddr*)&server, sizeof(server))==-1){ /* calls bind() */
		perror("\nError: ");
		return 0;
	}     
	
	//Step 3: Listen request from client
	if(listen(listen_sock, BACKLOG) == -1){  /* calls listen() */
		perror("\nError: ");
		return 0;
	}
	
	//Step 4: Communicate with client
	while(1){
		//accept request
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock,( struct sockaddr *)&client, &sin_size)) == -1) 
			perror("\nError: ");
  
		printf("You got a connection from %s\n", inet_ntoa(client.sin_addr) ); /* prints client's IP */
		
		//start conversation
		while(1){
			char numStr[100], letStr[100], message[300];

			//receives message from client
			bytes_received = recv(conn_sock, recv_data, BUFF_SIZE-1, 0); //blocking
			if (bytes_received <= 0){
				printf("\nConnection closed");
				break;
			}
			else{
				recv_data[bytes_received] = '\0';
				printf("\nReceive: %s", recv_data);
			}
			
			// TODO
			int result = checkMessage(recv_data,letStr,numStr);
			if (result == 0) {
				printf("Error: Invalid character\n\n");
				strcpy(message, "Error: Invalid character");
				bytes_sent = send(conn_sock, message, strlen(message), 0); /* send to the client welcome message */
				if (bytes_sent <= 0){
					printf("\nConnection closed");
					break;
				}
			} else {
				strcpy(message, "String 1: ");
				strcat(message, letStr);
				strcat(message, "\nString 2: ");
				strcat(message, numStr);
				bytes_sent = send(conn_sock, message, strlen(message), 0); /* send to the client welcome message */
				if (bytes_sent <= 0){
					printf("\nConnection closed");
					break;
				}
			}
		}//end conversation
		close(conn_sock);	
	}
	
	close(listen_sock);
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
