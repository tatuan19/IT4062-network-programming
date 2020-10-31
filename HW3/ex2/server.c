#include <stdio.h>          /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>  
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 1024

void resolver(char *buff, char *result);

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
		char message[300];

		sin_size=sizeof(struct sockaddr_in);

		bytes_received = recvfrom(server_sock, buff, BUFF_SIZE-1, 0, (struct sockaddr *) &client, &sin_size);

		if (bytes_received < 0)
			perror("\nError: ");
		else {
			buff[bytes_received] = '\0';
			// TODO
			// printf("[%s:%d]: %s", inet_ntoa(client.sin_addr), ntohs(client.sin_port), buff);
			
			resolver(buff, message);
			sendto(server_sock, message, strlen(message), MSG_CONFIRM, (const struct sockaddr *) &client, sizeof(client));
		}
					
	}

	close(server_sock);
	return 0;
}

void resolver(char *buff, char *result) {
	struct hostent *host_entry; 
   	struct in_addr addr;

   	printf("%s\n", buff);
	if (inet_aton(buff, &addr)) {
    	if ((host_entry = gethostbyaddr(&addr, sizeof(addr), AF_INET)) != NULL) {
    		// printf("Official name: %s\nAlias name:\n", host_entry->h_name);
    		strcpy(result, "Official name: ");
    		strcat(result, host_entry->h_name);
    		strcat(result, "\nAlias name:\n");
    		for (int i = 0; host_entry->h_aliases[i]; i++) {
    			// printf("\t%s\n", host_entry->h_aliases[i]);
    			strcat(result, host_entry->h_aliases[i]);
    			strcat(result,"\n");
    		}
    	} else {
    		strcpy(result, "Not found information\n");
    	}
    } else {
    	if ((host_entry = gethostbyname(buff)) != NULL) {
    		struct in_addr **addr_list = (struct in_addr **) host_entry->h_addr_list;

    		// printf("Official IP: %s\nAlias IP:\n", inet_ntoa(*addr_list[0]));
    		strcpy(result, "Official IP: ");
    		strcat(result, inet_ntoa(*addr_list[0]));
    		strcat(result, "\nAlias IP:\n");
    		for(int i = 0; addr_list[i] != NULL; i++) {
    			// printf("%s ", inet_ntoa(*addr_list[i]));
    			strcat(result, inet_ntoa(*addr_list[i]));
    		}
    		// printf("\n");
    		strcat(result,"\n");
    	} else {
    		strcpy(result, "Not found information\n");
    	}
    }
}