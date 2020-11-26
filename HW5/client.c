#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 8192
#define ERR_PASSWORD_INCORRECT "Password is incorrect"
#define ERR_ACCOUNT_NOT_EXIST "Account is not existed"
#define ERR_ACCOUNT_BLOCKED "Account is blocked"

typedef struct storage {
	char username[30];
} Storage;
Storage *localStorage;


int main(int argc, char** argv) {
	if (argc != 3) {
		printf("The number of arguments is incorrect\n");
		return 0;
	}

	int client_sock;
	char buff[BUFF_SIZE];
	struct sockaddr_in server_addr; /* server's address information */
	int bytes_sent, bytes_received;
	
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
	localStorage = (Storage *) malloc(sizeof(Storage));
	
	while (1) {
		int choice;
		char username[BUFF_SIZE], password[BUFF_SIZE], message[BUFF_SIZE];
		int bytes_received;

		printf("---------------------------------------------\n");
		printf("1. Sign in\n");
		printf("2. Sign out\n");
		printf("Your choice (1-2, other to quit): ");

		scanf("%d%*c", &choice);

		switch (choice) {
			case 1:
				if (localStorage->username[0] != '\0') {
					printf("You have already logged in\n");
					break;
				}
				//send username to server
				printf("Username: ");
				memset(username, '\0', (strlen(username) + 1));
				fgets(username, BUFF_SIZE, stdin);
				username[strlen(username)-1]='\0';
				send(client_sock, username, strlen(username), 0);

				//send password to server
				printf("Password: ");
				memset(password, '\0', (strlen(password) + 1));
				fgets(password, BUFF_SIZE, stdin);
				password[strlen(password)-1]='\0';
				send(client_sock, password, strlen(password), 0);

				//receive login status
				bytes_received = recv(client_sock, message, BUFF_SIZE - 1, 0);
				if (bytes_received <= 0) {
					printf("\nError!Cannot receive data from sever!\n");
					return 0;
				}
				message[bytes_received] = '\0';
    			// printf("%s\n", message);

				if (strcmp(message,"ERR_ACCOUNT_NOT_EXIST") == 0) {
					printf("%s\n", ERR_ACCOUNT_NOT_EXIST);
				} else if (strcmp(message,"ERR_ACCOUNT_BLOCKED") == 0) {
					printf("%s\n", ERR_ACCOUNT_BLOCKED);
				} else {
					int count = 0;
					int success = 0;
					while (1) {
						if (strcmp(message, "SUCCESS") == 0) {
							printf("Hello %s\n", username);
							strcpy(localStorage->username, username);
							// printf("%s\n", localStorage->username);
							success = 1;
							break;
						} else {
							printf("%s\n", ERR_PASSWORD_INCORRECT);
							count++;
							if (count > 3) break;
							printf("Please enter password again: ");
							memset(password, '\0', (strlen(password) + 1));
							fgets(password, BUFF_SIZE, stdin);
							password[strlen(password)-1]='\0';
	    					// re-send password
							send(client_sock, password, strlen(password), 0);
							bytes_received = recv(client_sock, message, BUFF_SIZE - 1, 0);
							if (bytes_received <= 0) {
								printf("\nError!Cannot receive data from sever!\n");
								return 0;
							}
						}
					}
					if (!success) {
						printf("Account is blocked\n");
					}
				}
				break;
			case 2: 	
				// printf("%s\n", localStorage->username);
				if (localStorage->username[0] == '\0') {
					printf("Please log in first\n");
				}
				else {
					printf("Goodbye %s\n", localStorage->username);
					strcpy(localStorage->username, "");
				}
				break;
			default: goto QUIT;
		}
	}

	QUIT:
	free(localStorage);

	//Step 4: Close socket
	close(client_sock);
	return 0;
}