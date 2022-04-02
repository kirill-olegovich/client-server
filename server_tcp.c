#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <mysql/mysql.h>

#define PORT 8080


int create_a_socket(int *sock) {
	int bind_status;
	struct sockaddr_in server_address;

	*sock = socket(AF_INET, SOCK_STREAM, 0);

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

	bind_status = bind(*sock, (struct sockaddr *) &server_address, sizeof(server_address));

	if (bind_status == -1) {
		puts("[ Failed to create a socket ]");
		return -1;
	}

	puts("[ Successfully created a socket ]");

	return 0;
}


int listen_for_connection(int sock, int *client_sock) {
	if ((listen(sock, 5)) != 0) {
		puts("[ Failed to start listening ]");
		return -1;
	}

	*client_sock = accept(sock, NULL, NULL);

	if (*client_sock < 0) {
		puts("[ Failed to accept a client ]");
		return -1;
	}

	puts("[ Successfully assepted the client ]");
}


int connect_to_db(MYSQL *db_conn) {
	char db_name[] = "app";
	char db_user[] = "app_user";
	char db_pass[] = "123456789";

	if (mysql_real_connect(db_conn, "localhost", db_user, db_pass, db_name, 0, NULL, 0) == NULL) {
		puts("[ Failed to connect to a database ]");
		fprintf(stderr, "%s\n", mysql_error(db_conn));
		return -1;
	}

	puts("[ Successfully connected to the database ]");

	return 0;
}


int send_query_to_db(MYSQL *db_conn) { // <----------------
	char buffer[64];

	sprintf(buffer, "select * from users", NULL);

	// mysql_query(db_conn, buff);

	if (mysql_query(db_conn, buffer) != 0) {
		puts("[ Failed to send a query ]");
		fprintf(stderr, "%s\n", mysql_error(db_conn));
		return -1;
	}

	puts("[ Successfully sent a query ]");

	return 0;
}


void create_buffer_from_query_result(MYSQL *db_conn, char *buffer) {
	MYSQL_RES *result;
	MYSQL_ROW row;
	int num_columns;

	result = mysql_store_result(db_conn);
	num_columns = mysql_num_fields(result);

	while ((row = mysql_fetch_row(result))) {
		for (int i=0; i<num_columns; i++) {
			strncat(buffer, row[i], sizeof(row[i]));
			strncat(buffer, " ", sizeof(" "));
		}
		strncat(buffer, "\n", sizeof("\n"));
	}

	mysql_free_result(result);
}


int main(void) {
	MYSQL *db_conn;
	int sock, client_sock;
	char buffer[1024];

	if (create_a_socket(&sock) == -1) return -1;	// Socket

	db_conn = mysql_init(NULL);						// MySQL
	if (connect_to_db(db_conn) == -1) return -1;	// database

	if (listen_for_connection(sock, &client_sock) == -1) return -1;	// Listening the clients

	// Main loop
	for (;;) {
		recv(client_sock, &buffer, sizeof(buffer), 0);

		if (strncmp("exit", buffer, 4) == 0) {
			bzero(&buffer, sizeof(buffer));
			puts("Shuting down...");
			break;

		} else if (strncmp("1", buffer, 1) == 0) {
			bzero(&buffer, sizeof(buffer));

			if (send_query_to_db(db_conn) == 0) {
				create_buffer_from_query_result(db_conn, &buffer);
			}
		}	

		send(client_sock, &buffer, sizeof(buffer), 0);
	}

	close(sock);
	mysql_close(db_conn);

	return 0;
}
