
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>

// IPv6ヘッダ
typedef struct
{
	// Version(4bit)、Traffic Class(8bit)、Flow Label(20bit)
	uint8_t		version_trafficClass_flowLabel[(4+8+20)/8];

	// Payload Length
	uint16_t	payload_len;

	// Next header
	uint8_t		next_header;

	// Hop Limit
	uint8_t		hop_limit;

	// Source address(IPv6)
	uint8_t		src_addr6[16];

	// Destination address(IPv6)
	uint8_t		dst_addr6[16];
} header_ipv6;

// IPv6解析ヘッダ
typedef struct
{
	// Version
	uint8_t		version;

	// Traffic Class
	uint8_t		traffic_class;

	// Flow Label
	uint32_t	flow_label;
	
	// Payload Length
	uint16_t	payload_len;

	// Next header
	uint8_t		next_header;

	// Hop Limit
	uint8_t		hop_limit;

	// Source address(IPv6)
	uint8_t		src_addr6[16];

	// Destination address(IPv6)
	uint8_t		dst_addr6[16];
} header_ipv6_analyzed;



// UDPヘッダ
typedef struct
{
	// Source Port
	uint16_t	src_port;

	// Destination Port
	uint16_t	dst_port;

	// Payload Length
	uint16_t	payload_len;

	// Checksum
	uint16_t	checksum;
} header_udp;


// 擬似ヘッダ(IPv6)
typedef struct
{
	// Source address(IPv6)
	uint8_t		src_addr6[16];

	// Destination address(IPv6)
	uint8_t		dst_addr6[16];

	// 上位レイヤ パケット長
	uint32_t	upperLayer_packet_length;

	// zero
	uint8_t		zero[3];

	// Next header
	uint8_t		next_header;
} pseudo_header_ip6;
