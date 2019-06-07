#include <sys/un.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define SV_SOCK_PATH "/tmp/ud_ucase"
#define BUF_SIZE 100

int main(int argc, char *argv[])
{
	struct sockaddr_un svaddr, claddr;
	int sfd, j;
	size_t msgLen;
	ssize_t numBytes;
	char resp[BUF_SIZE];

	/* Create client socket; bind to unique pathname (based on PID) */
	sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sfd == -1) {
		printf("socket API failed\n");
		return EXIT_FAILURE;
	}

	if(argc < 2) {
		printf("please pass agument while execution\n");
		return EXIT_FAILURE;
	}
	
	memset(&claddr, 0, sizeof(struct sockaddr_un));
	claddr.sun_family = AF_UNIX;
	
	printf( "pid is = %ld\n", (long)getpid());

	/*binds the socket to an address, so that the server can send its reply. */
	snprintf(claddr.sun_path, sizeof(claddr.sun_path), "/tmp/ud_ucase_cl.%ld", (long) getpid());
	if (bind(sfd, (struct sockaddr *) &claddr, sizeof(struct sockaddr_un)) == -1) {
		printf("bind API failed fd = %d\n", sfd);
		return EXIT_FAILURE;
	}
	/* Construct address of server */
	memset(&svaddr, 0, sizeof(struct sockaddr_un));
	svaddr.sun_family = AF_UNIX;
	strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

	/* Send messages to server; echo responses on stdout */
	for (j = 1; j < argc; j++) {
		msgLen = strlen(argv[j]); /* May be longer than BUF_SIZE */
		if (sendto(sfd, argv[j], msgLen, 0, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_un)) != msgLen) {
			printf("sendto failed\n");
			return EXIT_FAILURE;
		}
		numBytes = recvfrom(sfd, resp, BUF_SIZE, 0, NULL, NULL);
		if (numBytes == -1) {
			printf("recvfrom failed\n");
			return EXIT_FAILURE;
		}
		printf("Response %d: %.*s\n", j, (int) numBytes, resp);
	}
	remove(claddr.sun_path); /* Remove client socket pathname */
	return EXIT_SUCCESS;
}
