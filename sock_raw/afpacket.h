
#include "headdef.h"

// 指定されたインタフェースでIPv6通信の待受を行うソケットを生成する
int socket_packet(const char *dev);

// 指定されたインタフェースでデータを受信する
int recv_packet(int sock, int buffer_size, unsigned char *recv_data);


// IPv6ヘッダを取得する
void get_ipv6_header(
	uint8_t					*p_data,
	header_ipv6_analyzed	*p_ipv6_analyzed
);
