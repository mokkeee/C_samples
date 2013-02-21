
#include <stdio.h>

#include <libnet.h>


#include "afpacket.h"
#include "util.h"

#define SRC_IP  "3002:bf7::19"
#define DST_IP "3002:bf7::164"


// AFパケット生成
int sock_afpacket( char* dev )
{
	return socket_packet( dev );
}

int sock_udp( char* srcip, uint16_t	port )
{
	int sock;
	struct sockaddr_in6		in6;
	int ret;
	
	in6.sin6_family = AF_INET6;
	in6.sin6_flowinfo = 0;
	in6.sin6_port = htons(port);
	inet_pton( AF_INET6, srcip, &in6.sin6_addr );

	sock = socket( AF_INET6, SOCK_DGRAM, 0);
	ret = bind( sock, (struct sockaddr*)&in6, sizeof(in6) );
	if ( ret != 0 )
	{
		perror("bind");
		return -1;
	}
	return sock;
}

int main( int argc, char* argv[] )
{
	int		socket;
	unsigned char	data[1280];
	int		size;

	libnet_t	*libnet_context;
	char		libnet_errmsg_buf[LIBNET_ERRBUF_SIZE];

	//socket = sock_afpacket( argv[1] ); 
	socket = sock_udp( argv[2], strtoul(argv[3], NULL, 10 ) );
	if ( socket == -1 )
	{
		fprintf(stderr, "socket error\n");
		return 1;
	}

	// libnetのコンテキスト作成
	libnet_context = libnet_init(LIBNET_RAW6_ADV, NULL, libnet_errmsg_buf );
	//libnet_context = libnet_init(LIBNET_RAW6_ADV, argv[1], libnet_errmsg_buf );
	if ( NULL == libnet_context )
	{
		fprintf(stderr, "%s\n", libnet_errmsg_buf );
		close(socket);
		return 1;
	}
		
	for(;;)
	{
		libnet_ptag_t			ptag_udp, ptag_ip6;
		uint8_t					*pbuf;
		uint32_t				payload_len;
		
		//afpacketからのデータ受信
		//size = recv_packet( socket, sizeof(data), data );
		//print_ipv6_header( data );

		// UDPデータ受信
		size = recv( socket, data, sizeof(data), MSG_TRUNC );
		
		printf("data recieve. ");
		dump_packet( size, data );
		putc('\n', stdout);

		// UDPヘッダ
		ptag_udp = libnet_build_udp(
			(uint16_t)(random()%(65535-49152)+49152),	// source port(49152 - 65535)
			53,		// destination port
			sizeof(header_udp) + size,	// length of UDP pakcet
			0,			// checksum(autofill)
			data,		// payload
			size,		// payload length
			libnet_context,
			0 );
		
		pbuf = libnet_getpbuf( libnet_context, ptag_udp );
		payload_len = libnet_getpbuf_size( libnet_context, ptag_udp );

		// IPv6ヘッダ
		ptag_ip6 = libnet_build_ipv6(
			0,		// Traffic Class
			0,		// Flow Label
			//sizeof(header_ipv6) + sizeof(header_udp) + size,	// total length of the IP packet
			sizeof(header_udp) + size,	// total length of the IP packet
			IPPROTO_UDP,	// Next header(UDP)
			10,		// Hop Limit
			libnet_name2addr6(
				libnet_context, SRC_IP, LIBNET_DONT_RESOLVE ),		// src ip
			libnet_name2addr6(
				libnet_context, DST_IP, LIBNET_DONT_RESOLVE ),		// dst ip
			pbuf,			// payload
			payload_len,	// Payload Length
			libnet_context,
			0 );
		
		// 送信データ取得
		pbuf = libnet_getpbuf( libnet_context, ptag_ip6 );
		payload_len = libnet_getpbuf_size( libnet_context, ptag_ip6 );

		// チェックサム計算←IPv6は未対応の模様
		//libnet_do_checksum( libnet_context, pbuf, IPPROTO_UDP, payload_len );

		printf("data sending. ");
		dump_packet( payload_len, pbuf );

		// データ送信
		//libnet_write_raw_ipv6( libnet_context, data, size );
		libnet_write_raw_ipv6( libnet_context, pbuf, payload_len );

		// 作成データ解放
		libnet_clear_packet( libnet_context );
	}

	return 0;
}

