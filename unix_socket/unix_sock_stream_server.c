#include <sys/un.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#define SV_SOCK_PATH "/tmp/us_xfr"
#define BUF_SIZE 100
#define BACKLOG 5

int main(int argc, char *argv[])
{
	struct sockaddr_un addr;
	int sfd, cfd;
	ssize_t numRead;
	char buf[BUF_SIZE];

	/*Create a socket*/
	sfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sfd == -1)
		printf("socket API failed \n");

	/*Remove any existing file with the same pathname as that to which we want to
	bind the socket.*/
	if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT)
		printf("remove-%s", SV_SOCK_PATH);

	/*Construct an address structure for the server’s socket, bind the socket to that
	address, and mark the socket as a listening socket.*/
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;

	strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

	if (bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1)
		printf("bind API failed\n");

	if (listen(sfd, BACKLOG) == -1)
		printf("listen API failed\n");

	for (;;) { 
		/* Handle client connections iteratively */
		/* Accept a connection. The connection is returned on a new
		socket, 'cfd'; the listening socket ('sfd') remains open
		and can be used to accept further connections. */
		cfd = accept(sfd, NULL, NULL);
		if (cfd == -1)
			printf("accept API failed");
		/* Transfer data from connected socket to stdout until EOF */
		while ((numRead = read(cfd, buf, BUF_SIZE)) > 0)
			if (write(STDOUT_FILENO, buf, numRead) != numRead)
				printf("Error :partial/failed write");
		if (numRead == -1)
			printf("Error: read");
		if (close(cfd) == -1)
			printf("close API fialed");
	}	
}
