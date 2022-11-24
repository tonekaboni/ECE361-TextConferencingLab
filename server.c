#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <sys/select.h>

#define MAX_USERS 10
#define MAX_ROOMS 10
#define BUFFER_SIZE 2048
#define INVALID_SOCKET 2147483647
#define INVALID_ROOM 2147483647
struct user {

    char* name;
    char* password;
    int room_id;
    int socket;
    bool logged_in;


};
struct room {

    char* name;
    int num_members;
    struct user *members[10];

};

struct user *find_user(int client_socket, struct user *users){

}

void send_to(char *output, int client_socket){

}

void send_to_room(char *output, struct room *room){

}



int command_parser(char *message){
    //0 = text, not a command
    //1 = login
    //2 = logout
    //3 = joinsession
    //4 = leavesession
    //5 = createsession
    //6 = list
    //6 = quit
    //8 = invalid command

    if (message[0]!="/")
        return 0;
    
    if (strncmp(message , "/login", 6) == 0)
        return 1;
    
    if (strncmp(message , "/logout", 7) == 0)
        return 2;

    if (strncmp(message , "/joinsession", 12) == 0)
        return 3;

    if (strncmp(message , "/leavesession", 13) == 0)
        return 4;

    if (strncmp(message , "/createsession", 14) == 0)
        return 5;

    if (strncmp(message , "/list", 5) == 0)
        return 6;

    if (strncmp(message , "/quit", 5) == 0)
        return 7;

    return 8;
 
}

int receive_message(int client_socket, struct user *users, struct room **rooms){

    char message[BUFFER_SIZE];
    char output[BUFFER_SIZE];
    bzero(buff_out, BUFFER_SZ);

    recv(client_socket, message, BUFFER_SIZE, 0);
    int command = command_parser(message);

    struct user *sender =  find_user( client_socket,   users);

    if (sender == NULL){
        printf("user_list incomplete");
        return 0;
    }
    



    if (command == 0){// send text

        if (sender->logged_in == FALSE){
            output = "You are not logged in";
            send_to(output, client_socket); 
            
        }else if (sender->room_id == INVALID_ROOM){
            output = "You are not in any room";
            send_to(output, client_socket); 
           
        }else {
            memcpy(output,sender->name, sizeof(sender->name));
            strcat(output, message);
            send_to_room(output, rooms[sender->room_id]);
            
        }
        return 0;

    }else if (command == 1){//log in, handled un user initiation
        output = "invalid command";
        send_to(output, client_socket); 
        return 0;
        
    }else if (command == 2){//logout
        output = "you have been logged out";
        send_to(output, client_socket); 
        sender->socket = INVALID_SOCKET;
        sender->room = INVALID_ROOM;
        sender->logged_in = FALSE;
        close(client_socket);
        return 1

    }else if (command == 3){//join session
        
    }else if (command == 4){
        
    }else if (command == 5){
        
    }else if (command == 6){
        
    }else if (command == 7){
        
    }else if (command == 8){
        output = "invalid command";
        send_to(output, client_socket); 
        return;
    }

    

}
















void main(int argc, char const * argv[]){

	//*******************//
    //*******************//
    //**********SOCKET SETUP*********//

	if (argc != 2) {
		printf("Incorrect usage.\nUsage: server <server port num>\n");
		exit(1);
  	}

	//Read port argument
	int port = 0;
	printf("Port: %d \n", atoi(argv[1]));
	port = atoi(argv[1]);

	
	int sockfd = 0; 
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	
	if (sockfd < 0){
		perror("socket creation failed"); 
        exit(1);
    } 

    struct sockaddr_in servaddr, cliaddr;
	memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 

    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(port);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) { 
        perror("Socket bind failed"); 
        exit(1); 
    } 
    
    listen(sockfd,10)
    //**********END OF SOCKET SETUP*********//
    //*******************//
    //*******************//


	//*******************//
    //*******************//
    //**********SELECT SETUP*********//
    fd_set current_sockets, ready_sockets;
    FD_ZERO(&current_sockets);
    FD_SET(sockfd, &current_sockets);
    //**********END OF SELECT SETUP*********//
    //*******************//
    //*******************//

    while(1){


        ready_sockets = current_sockets;


        if (select(FD_SETSIZE, &ready_sockets,NULL,NULL,NULL) < 0){ 
            perror("select failed"); 
            exit(1); 
        } 

        for (int i = 0; i<FD_SETSIZE; i++){
            if(FD_ISSET(i, &ready_sockets)){
                if(i == sockfd){
                    //new client
                    socklen_t cli_len = sizeof(cliaddr);
    
                    client_sock = accept(sockfd, (struct sockaddr*)&cliaddr, &cli_len);
                    FD_SET(client_sock,&current_sockets);

                }else{
                    //existing client

                    if (rec function returns 1){
                        FD_CLR(i,&current_sockets);
                    }
                }

            }
        }




		printf("Listening at port: %d\n", port);
        

		/****************************************Recieve message from cient********************************************/
		//Length of client address
		socklen_t cli_len = sizeof(cliaddr);
    
        client_sock = accept(sockfd, (struct sockaddr*)&cliaddr, &cli_len);
        
    
    
    }

	return;
}
