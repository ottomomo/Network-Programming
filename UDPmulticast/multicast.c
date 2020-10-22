/*
 * @author: Octavio Sales Calvo, id:902769
 * @brief: multicast UDP multithreading app using socket.h 
 * 
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <pthread.h>


#define GROUP "239.137.194.111"
#define PORT 25558
#define bufsize 1024

void *sender(void);
void *receiver(void);

/*void sig_int(int signo) {
    close(sd);
    exit(0);
}*/

int main (int argc, char *argv[]){
    
    int p1, p2;
    pthread_t thread_sen, thread_rec;
    
    // SOCKET
  /*  sd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        perror("socket");
        exit(1);
    }
    // SIGNAL HANDLER
    signal(SIGINT, sig_int); */
    int op;
    if(argc<2){
        printf("Use: ./udpMulticast operation[1-3]\n 1: send a msg to the multicast group.\n 2: recieve msg from group.\n 3: send and receive msgs\n");
        return -1;
    }
        op=(int)atoi(argv[1]);
        if(op==1){//SENDER
            p1 = pthread_create( &thread_sen, NULL, sender, NULL); 
            if(p1)
            {
                fprintf(stderr,"Error - pthread_create() return code: %d\n",p1);
                exit(EXIT_FAILURE);
            }
            pthread_join( thread_sen, NULL);
        }
        else if(op==2){  //RECEIVER
            p2 = pthread_create( &thread_rec, NULL, receiver, NULL); 
            if(p2)
            {
                fprintf(stderr,"Error - pthread_create() return code: %d\n",p1);
                exit(EXIT_FAILURE);
            }
            pthread_join( thread_rec, NULL);
        }else if(op==3){ // SENDER AND RECEIVER
            p1 = pthread_create( &thread_sen, NULL, sender, NULL); 
            if(p1)
            {
                fprintf(stderr,"Error - pthread_create() return code: %d\n",p1);
                exit(EXIT_FAILURE);
            }
            p2 = pthread_create( &thread_rec, NULL, receiver, NULL); 
            if(p2)
            {
                fprintf(stderr,"Error - pthread_create() return code: %d\n",p1);
                exit(EXIT_FAILURE);
            }
            pthread_join( thread_sen, NULL);
            pthread_join( thread_rec, NULL);
        }else{
            fprintf( stderr, "Error: operation code must be in range: [ 1=sen, 2=rec, 3=sen+rec ]\n");
            return -1;
        }
        return 0;
}

void *receiver(void){
        int sd;
        struct sockaddr_in sock;
        int cnt;
        char buffer[bufsize];
        u_int yes=1;
        // SOCKET
        sd = (socket(PF_INET, SOCK_DGRAM, 0));
        if (sd < 0) {
            perror("socket");
            exit(1);
        }
        /* allow multiple sockets to use the same PORT number */
        if (setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
            perror("Reusing ADDR failed");
            exit(1);
        }
        /*
        * initialization of the reception socket
        */
        memset(&sock, 0, sizeof(sock));
        sock.sin_family = AF_INET;
        sock.sin_port = htons(PORT);
        sock.sin_addr.s_addr = inet_addr(GROUP);
        
        int len = sizeof(sock);
        struct ip_mreq imr;
        
        /*
        * bind the socket
        */
        if (bind(sd, &sock, sizeof(sock)) < 0) {
            perror("bind");
            exit(1);
        }
        
        /*
        * initialization of the structure imr
        */
        imr.imr_multiaddr.s_addr = inet_addr(GROUP);/* multicast group to join */
        imr.imr_interface.s_addr = htonl(INADDR_ANY);/* interface to join one */
        //SOCKET JOIN THE MULTICAST GROUP
        if (setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(struct ip_mreq)) < 0) {
            perror("setsockopt - IP_ADD_MEMBERSHIP");
            exit(1);
        }
        
        
        // Emisor Addr
        //struct sockaddr_in user_addr;
        //socklen_t user_addr_size = sizeof(user_addr);
        /*
        * reception of datagrams
        */
        while (1) {
            cnt = (recvfrom(sd, buffer, bufsize, 0, (struct sockaddr *)&sock, &len)); //???????????
            if (cnt < 0) {
                perror("recvfrom");
                exit(1);
            }
            else if (cnt == 0) { /* end of transmission */
                break;
            }
            //uint32_t s_addr= sock.sin_addr.s_addr;//inet_ntoa()
            printf("user form address: %s  ", inet_ntoa(sock.sin_addr));
            printf("sent: %s", buffer); /* posting of the message */
            memset(buffer, 0, bufsize);
        }
        close(sd);
}

void *sender(void){
        int sd;
        struct sockaddr_in sock;
        int cnt;
        // SOCKET
        sd = socket(PF_INET, SOCK_DGRAM, 0);
        if (sd < 0) {
            perror("socket");
            exit(1);
        }
        /*
        * initialization of the socket of emission
        */
        memset(&sock, 0, sizeof(sock));
        sock.sin_family = AF_INET;
        sock.sin_port = htons(PORT);
        sock.sin_addr.s_addr = inet_addr(GROUP);

        int len = sizeof(sock);
        char *buffer;
        unsigned char ttl = 1;
        // SET THE SOCKET MULTICAST WITH TTL=1
        setsockopt(sd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
        
        /*
        * emission of the datagrams
        */
        buffer= (char*) calloc(bufsize, sizeof(char));
        printf("Write the msg to send:\n");
        while(fgets(buffer, bufsize, stdin)!=NULL)
        {
            cnt = sendto(sd, buffer, strlen(buffer), 0, &sock, len);
            if (cnt < 0) {
                perror("sendto");
                close(sd);
                exit(-1);
            }
            memset(buffer, 0, bufsize);
            printf("Write the msg to send:\n");
        }

        close(sd);
}
