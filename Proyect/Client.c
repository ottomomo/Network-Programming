/*
 * @author: Octavio Sales Calvo, id:902769
 * @brief: multicast UDP, and TCP app using socket.h 
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

#define GROUP "239.137.194.111"
#define PORTMC 7
#define bufsize 1024

int main(){
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
    printf("sending DISCOVER message to the group...\n");
    buffer="DISCOVER";
    cnt = sendto(sd, buffer, strlen(buffer), 0, &sock, len);
    if (cnt < 0) {
        perror("send DISCOVER");
        close(sd);
        exit(-1);
    }
    memset(buffer, 0, bufsize);
    cnt = (recvfrom(sd, buffer, bufsize, 0, (struct sockaddr *)&sock, &len));
    if (cnt < 0) {
            perror("recvfrom OFFER");
            exit(1);
    }
    printf("Offer recived from server, with address: %s  and port n: %s\n", inet_ntoa(sock.sin_addr), buffer);
    

}    
