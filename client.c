// Client Solution to Lab #4 in CS 306, Fall 2015
// Author: Andrew Kralovec (copyright 2016), Computer Science Dept., Southern Illinois University Carbondale.
//
// Usage: lab4-client SERVER_IP_ADDRESS USERNAME
//
// This is a clientfor the cs306 chat application protocol.
// As specified in the assignment, it uses two processes to handle I/O betwee
// the user and the server: one process to read terminal input and send to server,
// a second (child process) to read messages relayed from server and print out.
//
// This is a basic client version that may not work cleanly with errors and unexpected
// closure of the connection and termination of the child process.
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

int main(int argc, char *argv[]) {

	int socketfd;
	struct sockaddr_in address;
		
	 if(argc != 3){
        printf("\n Usage: %s <ip of server> <username> \n",argv[0]);
        return 1;
    }

	char *server_ip = argv[1];
	char *user_name = argv[2]; 

	socketfd = socket(AF_INET, SOCK_STREAM, 0);

	// Standard to zero out address before setting:
	memset(&address, 0, sizeof(address));

	address.sin_family = AF_INET;

	// Here we will use inet_aton() since inet_addr() is deprecated:
	inet_aton(server_ip,&address.sin_addr);
	address.sin_port = htons(9734);

	if (connect(socketfd, (struct sockaddr *)&address, sizeof(address)) == -1) {
		perror("Error could not connect socket/n");
		exit(EXIT_FAILURE);
	}
	
	//Create child running function:
	pid_t child;
	if ((child = fork()) == 0) {
		//recive message
		char input_buffer[4095];
		int next ; 
		while ((next = read(socketfd,input_buffer,4095)) > 0) {
			//input_buffer[next]='\0';
			write(1, input_buffer, next);
		}
	}
	else if (child == -1){
		exit(EXIT_FAILURE);
	}
	//Back in parent:
	// Message to send back 
	sleep(1);
	// init by writing user name
	write(socketfd, user_name, strlen(user_name));
	while(1){
		char buffer[4095] ;
		int next; 
		while((next = read(0,buffer,4095)) > 0 ) {
			buffer[next]='\0' ; 
			//int newSize = 4095+strlen(user_name)+1 ; 
			//strcat(buffer,user_name); // Combine two strings 
			  char temp_buffer [4096];
			  int help = sprintf(temp_buffer,"%s : %s",user_name,buffer); 
			  write(socketfd, temp_buffer, help);
		}
	}
	close(socketfd);

	exit(EXIT_SUCCESS);
	
	return 0; 
}

