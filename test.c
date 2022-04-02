#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void change_buffer(char *buffer) {
	for (int i=0; i<10; i++) strncat(buffer, "2", sizeof("2"));
	strncat(buffer, "\n", sizeof("\n"));
}


int main(void) {
	char buffer[1024] = "1";

	change_buffer(buffer);

	printf("%s", buffer);

	return 0;
}