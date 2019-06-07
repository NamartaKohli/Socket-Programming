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
	ssize_t numBytes;
	socklen_t len;
	char buf[BUF_SIZE];

	sfd = socket(AF_UNIX, SOCK_DGRAM, 0); /* Create server socket */
	if (sfd == -1) {
		printf("socket API failed\n");
		return EXIT_FAILURE;
	}
	/* Construct well-known address and bind server socket to it */
	if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT) {
		printf("remove-%s\n", SV_SOCK_PATH);
		return EXIT_FAILURE;
	}
		

	memset(&svaddr, 0, sizeof(struct sockaddr_un));
	svaddr.sun_family = AF_UNIX;
	strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

	if (bind(sfd, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_un)) == -1) {
		printf("bind API failed\n");
		return EXIT_FAILURE;
	}
	/* Receive messages, convert to uppercase, and return to client */
	for (;;) {
		len = sizeof(struct sockaddr_un);
		numBytes = recvfrom(sfd, buf, BUF_SIZE, 0,
		(struct sockaddr *) &claddr, &len);
		if (numBytes == -1) {
			printf("recvfrom Error\n");
			return EXIT_FAILURE;
		}
		printf("Server received %ld bytes from %s\n", (long) numBytes, claddr.sun_path);
		printf("client address is %s\n", claddr.sun_path);
		for (j = 0; j < numBytes; j++)
			buf[j] = toupper((unsigned char) buf[j]);
		if (sendto(sfd, buf, numBytes, 0, (struct sockaddr *) &claddr, len) != numBytes) {
			printf("sendto error\n");
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}
