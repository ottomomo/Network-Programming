/*
 * @author: Octavio Sales Calvo, id:902769
 * @brief: server.c: server TCP multithreading calculator using socket.h, 
 * client gives the operator and get the result of the operation.
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
#include <pthread.h>


#define buffersize 16

//pthread_t tid[12];  //array of threads

void * socketThread(void *arg){
    
    char buffer[buffersize];
    int newSocket = *((int *)arg);
    int op1, op2, op, r;
    printf("new socket: %d\n", newSocket);
    // wait for operands
    if((r=recv(newSocket, buffer, buffersize, 0))<0){
        printf("error %d    socket %d\n", r, newSocket);
        perror("reception failed: ");
        close(newSocket);
        return NULL;
        
    }
    printf("Server received an operand:%s\n", buffer) ;
    op1= (int) atoi(buffer);
    if(recv(newSocket, buffer, buffersize, 0)<0){
        perror("reception failed: ");
        close(newSocket);
        return NULL;
    }
    printf("Server received an operand:%s\n", buffer) ;
    op2= (int) atoi(buffer);
    // wait for operator
    if(recv(newSocket, buffer, buffersize, 0)<0){
        perror("reception failed: ");
        close(newSocket);
        return NULL;
    }
    printf("Server received the operator code: %s. ", buffer) ;
    printf("Operator codes:[1='+', 2='-', 3='*', 4='/'] \n");
    op= (int) atoi(buffer);
    // operation
    int res;
    switch(op){
        case 1:
            res=op1+op2;
            break;
        case 2:
            res=op1-op2;
            break;
        case 3:
            res=op1*op2;
            break;
        case 4:
            if(op1==0 || op2==0)
                res=0;
            else
                res=op1/op2;
            break;
    }
    sprintf( buffer, "%d", res ) ;
    printf("Server sends the result: %d\n", res);
    send(newSocket,buffer,buffersize,0);
    printf("Exit socketThread \n");
    close(newSocket);
    printf("thread closed\n");
    //pthread_exit(0);
    return NULL;
}

int main(){
    
    int end=0;
    
    // socket TCP
    int serv_sock;
    if((serv_sock= socket(AF_INET, SOCK_STREAM, 0))<0){
        perror("socket failed:");
    }
    // SERVER ADDRESS
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;  
    serv_addr.sin_addr.s_addr = INADDR_ANY;//ANY
    // get the port service
    //struct servent *port = NULL;
   // port = getservbyname ("socketTCP", "tcp");
    serv_addr.sin_port = htons(25559); 
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
    int i=0;
    pthread_t tid[15];
    while (1) {
        pthread_t tid;
        // Client Addr
        struct sockaddr_in clnt_addr;
        socklen_t clnt_addr_size = sizeof(clnt_addr);
        //// wait for any request
        int newSocket = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

        //for each client request creates a thread and assign the client request to it to process
        //so the main thread can entertain next request
        if( pthread_create(&tid, NULL, socketThread, &newSocket) != 0 )
            printf("Failed to create thread\n");
       /* if(i>=10){
            i = 0;
            while(i < 10)
            {
                pthread_join(tid[i++],NULL);
            }
            i = 0;
        }*/
        
    }           
    close(serv_sock);   //CLOSE SERVICE
    printf("Closing service\n");
    return 0;
}
