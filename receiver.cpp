
#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <string.h>
#include <fstream>

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

int main(int argc, char const *argv[]) {
	if(argc != 5) {
		std::cerr << "Usage: receive out_file test_length(in seconds) num_tests packet_size" << std::endl;
		return 0;
	}
	std::string out_file = argv[1];
	float test_length = atof(argv[2]);
	int num_tests = atoi(argv[3]);
	int packet_size = atoi(argv[4]);

	// Create UDP sock
	int local_port = 8889;
	int sock = makeUDPSocket(local_port);

	// The foreign address
	sockaddr_in trans_addr;
	socklen_t addrlen = sizeof(trans_addr);
	
	// Buffer to actually receive data
	int buffer_size = packet_size * 4;
	char buffer[buffer_size];

	std::ofstream ofstr(out_file);
	ofstr << "test_length:" << test_length << " num_tests:" << num_tests << " packet_size:" << packet_size << std::endl;
	
	// Statistics on the number of bytes
	for (int i = 0; i < num_tests; ++i) {
		unsigned long total_bytes = 0;
		unsigned int number_of_packets = 0;

		std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
		std::chrono::time_point<std::chrono::steady_clock> now;
		std::chrono::duration<double> dur;
		while(true) {
			int recv_len = recvfrom(sock, buffer, buffer_size, 0, (sockaddr *) &trans_addr, &addrlen);
			if (recv_len == packet_size) {
				number_of_packets += 1;
				total_bytes += recv_len;
			}

			now = std::chrono::steady_clock::now();
			dur = now - start;
			if (dur.count() > test_length) {
				std::cout << dur.count() << std::endl;
				break;
			}
		}
		std::cout << "Bytes per second: " << total_bytes / dur.count() << std::endl;
		std::cout << "KB per second: " << total_bytes / dur.count() / 1000.0 << std::endl;
		std::cout << "MB per second: " << total_bytes / dur.count() / 1000.0 / 1000.0 << std::endl;
		std::cout << "GB per second: " << total_bytes / dur.count() / 1000.0 / 1000.0 / 1000.0 << std::endl;

		ofstr << i << " " <<  dur.count() << " " << total_bytes << " " << total_bytes / dur.count() / 1000.0 / 1000.0 / 1000.0 << std::endl;
	}
}
