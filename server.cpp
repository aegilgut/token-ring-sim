#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include<iostream>

using namespace std;


void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{

	int sockfd;
	int newsockfd;
	int portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	if (argc < 2) {
		fprintf(stderr, "ERROR, no port provided\n");
		exit(1);
	}
	// create a socket
	// socket(int domain, int type, int protocol)
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");

	// clear address structure
	bzero((char *)&serv_addr, sizeof(serv_addr));

	portno = atoi(argv[1]);

	//server byte order
	serv_addr.sin_family = AF_INET;

	// Host address
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	//network byte order
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");

	listen(sockfd, 5);

	// The accept() call actually accepts an incoming connection
	// clilen = sizeof(cli_addr); accepts a new socket so old socket
	// can be resued for new connections 

	newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
		error("ERROR on accept");
 
	string variable;
	while (cin >> variable)
		send(newsockfd, variable.c_str(), 255, 0);

     // This send() function sends the variable over

     bzero(buffer,256);

     close(newsockfd);
     close(sockfd);
     return 0; 
}