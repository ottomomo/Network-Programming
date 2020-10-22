#include "pch.h"
#include <iostream>


struct sockaddr_in localSock;
struct ip_mreq group;
int sd;
int datalen;
char databuf[1024];

int initSocketMC();
int readData();
int sendData();

bool read_line(SOCKET sock, char* line)
{
	while (true)
	{
		int result = recv(sock, line, 1, 0);
		if (result == 0 || result ==
			SOCKET_ERROR)
			return false;
		if (*line++ == '\n')
			break;
	}
	*line = '\x0';
	return true;
}

void socketThread(SOCKET client, sockaddr_in r) {
	char str[100];
    char nick[100];
	// wait for NICK
    read_line(client, nick);
    printf("nick: %s", nick);
    while(1){
        if(!read_line(client, str)){
            sleep(5);
	        printf("%s: disconnected\n", nick);
            closesocket(client);
            return;
        }
        printf("%s: the current value %s\n", nick, str);
    }
}

int main()
{
	WSAData data;
	int result;
	sockaddr_in service, remote;

	result = WSAStartup(MAKEWORD(2, 0), &data);
	assert(result == 0);
    //TCP socket
	SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	assert(listen_socket != INVALID_SOCKET);

	service.sin_family = AF_INET;
	service.sin_port = htons(3301);
	service.sin_addr.s_addr = INADDR_ANY;

	result = bind(listen_socket, (sockaddr*)&service, sizeof(sockaddr_in));
	assert(result != SOCKET_ERROR);

	result = listen(listen_socket, 5);
	assert(result != SOCKET_ERROR);
    // Multicast socket
	initSocketMC();
	std::thread hilo1(readData);

    while (true)
	{
		std::cout << "waiting for request" << std::endl;
		int size = sizeof(sockaddr_in);
		SOCKET client = accept(listen_socket,
			(sockaddr*)&remote, &size);
		printf("Polaczenie z %s:%d\n",
			inet_ntoa(remote.sin_addr),
			ntohs(remote.sin_port));
		assert(client != INVALID_SOCKET);

		std::thread th1(socketThread, client, remote);

	}
	return 0;
}

int initSocketMC()
{
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
	return 0;
}

int readData()
{
	/*
	* Read from the socket.
	*/
    while(1){
        datalen = sizeof(databuf);
        if (recv(sd, databuf, datalen, 0) < 0) {
            _tprintf(_T("Fatal Error: reading datagram mesage\n"));
            closesocket(sd);
            return 1;
        }
        if(strcmp(databuf, "DISCOVER") == 0){
	        printf("recieved: %s\n", databuf);
            std::thread hilo1(sendData);

    }
	return 0;
}

int sendData(){
    struct in_addr localInterface;
    struct sockaddr_in groupSock;
    SOCKET sd1;
    int datalen;
    char databuf[1024];
    // Create datagram socket.
	sd1 = socket(AF_INET, SOCK_DGRAM, 0);
	//Init the group socket structure with the group address 225.1.1.1 and port 5555
	memset((char *)&groupSock, 0, sizeof(groupSock));
	groupSock.sin_family = AF_INET;
	groupSock.sin_addr.s_addr = inet_addr("225.1.1.1");
	groupSock.sin_port = htons(5555);
	//Turn off the loop-back in order not to receive own datagrams.
	char loopch = 0;
	if (setsockopt(sd1, IPPROTO_IP, IP_MULTICAST_LOOP,
		(char *)&loopch, sizeof(loopch)) < 0) {
		_tprintf(_T("Fatal Error: setting IP_MULTICAST_LOOP\n"));
		closesocket(sd1); return 1;
	}
	//Get own IP address
	char myname[32] = { 0 };
	gethostname(myname, 32);
	hostent *he;
	he = gethostbyname(myname);
	//Configure the local interface for outgoing multicast datagrams.
	//Given IP address must be assigned to the local interface which supports multicast.
	memcpy(&localInterface.s_addr, *he->h_addr_list, 4);
	if (setsockopt(sd1, IPPROTO_IP, IP_MULTICAST_IF,
		(char *)&localInterface,
		sizeof(localInterface)) < 0) {
		_tprintf(_T("Fatal Error: local interface configuration\n"));
		closesocket(sd1); return 1;
	}
    /*
	* Sends the message to the multicast group using groupSock sockaddr structure.
	*/
	datalen = 50;
	sprintf_s(databuf, datalen, inet_ntoa(*(in_addr *)he->h_addr));
    // sending address
	if (sendto(sd1, databuf, datalen, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0)
	{
		_tprintf(_T("Fatal Error: sending datagram message\n"));
		return 1;
	}
    sprintf_s(databuf, datalen, "3301");
    // sending port number
	if (sendto(sd1, databuf, datalen, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0)
	{
		_tprintf(_T("Fatal Error: sending datagram message\n"));
		return 1;
	}
    closesocket(sd1); return 0;
}
