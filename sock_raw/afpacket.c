#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>

#include <netinet/in.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <linux/if.h>

#include "afpacket.h"


// 指定されたインタフェースでIPv6通信の待受を行うソケットを生成する
int socket_packet(const char *dev)
{
	int sock, ret;
	struct ifreq ifr;
	struct sockaddr_ll sll;

	sock = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_IPV6));
	if (sock < 0) {
		perror("socket");
		return -1;
	}

	/* get ifindex */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, dev, IFNAMSIZ);
	ret = ioctl(sock, SIOGIFINDEX, &ifr);
	if (ret < 0) {
		perror("ioctl");
		close(sock);
		return -1;
	}

	sll.sll_family = AF_PACKET;
	sll.sll_protocol = htons(ETH_P_IPV6);
	sll.sll_ifindex = ifr.ifr_ifindex;
	ret = bind(sock, (struct sockaddr*)&sll, sizeof(sll));
	if (ret == -1) {
		perror("bind");
		close(sock);
		return -1;
	}

	return sock;
}

// 指定されたインタフェースでデータを受信する
int recv_packet(int sock, int buffer_size, unsigned char *recv_data)
{
	struct sockaddr_ll src_ll;
	socklen_t		addrlen = sizeof(src_ll);
	int ret = recvfrom(sock, recv_data, buffer_size, MSG_TRUNC, (struct sockaddr*)&src_ll, &addrlen );
	if (ret == -1) {
		perror("read");
		return 0;
	}
	if (ret > buffer_size) {
		fprintf(stderr, "readpacket error. datasize=%d\n", ret);
		return 0;
	}
	return ret;
}

// IPv6ヘッダを取得する
void get_ipv6_header(
	uint8_t					*p_data,
	header_ipv6_analyzed	*p_ipv6_analyzed
)
{
	header_ipv6		header;
	union
	{
		uint32_t		bit32;
		uint8_t			bit[4];
	} wk_bit32;		// 32bit計算用

	// ヘッダをコピー(念のためポインタキャストではなくコピーで取得)
	memcpy( &header, p_data, sizeof(header) );

	// 各データを取得
	// Version(4bit)
	p_ipv6_analyzed->version = header.version_trafficClass_flowLabel[0] >> 4;

	// Traffic Class(8bit)
	p_ipv6_analyzed->traffic_class =
		header.version_trafficClass_flowLabel[0] << 4 &
		header.version_trafficClass_flowLabel[1] >> 4;
	
	// Flow Label(20bit)
	// Version, TrafficClassを除いた値を取得
	memcpy( &wk_bit32, header.version_trafficClass_flowLabel, sizeof(wk_bit32) );
	wk_bit32.bit[0] = 0;
	wk_bit32.bit[1] &= 0x0f;
	p_ipv6_analyzed->flow_label = ntohl( wk_bit32.bit32 );

	// Payload Length(16bit)
	p_ipv6_analyzed->payload_len = ntohs( header.payload_len );

	// Next header(8bit)
	p_ipv6_analyzed->next_header = header.next_header;

	// Hop Limit(8bit)
	p_ipv6_analyzed->hop_limit = header.hop_limit;

	// Source address(128bit)
	memcpy( p_ipv6_analyzed->src_addr6, header.src_addr6, sizeof(p_ipv6_analyzed->src_addr6) );

	// Destination address(128bit)
	memcpy( p_ipv6_analyzed->dst_addr6, header.dst_addr6, sizeof(p_ipv6_analyzed->dst_addr6) );
}


	

