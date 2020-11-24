#include<stdio.h> 
#include<netdb.h>  
#include<sys/socket.h> 
#include<arpa/inet.h>
#include<errno.h>

int main(int argc, char** argv) {
	struct hostent *host_entry; 
   	struct in_addr addr;

	if (argc != 2) {
		printf("The number of arguments is incorrect\n");
		return 0;
	} 
	
    // Convert IP addresses from a dots-and-number string to a struct in_addr
   	if (inet_aton(argv[1], &addr)) { 
    	if ((host_entry = gethostbyaddr(&addr, sizeof(addr), AF_INET)) != NULL) {
    		printf("Official name: %s\nAlias name:\n", host_entry->h_name);
    		for (int i = 0; host_entry->h_aliases[i]; i++)
    			printf("\t%s\n", host_entry->h_aliases[i]);
    	} else {
    		printf("Not found information\n");
    	}
    } else {
    	if ((host_entry = gethostbyname(argv[1])) != NULL) {
    		struct in_addr **addr_list = (struct in_addr **) host_entry->h_addr_list;

    		printf("Official IP: %s\nAlias IP:\n", inet_ntoa(*addr_list[0]));

    		for(int i = 0; addr_list[i] != NULL; i++) {
    			printf("%s ", inet_ntoa(*addr_list[i]));
    		}
    		printf("\n");

    	} else {
    		printf("Not found information\n");   
    	}
    }
    printf("------------------------------\n");
}