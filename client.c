#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <sys/select.h>
#include <pthread.h>
 
//Maximum packet size
#define FRAGMENT_SIZE 1000

void *listner_thread(void *vargp)
{
	int sockfd = *((int*)vargp);
	char buffer[1024];
	while(1){
			bzero(buffer, 1024);
			recv(sockfd, buffer, sizeof(buffer), 0);
			if (strlen(buffer)>1)
				printf("Server: %s\n", buffer);
			if (strcmp(buffer, "you have been logged out")==0)
				return NULL;
	}
}





void main(int argc, char const * argv[]){
    char input[100];
    bzero(input, 100);
	char * deli = " ";
    while(1){
        printf("Dont forget to log in\n");
		fgets(input, 100, stdin);
        //scanf("%s",input);
		char* rest = input;
		printf("input: %s\n",input);
        char *command = strtok_r  (rest, " ", &rest);
		//printf("command: %s\n",command);
		char *name = strtok_r  (rest, " ", &rest);
		printf("name: %s\n",name);
		char *password = strtok_r  (rest, " ", &rest);
		printf("pass: %s\n",password);
		char *s_addr = strtok_r  (rest, " ", &rest);
		printf("address: %s\n",s_addr);
		char *port = strtok_r  (rest, " ", &rest);
		if (strcmp(command, "/login")!= 0){
			printf("you need to log in\n");
		}else{
			
			
			printf("port: %s\n",port);
			struct sockaddr_in servaddr;
			struct addrinfo *servinfo;
			struct addrinfo hints;

			memset(&servaddr, 0, sizeof(servaddr));  
			memset(&servinfo, 0, sizeof(servinfo)); 
			memset(&hints, 0 , sizeof (hints));

			//Server settings
			servaddr.sin_family = AF_INET; 
			servaddr.sin_addr.s_addr = inet_addr(s_addr); 
			servaddr.sin_port = htons(atoi(port));

			//Hints
			
			// /login user0 1234 128.100.13.170 11313147
			// /login user1 1234 128.100.13.170 11313147
			/****************************************Server Connection********************************************/
			//Length of client address
			int sockfd = 0; 
			socklen_t ser_len = sizeof(servaddr);
			sockfd = socket(AF_INET, SOCK_STREAM, 0);

			//Check if socket creation was succesful
			if (sockfd < 0){
				perror("socket creation failed"); 
				exit(1);
			} 
			char buffer[1024];
			
			bzero(buffer, 1024);
			connect(sockfd, (struct sockaddr*)&servaddr,ser_len);
			printf("Connected to the server.\n");
			bzero(buffer, 1024);
			recv(sockfd, buffer, sizeof(buffer), 0);
			printf("Server: %s\n", buffer);

			bzero(buffer, 1024);
			char * space = " ";
			char *starter = "/login ";
			memcpy(buffer, starter, strlen(starter)*sizeof(char));
			strcat(buffer, name );
			strcat(buffer, space );
			strcat(buffer, password );
			printf("sending: %s\n", buffer);
			send(sockfd, buffer, strlen(buffer), 0);

			bzero(buffer, 1024);
			recv(sockfd, buffer, sizeof(buffer), 0);
			printf("Server: %s\n", buffer);

			

			if (strcmp(buffer, "You are logged in")==0){
				pthread_t listener;
				pthread_create(&listener, NULL, listner_thread, (void *)&sockfd);
				while(1){
					bzero(buffer, 1024);
					
					fgets(buffer, 100, stdin);
					 if (strncmp(buffer,"/quit", 5 )==0){
						char * tp = "/logout ";
						send(sockfd, tp, strlen(tp), 0);
						pthread_cancel(listener);
						return;
					 }
					 printf("sending: %s\n", buffer);
					
					if (strncmp(buffer,"/logout \n" ,7)==0){
						
						pthread_cancel(listener);
						send(sockfd, buffer, strlen(buffer), 0);
						break;
						
						
					}
					send(sockfd, buffer, strlen(buffer), 0);
				}

			}


		}


    }
	

	return;
}