#include <stdio.h>          /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "linkedList.h"

#define BACKLOG 2   /* Number of allowed connections */
#define BUFF_SIZE 1024

int checkAccount(char *username, char *password, Node *root);
Node* lookUp(char* username, Node *root);
void blockAccount(char *username, Node *root);

void loadFromFile(Node **root) {
	Account temp;
	FILE* fp = fopen("account.txt", "r");

	if (fp == NULL) {
		printf("Error!");   
      	exit(1);
	}

	do {
		fscanf(fp, "%s %s %d", temp.username, temp.password, &temp.status);
		if (feof(fp)) break;
		insertFirst(root,temp);
	} while (1);

	fclose(fp);
}

void writeToFile(Node *root) {
	FILE* fp = fopen("account.txt", "w");
	Node* currentNode = root;

	while (currentNode != NULL) {
		fprintf(fp, "%s %s %d\n", currentNode->acc.username, 
			currentNode->acc.password, currentNode->acc.status);
		currentNode = currentNode->next;
	}

	fclose(fp);
}

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
	int pid;
	
	// Step 3.5: Innitial data 
	Node * root = NULL;
	loadFromFile(&root);
	// printLinkedList(root);

	//Step 4: Communicate with client
	while(1){
		//accept request
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock,( struct sockaddr *)&client, &sin_size)) == -1) 
			perror("\nError: ");
  
		printf("You got a connection from %s\n", inet_ntoa(client.sin_addr) ); /* prints client's IP */
		
		//start conversation
		pid = fork();

		if (pid == 0) {
        	//child process
        	while(1){
        		char username[BUFF_SIZE];
                char password[BUFF_SIZE];
                memset(username, 0, strlen(username));
                memset(password, 0, strlen(password));

                //receives username from client
                recv(conn_sock, username, BUFF_SIZE, 0);
                // printf("Username: %s\n", username);

                //receives password from client
                recv(conn_sock, password, BUFF_SIZE, 0);
                // printf("Password: %s\n", password);
                
                int re = checkAccount(username, password, root);
                // printf("%d\n", re);

                if(re == -2){
                    send(conn_sock, "ERR_ACCOUNT_NOT_EXIST", sizeof("ERR_ACCOUNT_NOT_EXIST"), 0);
                }
                if(re == -1){
                    send(conn_sock, "ERR_ACCOUNT_BLOCKED", sizeof("ERR_ACCOUNT_BLOCKED"), 0);
                }

                if (re == 0) {
					int count = 0;
					int success = 0;
					while (1) {
						if (checkAccount(username, password, root) == 1) {
							// printf("Hello %s\n", username);
							send(conn_sock, "SUCCESS", sizeof("SUCCESS"), 0);
							success = 1;
							break;
						} else {
							// printf("%s\n", ERR_PASSWORD_INCORRECT);
							send(conn_sock, "ERR_PASSWORD_INCORRECT", sizeof("ERR_PASSWORD_INCORRECT"), 0);
							count++;
							if (count > 3) break;
							// re-receive password
							recv(conn_sock, password, BUFF_SIZE, 0);
						}
					}
					if (!success) {
						// printf("Password is incorrect. Account is blocked\n");
						// block accout
						blockAccount(username, root);
						writeToFile(root);
						printLinkedList(root);
					}
				}
				if(re == 1){
                    send(conn_sock, "SUCCESS", sizeof("SUCCESS"), 0);
                }
        	}
        }
		//end conversation
		close(conn_sock);	
	}
	close(listen_sock);
	return 0;
}

int checkAccount(char *username, char *password, Node *root) {
    Node* signInAccount  = lookUp(username, root);

    if(signInAccount != NULL) {
        if(signInAccount->acc.status == 0) 
        	return -1;	// block
        else if(strcmp(signInAccount->acc.password,password)==0) 
        	return 1;	// ok
        else 
        	return 0;	// sai pass
    } else 
    	return -2;	// not exist
}

Node* lookUp(char* username, Node *root) {
	Node* currentNode = root;

	while (currentNode != NULL) {
		if (strcmp(username, currentNode->acc.username) == 0)
			return currentNode;
		currentNode = currentNode->next;
	}

	return NULL;
}

void blockAccount(char *username, Node *root) {
	Node* signInAccount  = lookUp(username, root);
	signInAccount->acc.status = 0;
}