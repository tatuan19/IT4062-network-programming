#include <stdio.h>          /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define BACKLOG 2   /* Number of allowed connections */
#define BUFF_SIZE 1024
#define SEND_SIZE 1024
#define STORAGE "./storage/"

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
			//receives message from client
			struct stat st;
			char filename[BUFF_SIZE], path[BUFF_SIZE*2];
			FILE *f;

			//create storage if it not exist
			if (stat(STORAGE, &st) == -1) { 
			    mkdir(STORAGE, 0755);
			}

			// receive filename
			bytes_received = recv(conn_sock, recv_data, BUFF_SIZE-1, 0); //blocking
			if (bytes_received <= 0){
				printf("\nConnection closed");
				break;
			}
			recv_data[bytes_received] = '\0';
			printf("\nReceive: File \"%s\"\n", recv_data);

			// check if file exist
			strcpy(path, STORAGE); 
			strcat(path, recv_data);
			if((f = fopen(path, "rb")) != NULL) { 
				bytes_sent = send(conn_sock, "ERR_FILE_EXISTED", strlen("ERR_FILE_EXISTED"), 0);
				if (bytes_sent <= 0){
					printf("\nConnection closed");
					break;
				}
				fclose(f);
				continue;
			}
			// if not exist
			bytes_sent = send(conn_sock, "SUCCESS", strlen("SUCCESS"), 0); 
			if (bytes_sent <= 0){
				printf("\nConnection closed");
				break;
			}
			
			// receive file len
			long flen;
			int errFlag = 0;
			bytes_received = recv(conn_sock, &flen, 20, 0);
			if (bytes_received <= 0){
				printf("\nConnection closed");
				break;
			}
			// printf("Uploaded file name: %s.\n\n", fileName);

			//  receive file content
			int byteReceivedCount = 0;
			char* fileContent;

			f = fopen(path, "wb");
			printf("Receiving...\n");
			while(1) {
				fileContent = (char*) malloc(SEND_SIZE * sizeof(char));
				bytes_received = recv(conn_sock, fileContent, SEND_SIZE, 0);
				if(bytes_received <= 0) {
					printf("\nError!Cannot receive data from client!\n");
					errFlag = 1;
					break;
				}
				byteReceivedCount += bytes_received;
				fwrite(fileContent, bytes_received, 1, f);
				free(fileContent);
				if(byteReceivedCount >= flen) {
					break;
				}
			}
			fclose(f);
			// if (bytes_received <= 0){
			// 	printf("\nConnection closed");
			// 	break;
			// }
			if (errFlag) {
				printf("Interuped!\n");
				bytes_sent = send(conn_sock, "ERR_TRANFER_INTERUPTED", strlen("ERR_TRANFER_INTERUPTED"), 0);
				if (bytes_sent <= 0){
					printf("\nConnection closed");
					break;
				}
				continue;
			}

			printf("Successful!\n");
			bytes_sent = send(conn_sock, "SUCCESS", strlen("SUCCESS"), 0);
			if (bytes_sent <= 0){
				printf("\nConnection closed");
				break;
			}
		}//end conversation
		close(conn_sock);	
	}
	
	close(listen_sock);
	return 0;
}