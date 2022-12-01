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
#define BUFFER_SIZE 1024
#define INVALID_SOCKET 2147483647

#define INVALID_USER 2147483647
struct user {

    char name[6];
    char password[10];
    char room_id[20];
    int sock;
    bool logged_in;


};
struct room {

    char name[20];
    int num_members;
    struct user *members[10];

};

struct user *find_user(int client_socket, struct user *users){
    for (int i = 0 ; i<MAX_USERS; i++){
        if (users[i].sock ==  client_socket)
            return &(users[i]);
    }
    return NULL;
}

struct user *find_user_name(char *user_name, struct user *users){
    for (int i = 0 ; i<MAX_USERS; i++){
        if (strcmp(users[i].name,user_name)==0)
            return &(users[i]);
    }
    return NULL;
}
void send_to(char *output, int client_socket){
    send(client_socket, output, sizeof(char)*strlen(output) , 0);
}

void send_to_room(char *output, struct room *rm){
    for (int i = 0; i < 10; i ++ ){
        struct user *member = rm->members[i];
        if (member != NULL){
            if (member->sock != INVALID_SOCKET){

                send_to(output,member->sock);
            }
        }

    }
}


/////this fucker down here
int  find_roomw (char *room_name, struct room *rooms){
    //error ("made it");
    for (int i = 0; i < MAX_ROOMS; i ++ ){
        
        if (strcmp(rooms[i].name,room_name)==0){
            
            return i;
        }
    }
    //error ("made it");
    return 100;
}

int  add_to_room (struct user *sender, struct room *rm){
    if (rm->num_members == 10)
        return 0;

    for (int i = 0; i < 10; i ++ ){
        struct user *member = rm->members[i];
        if (member==NULL){
            rm->num_members ++;
            rm->members[i] = sender;
            //sender->room_id = rm->name;
            memcpy(sender->room_id, rm->name, strlen(rm->name)*sizeof(char));

            return 1;
        }
    
    } 
    return 0;

}

void kick_from_room (struct user *sender, struct room *rm){

    for (int i = 0; i < 10; i ++ ){
        struct user *member = rm->members[i];
        if (member==sender){
            rm->num_members --;
            rm->members[i] = NULL;
            char * pp = "INVALID_ROOM";
            bzero(sender->room_id, 20); 
            memcpy(sender->room_id, pp, strlen(pp)*sizeof(char));
            //sender->room_id = "INVALID_ROOM";
            return ;
        }
    
    } 
    return ;
}

int add_room (struct user *sender, char *room_name, struct room *rooms){

 
    for (int i = 0; i < MAX_ROOMS; i ++ ){
        
        if (strcmp(rooms[i].name,"INVALID")==0){
            //bzero(rooms[i].name, 20); 
            strcpy(rooms[i].name, room_name);
            rooms[i].num_members = 1;
            rooms[i].members[0] = sender;
            strcpy(sender->room_id ,room_name);
            return 1;

        }
           
    }
    return 0;
}

void list_of_rooms (  struct user *users, struct room *rooms, int client_socket){
    
    char *tmp1 = "******rooms: ";
    send_to(tmp1, client_socket);
    for (int i = 0; i < MAX_ROOMS; i ++ ){
        if(strcmp(rooms[i].name, "INVALID")!=0){
            send_to(rooms[i].name, client_socket);
            //strcat(output, rooms[i].name);
            //strcat(output, pp);
        }
        
    }
    char *tmp2 = "******users: ";
    send_to(tmp2, client_socket);
    for (int i = 0; i < MAX_USERS; i ++ ){
        if(users[i].logged_in){
            send_to(users[i].name, client_socket);
            //strcat(output, users[i].name);
            //strcat(output, pp);
        }
        
    }
}
int command_parser(char *token, int client_socket){
    //0 = text, not a command
    //1 = login
    //2 = logout
    //3 = joinsession
    //4 = leavesession
    //5 = createsession
    //6 = list
    
    //8 = invalid command

    //if (message[0]!="/")
    //    return 0;
    //perror (message);
    //send_to(message, client_socket); 
    //char * token = strtok(message, " ");
    char * login =  "/login";
    
    if (strcmp(token , login ) == 0 )
        return 1;
    //send_to(token, client_socket); 
    //perror (message);
    if (strcmp(token , "/logout") == 0)
        return 2;

    if (strcmp(token , "/joinsession") == 0)
        return 3;

    if (strcmp(token , "/leavesession") == 0)
        return 4;

    if (strcmp(token , "/createsession") == 0)
        return 5;      

    if (strcmp(token , "/list") == 0)
        return 6;

    if (strcmp(token , "/quit") == 0)
        return 7;

    return 8;
 
}

