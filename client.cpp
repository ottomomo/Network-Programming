#include "pch.h"
#include <iostream>

bool read_line(SOCKET sock, char* line);

// server address
typedef struct{
  int port;
  char ipAddress[512];
} server_t;

static server_t servers [10];   //array of servers
static int cont;		// counter of servers

//multicast sender
int sendData(){
    struct in_addr localInterface;
    struct sockaddr_in groupSock;
    SOCKET sd;
    int datalen;
    char databuf[1024];
    // Create datagram socket.
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	//Init the group socket structure with the group address 225.1.1.1 and port 5555
	memset((char *)&groupSock, 0, sizeof(groupSock));
	groupSock.sin_family = AF_INET;
	groupSock.sin_addr.s_addr = inet_addr("225.1.1.1");
	groupSock.sin_port = htons(5555);
	//Turn off the loop-back in order not to receive own datagrams.
	char loopch = 0;
	if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP,
		(char *)&loopch, sizeof(loopch)) < 0) {
		_tprintf(_T("Fatal Error: setting IP_MULTICAST_LOOP\n"));
		closesocket(sd); return 1;
	}
	//Get own IP address
	char myname[32] = { 0 };
	gethostname(myname, 32);
	hostent *he;
	he = gethostbyname(myname);
	printf("Host name : %s\n", he->h_name);
	printf("Host addr : %s\n", inet_ntoa(*(in_addr *)he->h_addr));
	//Configure the local interface for outgoing multicast datagrams.
	sendData();
	//Given IP address must be assigned to the local interface which supports multicast.
	memcpy(&localInterface.s_addr, *he->h_addr_list, 4);
	if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF,
		(char *)&localInterface,
		sizeof(localInterface)) < 0) {
		_tprintf(_T("Fatal Error: local interface configuration\n"));
		closesocket(sd); return 1;
	}
    /*
	* Sends the message to the multicast group using groupSock sockaddr structure.
	*/
	datalen = 50;
	sprintf_s(databuf, datalen, "DISCOVER");
	if (sendto(sd, databuf, datalen, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0)
	{
		_tprintf(_T("Fatal Error: sending datagram message\n"));
		return 1;
	}
	_tprintf(_T("message datagram sending: OK\n"));
    closesocket(sd);
	return 0;
}

// multicast receiver 
int readData(){
    struct sockaddr_in localSock;
    struct ip_mreq group;
    int sd;
    int datalen;
    char databuf[1024];
    // Create datagram socket.
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd == INVALID_SOCKET)
	{
		_tprintf(_T("Fatal Error: while opening socket\n"));
		return 1;
	}
	// Turn the SO_REUSEADDR option on to allow multibinding
	{
		int reuse = 1;
		if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
			(char *)&reuse, sizeof(reuse)) < 0) {
			_tprintf(_T("Fatal Error: setting SO_REUSEADDR\n"));
			closesocket(sd);
			return 1;
		}
		_tprintf(_T("setting SO_REUSEADDR: OK\n"));
	}
	//Assign the port number with the IP address given as INADDR_ANY.
	memset((char *)&localSock, 0, sizeof(localSock));
	localSock.sin_family = AF_INET;
	localSock.sin_port = htons(5555);;
	localSock.sin_addr.s_addr = INADDR_ANY;
	if (bind(sd, (struct sockaddr*)&localSock, sizeof(localSock))) {
		_tprintf(_T("Fatal Error: while binding socket\n"));
		closesocket(sd); return 1;
	}
	// Get own IP address
	char myname[32] = { 0 };
	gethostname(myname, 32);
	hostent *he;
	he = gethostbyname(myname);
	printf("Host name : %s\n", he->h_name);
	printf("Host addr : %s\n", inet_ntoa(*(in_addr *)he->h_addr));
	/* Join the group wit 225.1.1.1 address
	* Note that the IP_ADD_MEMBERSHIP option must be called
	* for every local interface on which the datagrams should be received.
	*/
	group.imr_multiaddr.s_addr = inet_addr("225.1.1.1");
	memcpy(&group.imr_interface.s_addr, *he->h_addr_list, 4);
	if (setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0) {
		_tprintf(_T("Fatal Error: adding IP to the multicasting group number %d\n"), WSAGetLastError());
		closesocket(sd); return 1;
	}
	_tprintf(_T("adding IP to the multicasting group: OK\n"));
    /*
	* Read from the socket.
	*/
	_tprintf(_T("reading datagram mesage: ......... (waiting)\n"));
	datalen = sizeof(databuf);
	if (recv(sd, databuf, datalen, 0) < 0) {        //ip address
		_tprintf(_T("Fatal Error: reading datagram mesage\n"));
		closesocket(sd);
		return 1;
	}
	strncpy(servers[cont].ipAddress, databuf, sizeof(char)*512);
    if (recv(sd, databuf, datalen, 0) < 0) {        // port
		_tprintf(_T("Fatal Error: reading datagram mesage\n"));
		closesocket(sd);
		return 1;
	}
    printf("Offer recived from server, with address: %s  and port n: %s\n", servers[cont].ipAddress, databuf);
    p= (int) atoi(databuf);
    servers[cont].port=p;
    cont++;
    closesocket(sd);
	return 0;
}

// tcp socket
int tcp(int i){
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	assert(sock != INVALID_SOCKET);
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_port = htons(3301);
	service.sin_addr.s_addr = inet_addr("127.0.0.1");
	result = connect(sock, (sockaddr*)&service,
	sizeof(sockaddr_in));
	assert(result != SOCKET_ERROR);
	char str[100];
    std::cout <<"Enter your nick:"<< std::endl;
    std::cin >> str;
    send(sock, str, strlen(str), 0);
     // SET THE FRECUENCY
    int f, n, ok=0;
    do{
	printf("Set the frecuency of the messages: [1 - 10000 ms]\n");
	memset(str, 0, 100);
    std::cin >> str;
	f = (int) atoi(str);
	if(f>0 && f < 10000)
	  ok=1;
    }while(!ok);
    srand(time(NULL));
    while(1){
      memset(str, 0, 100);
      n=rand()%100;
      sprintf(str, "%d", n);
      usleep(f*1000); 		//sleep microseconds.
      printf("sending to the server the value: %s\n", str);
        send(sock, str, strlen(str), 0);
    }
    return 0;
}
int main(){
    WSAData wsaData;
	int nCode;
	char errdesc[100];
	if ((nCode = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0)
	{
		sprintf_s(errdesc,
			"Error while initializing the WinSock library.Error %d", nCode);
		exit(1);
	}
	printf("WinSock: %s [%s]\n", wsaData.szDescription, wsaData.szSystemStatus);
	printf("MaxSockets: %d\n", wsaData.iMaxSockets);
    cont=0;
    int i=0;
    int opt, end=0;
    int p1, p2;
    while(!end){
	printf("\n _______________________________________________________\n\n");
	printf("(1) DISCOVER \n");
	if(cont>0){
	  for(i=0;i<cont;i++){
	    printf("(%d) CONNECT TO: ip: %s, port: %d\n",(i+2), servers[i].ipAddress, servers[i].port);
	  }
	}
	printf("(0) EXIT \n");
	printf("\n _______________________________________________________\n");
	scanf("%d", &opt);
	switch(opt){
	  case 0:
	    end=1;break;
	  case 1:
        std::thread th1(sender);
        std::thread th2(readData);
        th1.join();
        th2.join();
	    break;
	  default:
	    if(cont>= opt-1){
	      tcp(opt-2); 
	    }else{
	      printf("Wrong option, try again\n");
	    }break;
	}
    }
    return 0;
}    