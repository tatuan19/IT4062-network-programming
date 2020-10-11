#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct _Account {
	char username[30];
	char password[20];
	int status;
	struct _Account *next;
} Account;

Account* root = NULL;
Account* currentAccount = NULL;

Account* createNode(char *username, char* password, int status) {
	Account* newNode = (Account*)malloc(sizeof(Account));
	strcpy(newNode->username, username);
	strcpy(newNode->password, password);
	newNode->status = status;
	newNode->next = NULL;

	return newNode;
}

void insertFirst(char *username, char* password, int status) {
	Account* newNode = createNode(username, password, status);
	if (root == NULL) {
		root = newNode;
	} else {
		newNode-> next = root;
		root = newNode;
	}
}

void loadFromFile(void) {
	char usn[30], pwd[20];
	int stt;

	FILE* fp = fopen("account.txt", "r");

	if (fp == NULL) {
		printf("Error!");   
      	exit(1);
	}

	do {
		fscanf(fp, "%s %s %d", usn, pwd, &stt);
		if (feof(fp)) break;
		insertFirst(usn, pwd, stt);
	} while (1);

	fclose(fp);
}

 Account* lookUp(char* username) {
	Account* currentNode = root;

	while (currentNode != NULL) {
		if (strcmp(username, currentNode->username) == 0)
			return currentNode;
		currentNode = currentNode->next;
	}

	return NULL;
}

void writeToFile(char* username, char* password, int status) {
	FILE* fp = fopen("account.txt", "a");

	fprintf(fp, "%s %s %d\n", username, password, status);

	fclose(fp);
}

void writeToFile2(void) {
	FILE* fp = fopen("account.txt", "w");
	Account* currentNode = root;

	while (currentNode != NULL) {
		fprintf(fp, "%s %s %d\n", currentNode->username, 
			currentNode->password, currentNode->status);
		currentNode = currentNode->next;
	}

	fclose(fp);
}

void signUp(void);
void signIn(void);
void search(void);
void signOut(void);  

void main(int argc, char** argv) {
	int choice;

	loadFromFile();

	while (1) {
		printf("---------------------------------------------\n");
		printf("USER MANAGEMENT PROGRAM\n");
		printf("---------------------------------------------\n");
		printf("1. Register\n");
		printf("2. Sign in\n");
		printf("3. Search\n");
		printf("4. Sign out\n");
		printf("Your choice (1-4, other to quit): ");

		scanf("%d%*c", &choice);

		switch (choice) {
			case 1: signUp(); break;
			case 2: signIn(); break;
			case 3: search(); break;
			case 4: signOut(); break;
			default: exit(0);
		}
	}
}

void signUp(void) {
	char username[30], password[20];	

	printf("Username: "); scanf("%s", username);

	if (!lookUp(username)) {
		printf("Password: "); scanf("%s", password);

		insertFirst(username, password, 1);
		writeToFile(username,password, 1);

		printf("Successful registration\n");
	} else {
		printf("Account existed\n");
	}
}

void signIn(void) {
	char username[30], password[20];	

	printf("Username: "); scanf("%s", username);

	Account* signInAccount  = lookUp(username);
	if (!signInAccount) {
		printf("Cannot find account\n");
	} else if (signInAccount->status == 0) {
		printf("Account is blocked\n");
	} else {
		int count = 0;

		printf("Password: "); scanf("%s", password);

		while (count < 3) {
			if (strcmp(signInAccount->password, password) == 0) {
				printf("Hello %s\n", username);
				currentAccount = signInAccount;
				return;
			} else {
				printf("Password is incorrect\n");
				printf("Please enter password again: "); scanf("%s", password);
				count++;
			}
		}
		printf("Password is incorrect. Account is blocked\n");
		signInAccount->status = 0;
		writeToFile2();
	}
}

void search(void) {
	char username[30], password[20];	

	printf("Username: "); scanf("%s", username);

	if (!currentAccount) 
		printf("Account is not sign in\n");
	else {
		Account* result = lookUp(username);
		if (!result) {
			printf("Cannot find account\n");
		} else {
			printf("Acount is %s\n", result->status == 1 ? "active" : "blocked");
		}
	}
}

void signOut(void) {
	char username[30], password[20];	

	printf("Username: "); scanf("%s", username);

	Account* result = lookUp(username);
	if (!result)
		printf("Cannot find account\n");
	else if ( currentAccount == NULL || strcmp(currentAccount->username, username) != 0) 
		printf("Account is not sign in\n");
	else {
		printf("Goodbye %s\n", result->username);
		currentAccount = NULL;
	}
}
