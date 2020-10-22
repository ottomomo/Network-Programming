/*
 * @author: Octavio Sales Calvo, id:902769
 * @brief: client.c: client UDP calculator using socket.h, the server allows 3 different roles of client. client 1 gives the operand, 
 * client 2 gives the operator and client 3 get the result of the operation.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>

#define buffersize 16
#define serviceport 25559

int main(int argc, char *argv[]){
    
    if (argc < 2) {
		printf("Use: ./client role [arguments]\n");
		return -1;
	}
 
    // SOCKET TCP
    int sock;
    if((sock = socket(AF_INET, SOCK_STREAM, 0))<0){
        perror("socket failed");
    }

    // SERVER ADDRESS
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;  
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // should i write loopback or INADDR_ANY????????????????
    serv_addr.sin_port = htons(serviceport); 
    
    //CONECTION
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    
    if(argc<3){
        printf("Use: ./client op1 op2 operator[1='+', 2='-', 3='*', 4='/']\n");
        return -1;
    } 
    int op1=(int) atoi(argv[1]);
    int op2=(int) atoi(argv[2]);
    if((op1<0 || op1>9) || (op2<0 || op2>9)){
        printf("Error: operands must be an positve integer and only one digite [0-9]\n");
        return -1;
    }
    char *message = (char*) calloc(buffersize, sizeof(char)) ;
    strncpy(message, argv[1],16) ;
    // send the operand 1 to the server
    send(sock, message, buffersize, 0);
    
    message = (char*) calloc(strlen(argv[2])+1, sizeof(char)) ;
    strncpy(message, argv[2], 16) ;
    // send the operand 2 to the server
    send(sock, message, buffersize, 0);
    printf("Operands sent to the server\n");
    printf("write the operation to send:\n");
    //scanf("%d", op1);
    //memset(&serv_addr, 0, sizeof(serv_addr));
    fgets(message, buffersize, stdin);
    op1= (int) atoi(message);
    if(op1<1 || op1>4){
        printf("Error: operator code: [1-4]\n");
        return -1;
    }
    //message = (char*) calloc(buffersize, sizeof(char)) ;
    //strncpy(message, &op1, 16) ;
    // send the operator to the server
    send(sock, message, buffersize, 0);
        printf("Operation sent to the server\n");
    //WAIT FOR RESULT
    char *buffer = (char*) calloc(buffersize, sizeof(char)) ;
    read(sock, buffer, buffersize);
    int result= (int) atoi(buffer);
    printf("result recieved form server: %d\n", result);
    printf("closing connection\n");
    close(sock);
    return 0;
}
