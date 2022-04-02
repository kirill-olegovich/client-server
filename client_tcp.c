#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define PORT 8080

void connect_to_the_server(int *sock) {
	int connection_status;
	struct sockaddr_in server_address;

	*sock = socket(AF_INET, SOCK_STREAM, 0);

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

	connection_status = connect(*sock, (struct sockaddr *) &server_address, sizeof(server_address));

	if (connection_status == -1) {
		puts("[ Failed to connect to the server ]");
		*sock = 0;
	}
}

int main(void) {
	int sock;
	char command[10];
	char buffer[1024];

	connect_to_the_server(&sock);

	if (sock == 0) return -1;

	printf("> 1 - connect to the server\n> 2 - list my chats\n> 3 - list available people\n> 4 - begging the chat\n> exit - disconnect\n>\n");

	for(;;) {
		printf("> ");
		fgets(command, 10, stdin);
		command[strlen(command) - 1] = '\0';

		send(sock, &command, sizeof(command), 0);
		recv(sock, &buffer, sizeof(buffer), 0);

		printf("%s\n", buffer);

		if (strncmp("exit", command, 4) == 0) {
			puts("Shuting down...");
			break;
		}
	}

	close(sock);

	return 0;
}