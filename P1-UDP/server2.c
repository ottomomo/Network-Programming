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

static int operator;
static int operands[2];
static int flag[3];

void sig_fork(int signo) {
    int stat;
    waitpid(0, &stat, WNOHANG);
    return;
}

int main(){
    signal(SIGCHLD, sig_fork); 

    operator=0;
    operands[0]=0;
    operands[1]=0;
    flag[0]=0;
    flag[1]=0;
    flag[2]=0;
    
    // socket UDP
    int serv_sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;  
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // should i write loopback or INADDR_ANY????????????????
    
    // get the port services
    struct servent *port = NULL;
    port = getservbyname ("socketP", "udp");
    serv_addr.sin_port = port->s_port; 
    //serv_addr.sin_port = htons(5566);  
    
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));


    while (1) {
        
        // Client Addr
        struct sockaddr_in clnt_addr;
        socklen_t clnt_addr_size = sizeof(clnt_addr);
        //buffer of msg
        char *buffer = (char*) calloc(buffersize, sizeof(char));
        // wait for request
        recvfrom (serv_sock, buffer, buffersize, 0, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
        printf("Server receive:%s\n", buffer) ;
        int role = (int) atoi(buffer);
        int pid = fork();
        if (pid == -1) {
            printf( "ERROR pid\n" );
        } 
        else if (pid == 0) {
            printf("PID: %d\n", getpid()) ;
            int ok=1;
            buffer = (char*) calloc(buffersize, sizeof(char));
            switch(role){
                case 1:
                    if(flag[0]){
                        // send to client that role 1 is busy
                        sendto (serv_sock, buffer, buffersize, 0, (struct sockaddr *)&clnt_addr, clnt_addr_size);
                        break;
                    }
                    flag[0]=1;
                    // send request accepted
                    sprintf( buffer, "%d", ok);
                    sendto (serv_sock, buffer, buffersize, 0, (struct sockaddr *)&clnt_addr, clnt_addr_size);
                    // wait for operands
                    recvfrom (serv_sock, buffer, buffersize, 0, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
                    printf("Server received an operand:%s\n", buffer) ;
                    operands[0]= (int) atoi(buffer);
                    recvfrom (serv_sock, buffer, buffersize, 0, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
                    printf("Server received an operand:%s\n", buffer) ;
                    operands[1]= (int) atoi(buffer);
                    break;
                case 2:
                    if(flag[1]){
                        // send to client that role 2 is busy
                        sendto (serv_sock, buffer, buffersize, 0, (struct sockaddr *)&clnt_addr, clnt_addr_size);
                        break;
                    }
                    flag[1]=1;
                    // send request accepted
                    sprintf( buffer, "%d", ok);
                    sendto (serv_sock, buffer, buffersize, 0, (struct sockaddr *)&clnt_addr, clnt_addr_size);
                    // wait for operator
                    recvfrom (serv_sock, buffer, buffersize, 0, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
                    printf("Server received the operator:%s\n", buffer) ;
                    operator= (int) atoi(buffer);
                    break;
                case 3:
                    if(flag[2]){
                        // send to client that role 2 is busy
                        sendto (serv_sock, buffer, buffersize, 0, (struct sockaddr *)&clnt_addr, clnt_addr_size);
                        break;
                    }
                    flag[2]=1;
                    // send request accepted
                    sprintf( buffer, "%d", ok);
                    sendto (serv_sock, buffer, buffersize, 0, (struct sockaddr *)&clnt_addr, clnt_addr_size);
                    int res;
                    while(!flag[0] && !flag[1]);
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
                    sprintf( buffer, "%d", total ) ;
                    printf("server send the result: %d", res);
                    sendto (serv_sock, buffer, buffersize, 0, (struct sockaddr *)&clnt_addr, clnt_addr_size);
                    operator=0;
                    operands[0]=0;
                    operands[1]=0;
                    flag[0]=0;
                    flag[1]=0;
                    flag[2]=0;
            }
            close(serv_sock) ;
            return 0 ;
        } 
        /*else {
        close(serv_sock);
        }*/
    }           
    close(serv_sock);
    return 0;
}
