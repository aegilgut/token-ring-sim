#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace std;

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[256] = { 0 };
	if (argc < 3) {
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(0);
	}
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR connecting");
	bzero(buffer, 256);
	fgets(buffer, 255, stdin);
	bzero(buffer, 256);
	
	close(sockfd);
	
	/////////////////
	int NO_OF_CHILDREN;
	NO_OF_CHILDREN = atoi(buffer);
	int pipefd[NO_OF_CHILDREN + 1][2];
	int input[50];
	int pid;
	int i;
	int token;
	int c;
	int temp;
	temp = read(sockfd, buffer, 256);
	NO_OF_CHILDREN = temp;
	while (temp != -1) {
		temp = read(sockfd, buffer, 256);
		input[i] = temp;
		i++;
	}
	input[i] = -1;

	for (i = 0; i <= NO_OF_CHILDREN; i++)
	{
		if (pipe(pipefd[i]) < 0)
		{
			printf("Pipe Creation failed\n");
			exit(1);
		}
	}

	printf("Pipes created\n");

	for (i = 0; i < NO_OF_CHILDREN; i++)
	{

		if ((pid = fork()) == 0)
		{
			while (1)
			{
				read(pipefd[i][0], &token, sizeof(token));
				if (token >= 0 && input[token] == 1)
					printf("Process %d is sending information\n", i + 1);
				if (token == -1 || input[token] == -1)
				{
					printf("Ending\n");
					token = -1;
					write(pipefd[i + 1][1], &token, sizeof(token));
					_exit(0);
				}
				token = token + 1;
				write(pipefd[i + 1][1], &token, sizeof(token));
			}

		}
		//These ends are no longer needed in parent.
		close(pipefd[i][0]);
		close(pipefd[i + 1][1]);
	}

	token = 0;
	while (1)
	{
		//Write to the pipe which is connected to the first child.
		if (input[token] == 1)
			printf("Process 0 is sending information\n");
		token = token + 1;
		write(pipefd[0][1], &token, sizeof(int));
		//Read from the pipe, which is attached to the last child created
		read(pipefd[NO_OF_CHILDREN][0], &token, sizeof(int));
		if (token == -1 || input[token] == -1)
		{
			token = -1;
			printf("Parent breaking\n");
			write(pipefd[0][1], &token, sizeof(int));
			break;
		}
	}
	return 0;
}
