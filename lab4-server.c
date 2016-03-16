// Sever Solution to Lab #4 in CS 306, Fall 2015
// Author: Andrew Kralovec (copyright 2016), Computer Science Dept., Southern Illinois University Carbondale.
//
// Usage: lab4-server
//
// This is a server for the cs306 chat application protocol.
// It creates a separate thread to handle each client as per assignment.
//
// More advanced version:
// Client info is held in unlimited size linked lists.
// Mutexes are used to protect against simultaneous updates of this info.
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <pthread.h>


void *handle_client_in_thnext(void *client_fd_ptr) ; 
int client_count=0 ;  
int fd_array[10]; 
char *client_names[10]; // array for client usernames      



int main(int argc , char *argv[])
{
    int server_sockfd;
    int client_sockfd;
    struct sockaddr_in server;
    // create socket
    server_sockfd = socket(AF_INET , SOCK_STREAM , 0); // IP4v
    if (server_sockfd == -1) {
        printf("Could not create socket");
    }else {
	    printf("Socket created\n");	
	}
     
    // set up the sockaddress 
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons( 9734 );
     
    // bind
    if( bind(server_sockfd,(struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind failed. Error");
        return 1;
    }else {
		printf("bind done\n");
	}
    // listen for connection (5 clients)
    listen(server_sockfd , 5);
    
    printf("\nWaiting for incoming connections...\n");
   

    // for sending messages 
    while(1) {
        client_sockfd = accept(server_sockfd,NULL,NULL);
        // create threads 
        pthread_t tid;
        pthread_create(&tid,NULL,handle_client_in_thnext,&client_sockfd);
        // Message to send back 
        sleep(1);
    }

    return 0;
}

void *handle_client_in_thnext(void *client_fd_ptr) {
    // get the socket descriptor
    int client_fd = *(int*)client_fd_ptr;
    fd_array[client_count++] = client_fd ;  // add socket to fd array
    int next;     
    char client_buffer[4095] ; 
    int i=0; 
    
    // Get username 
	char user_buffer[20] ; 
	int user_count; 
	if((user_count = read(client_fd,user_buffer,200)) > 0){
		//printf("buffer %s  \n",user_buffer);
		client_names[client_count] = user_buffer; 
		//printf("client: %s  \n",client_names[client_count]);
	}
	
	// printf("\n There are: %d number of users \n",client_count);
	char users_message [100];
	int users_length = sprintf(users_message,"There are: %d number of users \n",client_count); 
	write(client_fd, users_message, users_length);
	
	// To many clients , close them out 
	if(client_count > 5){
		char *close_message = "To many clients are connected " ; 
		write(client_fd, close_message, strlen(close_message));
		close(client_fd);
		return NULL ;
	}
	

	// inform clients of new member 
	for(i=0; i < client_count; i++){
		if(fd_array[i] != client_fd){
			char new_client_message [100];
			int new_client_length = sprintf(new_client_message,"%s has joined \n",client_names[client_count]); 
			write(fd_array[i], new_client_message, new_client_length);
		}
	}
    // recive client message and write to input 
    while((next = recv(client_fd,client_buffer,4095,0)) > 0) {
			client_buffer[next]='\0' ; 
			for(i=0; i < client_count; i++){
				if(fd_array[i] != client_fd){
					write(fd_array[i],client_buffer,strlen(client_buffer)); 
				}
			}
		
    }
    
    if(next == 0) {
        printf("\nconnection closed \n");
        client_count--;
        fflush(stdout);
    } else if(next == -1){
        perror("\nrecv failed \n");
        client_count--;
    }
         
    close(client_fd); 
    return NULL;

}
