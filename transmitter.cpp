
#include <iostream>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int makeUDPSocket(int port) {
	// Create UDP socket and struct for foreign host
	int sock = socket(AF_INET,SOCK_DGRAM,0);
	if(sock < 0){
		std::cerr << "Couldn't open socket" << std::endl;
		exit(0);
	}

	// Bind socket
	sockaddr_in addr;
	memset((void *)&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	int rv = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
	if(rv < 0){
		std::cerr << "Couldn't bind socket" << std::endl;
		exit(0);
	}
	return sock;
}

int sendMessage(int sock,std::string message, sockaddr_in foreign_addr){
	return sendto(sock, message.c_str(), message.size(), 0, (sockaddr *)&foreign_addr, sizeof(foreign_addr));
}

int main(int argc, char const *argv[]) {
	if (argc != 4){
		std::cerr << "Usage: transmitter foreign_host Packet_Char Packet_Size" << std::endl;
		return 0;
	}
	std::string foreign_host = argv[1];
	char packet_char = argv[2][0];
	int packet_size = atoi(argv[3]);
	
	std::string packet(packet_size, packet_char);

	int local_port = 8888, foreign_port = 8889;
	int sock = makeUDPSocket(local_port);

	// Set up struct to send to
	sockaddr_in foreign_addr;

	foreign_addr.sin_family = AF_INET;
	int rv = inet_pton(AF_INET, foreign_host.c_str(), &(foreign_addr.sin_addr));
	if(rv < 0){
		std::cerr << "Couldn't convert given ip address" << std::endl;
		exit(0);
	}
	foreign_addr.sin_port = htons(foreign_port);

	int i = 1;
	while(true) {
		// Send packet
		std::cout << "Packet " << (i + 1) << "\r";
		i += 1;

		sendMessage(sock, packet, foreign_addr);
	}

	return 0;
}