int receive_message(int client_socket, struct user *users, struct room rooms[]){

    char message[BUFFER_SIZE];
    //char output[BUFFER_SIZE];
    bzero(message,BUFFER_SIZE );
    //printf("received");
    recv(client_socket, message, BUFFER_SIZE, NULL);
    //printf("received: %s", message);
    //send_to(&pp, client_socket); 
    perror (message);
    char * token = strtok(message, " ");
    int command = command_parser(token,client_socket);
    char  pp = (char)('0'+command);
    //perror (&pp);
    struct user *sender =  find_user( client_socket,   users);

     
    
    if (command == 1){//log in, handled a user initiation
        if (sender!= NULL){
            char *output = "you are already logged in";
            send_to(output, client_socket); 
            return 0;
        }
        char * user_name = strtok(NULL, " ");
        //perror (user_name);
        char * password = strtok(NULL, " ");
        //perror (password);
        if (strlen(user_name) == 0 || strlen(password) == 0){
            char *output = "invalid formatting";
            send_to(output, client_socket); 
            return 0;
        }
        sender =  find_user_name( user_name,   users);
        if (sender == NULL){
            char *output = "invalid username";
            send_to(output, client_socket); 
            return 0;
        }
        if (strcmp(sender->password,password)!=0){
            char *output = "invalid password";
            send_to(output, client_socket); 
            return 0;
        }
        sender->sock = client_socket;
        sender->logged_in = true;
        char *output = "You are logged in";
        send_to(output, client_socket); 
        return 0;
    }
    if (sender == NULL){
        char *output = "You are not logged in";
        send_to(output, client_socket); 
        return 0;
    }

    if (command == 0){// send text

        if (sender->logged_in == false){
            char *output = "You are not logged in";
            send_to(output, client_socket); 
            return 0;
            
        }else if (strcmp(sender->room_id , "INVALID_ROOM")==0){
            char *output = "You are not in any room";
            send_to(output, client_socket); 
            return 0;
           
        }else {
            char output[BUFFER_SIZE];
            bzero(output, BUFFER_SIZE); 
            memcpy(output,sender->name, strlen(sender->name)*sizeof(char));
            char * mm = strtok(NULL, " ");
            char * pp = ": ";
            strcat(output, pp);
            strcat(output, mm);
            int ind = find_roomw(sender->room_id, rooms);
            struct room rm = rooms[ind];
            send_to_room(output, &rm);
            
        }
        char *out = "message received";
        send_to(out, client_socket); 
        return 0;

    }else if (command == 2){//logout
        char *output = "you have been logged out";
        send_to(output, client_socket); 
        sender->sock = INVALID_SOCKET;
        char * pp = "INVALID_ROOM";
        bzero(sender->room_id, 20); 
        memcpy(sender->room_id,pp,strlen(pp)*sizeof(char));
        //sender->room_id = "INVALID_ROOM";
        sender->logged_in = false;
        close(client_socket);
        return 1;

    }else if (command == 3){//join session
        struct room rm;
        if (strcmp(sender->room_id , "INVALID_ROOM")!=0){
            rm = rooms[find_roomw (sender->room_id, rooms)];
            kick_from_room (sender, &rm);
        }
     
        char * room_name = strtok(NULL, " ");
        int ind = find_roomw(sender->room_id, rooms);
        
        if (ind == 100){
            char *output = "Invalid room name";
            send_to(output, client_socket); 
            return 0;
        }
        rm = rooms[ind];
        //sender->room_id = rm;
        if (add_to_room (sender, &rm) == 0){
            char *output = "not enough space in room";
            send_to(output, client_socket); 
            return 0;
        }   
        char *output = "you have been added to the room";
        send_to(output, client_socket); 
        return 0;     
    }else if (command == 4){//4 = leavesession


        if (strcmp(sender->room_id , "INVALID_ROOM")==0){
            char *output = "you are not in any room";
            send_to(output, client_socket); 
            return 0;
        }
        char * ppp = malloc(30*sizeof(char));
        bzero(ppp, 30); 
        strcpy(ppp, sender->room_id);
        perror("before find");
        int idx = find_roomw (ppp, rooms);
        perror("after find");
        struct room rm = rooms[idx];
        kick_from_room (sender, &rm);   
        
        
        char *output = "you have been booted from the room";
        send_to(output, client_socket); 
        return 0;     

        
    }else if (command == 5){//5 = createsession
        
        char * room_name = strtok(NULL, " ");
        
        //perror("start");
        int ind = find_roomw(sender->room_id, rooms);
        perror("1");
        if (ind != 100){
            char *output = "room already exists";
            send_to(output, client_socket); 
            return 0;
        }
        //perror("2");
        
        int success = add_room (sender, room_name, rooms);
        //perror("3");
        if (success){
            char *output = "room added";
            send_to(output, client_socket); 
            return 0;
        }
        //perror("4");
        char *output = "server at max number of rooms";
        send_to(output, client_socket); 
        return 0;
        
    }else if (command == 6){//6 = list
        //char output[BUFFER_SIZE];
        list_of_rooms(users,rooms,client_socket);
        //send_to(output, client_socket); 
        return 0;
        
    }else {
        char *output = "invalid command";
        send_to(output, client_socket); 
        return 0;
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
    
    listen(sockfd,10);
    //**********END OF SOCKET SETUP*********//
    //*******************//
    //*******************//



	//*******************//
    //*******************//
    //**********HARD CODED DATABSES*********//
    struct user *users = malloc ( MAX_USERS *  sizeof (struct room));
    char *pp = "1234";
    char *invrm = "INVALID_ROOM";
    for (int i = 0; i < MAX_USERS; i++){
        //char name[6];
        //bzero(users[i].name, 6); 
        sprintf(users[i].name, "user%d", i);
        bzero(users[i].password, 10); 
        strcpy(users[i].password, pp );
        //users[i].password = "1234";
        users[i].sock = INVALID_SOCKET;
        //bzero(users[i].room_id, 20); 
        strcpy(users[i].room_id, invrm );
        //users[i].room_id = "INVALID_ROOM";
        users[i].logged_in = false;
    }
    char *inv = "INVALID";
    struct room *rooms = malloc ( MAX_ROOMS *  sizeof (struct room));
    for (int i = 0; i < MAX_ROOMS; i++){
        //bzero(rooms[i].name, 20);
        strcpy(rooms[i].name, inv);
        //rooms[i].name = "INVALID";
        rooms[i].num_members = 0;
        for (int j = 0; j <10; j++){
            rooms[i].members[j] = NULL;
        }

    }


    //**********END OF HARD CODED DATABSES*********//
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
    printf("Listening at port: %d\n", port);
    while(1){

        int client_sock;
        ready_sockets = current_sockets;

        //printf("Before select\n");
        if (select(FD_SETSIZE, &ready_sockets,NULL,NULL,NULL) < 0){ 
            perror("select failed"); 
            exit(1); 
        } 
        //printf("after\n");
        for (int i = 0; i<FD_SETSIZE; i++){
            if(FD_ISSET(i, &ready_sockets)){
                if(i == sockfd){//i == sockfd
                    //new client
                    socklen_t cli_len = sizeof(cliaddr);
    
                    client_sock = accept(sockfd, (struct sockaddr*)&cliaddr, &cli_len);
                    FD_SET(client_sock,&current_sockets);
                    char *output = "connection accepted";
                    //printf("connection accepted\n");
                    send_to(output, client_sock); 

                }else{
                    //existing client
                    
                    if ( receive_message( i, users, rooms) == 1){
                        FD_CLR(i,&current_sockets);
                    }
                    
                   
                }

            }
        }




		//printf("Listening at port: %d\n", port);
        

		/****************************************Recieve message from cient********************************************/
		//Length of client address
		//socklen_t cli_len = sizeof(cliaddr);
    
        //client_sock = accept(sockfd, (struct sockaddr*)&cliaddr, &cli_len);
        
    
    
    }

	return;
}