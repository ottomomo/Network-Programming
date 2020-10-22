#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>

#define buffersize 16

int main(int argc, char *argv[]){
    
    if (argc < 2) {
		printf("Use: ./client role [arguments]\n");
		return -1;
	}
 
    // SOCKET UDP
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    // CLIENT ADDRESS
    struct sockaddr_in clnt_addr;
    memset(&clnt_addr, 0, sizeof(clnt_addr));
    clnt_addr.sin_family = AF_INET;  
    clnt_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // should i write loopback or INADDR_ANY????????????????
    clnt_addr.sin_port =0;
    // CONECTING
    bind(sock, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr));
    // SERVER ADDRESS
    struct sockaddr_in serv_addr;
    socklen_t serv_addr_size = sizeof(serv_addr);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;  
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // should i write loopback or INADDR_ANY????????????????
    // get the port services
    struct servent *port = NULL;
    port = getservbyname ("socketP", "udp");
    serv_addr.sin_port = port->s_port; 
            
    int role= (int) atoi(argv[1]);
    switch (role){
        case 1: //role 1 must send the operands to the server
        {
            if(argc<4){
                printf("Use: ./client 1 op1 op2\n");
                return -1;
            } 
            char *message = (char*) calloc(strlen(argv[1])+1, sizeof(char)) ;
            strncpy(message, argv[1], 16) ;
            // set the role
            sendto (sock, message, strlen(message), 0, (struct sockaddr *)&serv_addr, serv_addr_size);
            
            char *buffer = (char*) calloc(buffersize, sizeof(char)) ;
            // wait for respond from server
            recvfrom (sock, buffer, buffersize, 0, (struct sockaddr *)&serv_addr, &serv_addr_size);

            int result= (int) atoi(buffer);
            if(result){
                printf("result form server: accepted\n");
                int op1=(int) atoi(argv[2]);
                int op2=(int) atoi(argv[3]);
                if((op1<0 || op1>9) || (op2<0 || op2>9)){
                    printf("Error: operands must be an positve integer and only one digite [0-9]\n");
                    return -1;
                }
                char *message = (char*) calloc(strlen(argv[2])+1, sizeof(char)) ;
                strncpy(message, argv[2],16) ;
                // send the operand 1 to the server
                sendto (sock, message, strlen(message), 0, (struct sockaddr *)&serv_addr, serv_addr_size);
                
                message = (char*) calloc(strlen(argv[2])+1, sizeof(char)) ;
                strncpy(message, argv[3], 16) ;
                // send the operand 2 to the server
                sendto (sock, message, strlen(message), 0, (struct sockaddr *)&serv_addr, serv_addr_size);
                
                printf("Operands sent to the server\n");
            }else{
                printf("result form server: busy\n");
                close(sock);
                printf("closing connection\n");
                return -1;
            }
            break;
        }
        case 2: // role 2 must send the operator to the server
        {
            if(argc<3){
                printf("Use: ./client 2 operator[1='+', 2='-', 3='*', 4='/']\n");
                return -1;
            }
            char *message = (char*) calloc(strlen(argv[1])+1, sizeof(char)) ;
            strncpy(message, argv[1], 16) ;
            // set the role
            sendto (sock, message, strlen(message), 0, (struct sockaddr *)&serv_addr, serv_addr_size);
            
            char *buffer = (char*) calloc(buffersize, sizeof(char)) ;
            // wait for respond from server
            recvfrom (sock, buffer, buffersize, 0, (struct sockaddr *)&serv_addr, &serv_addr_size);
            int result= (int) atoi(buffer);
            if(result){
                printf("result form server: accepted\n");

                int op=(int) atoi(argv[2]);
                if(op<1 || op>4){
                    printf("Error: operators code:[1='+', 2='-', 3='*', 4='/'] \n");
                    return -1;
                }
                message = (char*) calloc(strlen(argv[2])+1, sizeof(char)) ;
                strncpy(message, argv[2], 16) ;
                // send the operator to the server
                sendto (sock, message, strlen(message), 0, (struct sockaddr *)&serv_addr, serv_addr_size);
                printf("Operator sent to the server\n");
            }else{
                printf("result form server: busy\n");
                close(sock);
                printf("closing connection\n");
                return -1;
            }
            break;
        }
        case 3: // role 3 must recieve the result of the operation from the server
        {
            if(argc<2){
                printf("Use: ./client 3\n");
                return -1;
            }
            char *message = (char*) calloc(strlen(argv[1])+1, sizeof(char)) ;
            strncpy(message, argv[1], 16) ;
            // set the role
            sendto (sock, message, strlen(message), 0, (struct sockaddr *)&serv_addr, serv_addr_size);
            
            char *buffer = (char*) calloc(buffersize, sizeof(char)) ;
            // wait for respond from server
            recvfrom (sock, buffer, buffersize, 0, (struct sockaddr *)&serv_addr, &serv_addr_size);
            int result= (int) atoi(buffer);
            if(result){
                printf("result form server: accepted\n");
                buffer = (char*) calloc(buffersize, sizeof(char)) ;
                // wait for the result
                recvfrom (sock, buffer, buffersize, 0, (struct sockaddr *)&serv_addr, &serv_addr_size);
                int result= (int) atoi(buffer);
                printf("result recieved form server: %d\n", result);
            }else{
                printf("result form server: busy\n");
                close(sock);
                printf("closing connection\n");
                return -1;
            }
            break;
        }
        default:
            printf("incorrect role. Use: ./client [1-3]\n");
            return -1;
    }
    printf("closing connection\n");
    close(sock);
    return 0;
}
