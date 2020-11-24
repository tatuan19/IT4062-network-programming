#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 8192
#define ERR_FILE_EXISTED "Error: File is existent on server"
#define ERR_TRANFER_INTERUPTED "Error: File tranfering is interupted"
#define SUCCESS "Successful transfering"
#define SEND_SIZE 1024

void getFileName(char* filename, char* path);

int main(int argc, char** argv) {
	if (argc != 3) {
		printf("The number of arguments is incorrect\n");
		return 0;
	} 

	int client_sock;
	char buff[BUFF_SIZE];
	struct sockaddr_in server_addr; /* server's address information */
	int bytes_sent, bytes_received;
	char path[BUFF_SIZE], filename[BUFF_SIZE];
    FILE *f;

	//Step 1: Construct socket
	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	//Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	
	//Step 3: Request to connect server
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}
		
	//Step 4: Communicate with server			
	while (1) {
		// read file path from keyboard
		printf("\nInsert file's path: ");
		memset(path,'\0',(strlen(path)+1));
		fgets(path, BUFF_SIZE, stdin);
		if (path[0] == '\n') break;
		path[strlen(path)-1] = '\0';
		
		// strcpy(path, "/home/tatuan/IT4062-network-programming/HW4/test/abc.txt");
		
		// read file
		if ((f = fopen(path, "rb")) == NULL){
            printf("Error: File not found\n");
            continue;
            // return 0;
            // chuong trinh chi dung lai trong truong hop xau rong.
        }

        //send file name
        getFileName(filename, path);
        // printf("%s\n", filename);
		bytes_sent = send(client_sock, filename, strlen(filename), 0);
		if(bytes_sent <= 0){
			printf("\nConnection closed!\n");
			break;
		} else {
			bytes_received = recv(client_sock, buff, BUFF_SIZE-1, 0);
			if(bytes_received <= 0){
				printf("\nError!Cannot receive data from sever!\n");
				break;
			}
			buff[bytes_received] = '\0';
			if(strcmp(buff, "ERR_FILE_EXISTED") == 0) {
	            printf("%s\n", ERR_FILE_EXISTED);
	            continue;
      		}
      		// printf("%s\n", buff);
		}

		// begin read file
        fseek(f, 0, SEEK_END);	// Jump to the end of the file
        long flen = ftell(f);	// Get size of file       
        rewind(f);				// pointer to start of file	

 		//send file size
 		printf("File's Size: %d\n", flen);
        bytes_sent = send(client_sock, &flen, 20, 0);
        if(bytes_sent <= 0){
            printf("\nConnection closed!\n");
            break;
        }

        // send file
        int byteSent = 0;		// the number of byte had been sent
        int byteWillSend;		// the number of byte will have been sent

        printf("Tranfering...\n");
        while(1) {
            int byteWillSend = SEND_SIZE;
            // if over file size then adjust byteWillSend
            if((byteSent + SEND_SIZE) > flen) {
                byteWillSend = flen - byteSent; 
            }
            char* buffer = (char *) malloc((byteWillSend) * sizeof(char));
            fread(buffer, byteWillSend, 1, f); // read buffer with size 
            byteSent += byteWillSend; //increase byte send
            bytes_sent = send(client_sock, buffer, byteWillSend, 0);
            if(bytes_sent <= 0){
                printf("\nConnection closed!asdsad\n");
                break;
            }
            free(buffer);
            if(byteSent >= flen) {
            	fclose(f);
         		break;
            }
        }

        // recv result
        bytes_received = recv(client_sock, buff, BUFF_SIZE-1, 0); 
        if(bytes_received <= 0){
            printf("\nError!Cannot receive data from sever!\n");
            break;
        }
        //print result
        if(strcmp(buff, "SUCCESS") == 0) {
        	printf("%s\n", SUCCESS);
        } else {
        	printf("%s\n", ERR_TRANFER_INTERUPTED);
        }
	}

	//Step 4: Close socket
	close(client_sock);
	return 0;
}

void getFileName(char* filename, char* path) {
	int i;
	for (i = strlen(path)-1; i >= 0 && path[i] != '/'; i--); 
	strcpy(filename, &(path[i+1]));
}