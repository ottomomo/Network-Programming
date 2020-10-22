/*
 * @author: Octavio Sales Calvo, id:902769
 * @brief: server.c: server TCP calculator using socket.h, the server allows 3 different roles of client. client 1 gives the operand, 
 * client 2 gives the operator and client 3 get the result of the operation.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <netdb.h>
#include <sys/types.h>

#define buffersize 16

int operator;   //operator code
int operands[2];//operands
int flag[3];    //client flag: tells that if a role is busy already.

int main(){
    
    int end=0;
    operator=0;
    operands[0]=0;
    operands[1]=0;
    flag[0]=0;
    flag[1]=0;
    flag[2]=0;
    
    // socket TCP
    int serv_sock;
    if((serv_sock= socket(AF_INET, SOCK_STREAM, 0))<0){
        perror("socket failed:");
    }
    // SERVER ADDRESS
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;  
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // should i write loopback or INADDR_ANY????????????????
    // get the port service
    struct servent *port = NULL;
    port = getservbyname ("socketTCP", "tcp");
    serv_addr.sin_port = port->s_port; 
    // LINK
    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))<0){
        perror("bind failed:");
    }
    // START SERVICE
    if(listen(serv_sock, 10)<0){
        perror("listen failed:");
    }

    // Client Addr
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    while (!end) {
        
        //// wait for any request
        int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        //buffer of msg
        char *buffer = (char*) calloc(buffersize, sizeof(char));
        //reception
        if(recv(clnt_sock, buffer, buffersize, 0)<0){
            perror("reception failed: ");
        }
        printf("Server receives request from client with role: %s\n", buffer) ;
        int role = (int) atoi(buffer);
        
        int ok=1;
        buffer = (char*) calloc(buffersize, sizeof(char));
        switch(role){
            case 1: // request form C1
            {
                if(flag[0]){
                    // send to client that role 1 is busy
                send(clnt_sock, buffer, buffersize, 0);
                    break;
                }
                flag[0]=1;
                // send to client 1: request accepted
                sprintf( buffer, "%d", ok);
                send(clnt_sock, buffer, buffersize, 0);
                // wait for operands
                if(recv(clnt_sock, buffer, buffersize, 0)<0){
                    perror("reception failed: ");
                }
                printf("Server received an operand:%s\n", buffer) ;
                operands[0]= (int) atoi(buffer);
                if(recv(clnt_sock, buffer, buffersize, 0)<0){
                    perror("reception failed: ");
                }
                printf("Server received an operand:%s\n", buffer) ;
                operands[1]= (int) atoi(buffer);
                close(clnt_sock);   //comunication finished
                break;
            }
            case 2: // request form C2
            {
                if(flag[1]){
                    // send to client that role 2 is busy
                send(clnt_sock, buffer, buffersize, 0);
                    break;
                }
                flag[1]=1;
                // send to client 2: request accepted
                sprintf( buffer, "%d", ok);
                send(clnt_sock, buffer, buffersize, 0);
                // wait for operator
                if(recv(clnt_sock, buffer, buffersize, 0)<0){
                    perror("reception failed: ");
                }
                printf("Server received the operator code: %s. ", buffer) ;
                printf("Operator codes:[1='+', 2='-', 3='*', 4='/'] \n");
                operator= (int) atoi(buffer);
                close(clnt_sock);   //comunication finished
                break;
            }
            case 3: // request form C3
            {
                if(flag[2] || (!flag[0] || !flag[1])){
                    // send to client that role 3 is busy
                send(clnt_sock, buffer, buffersize, 0);
                    break;
                }
                flag[2]=1;
                // send to client 3: request accepted
                sprintf( buffer, "%d", ok);
                send(clnt_sock, buffer, buffersize, 0);
                int res;
                switch(operator){
                    case 1:
                        res=operands[0]+operands[1];
                        break;
                    case 2:
                        res=operands[0]-operands[1];
                        break;
                    case 3:
                        res=operands[0]*operands[1];
                        break;
                    case 4:
                        if(operands[0]==0 || operands[1]==0)
                            res=0;
                        else
                            res=operands[0]/operands[1];
                        break;
                }
                sprintf( buffer, "%d", res ) ;
                printf("Server sends the result: %d\n", res);
                // send to client 3 the result
                send(clnt_sock, buffer, buffersize, 0);
                // restart the flags
                operator=0;
                operands[0]=0;
                operands[1]=0;
                flag[0]=0;
                flag[1]=0;
                flag[2]=0;
                end=1; //ending service
                close(clnt_sock);   //comunication finished
                break;
            }
        }
    }           
    close(serv_sock);   //CLOSE SERVICE
    printf("Closing service\n");
    return 0;
}
