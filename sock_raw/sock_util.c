#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "headdef.h"
#include "util.h"

void print_ipv6_header( uint8_t	*p_data )
{
	header_ipv6		*ipv6_header = (header_ipv6*)p_data;
	char		ipstr[INET6_ADDRSTRLEN];

	fprintf(stdout, "source ip=%s",
		inet_ntop( AF_INET6, ipv6_header->src_addr6, ipstr, sizeof(ipstr)) );
	fprintf(stdout, ", " "dest ip=%s",
		inet_ntop( AF_INET6, ipv6_header->dst_addr6, ipstr, sizeof(ipstr)) );
	fprintf(stdout, ", " "next header=%u", ipv6_header->next_header );

	fprintf(stdout, "\n");
}


static void dump16(
	const uint8_t	*p_packet_data,
	const int		data_length,
	char	*p_dump_data );



void dump_packet(
	const uint32_t		datalen,
	const uint8_t		*p_data
)
{
	int current_len;

	// 受信データサイズ出力
	fprintf(stdout, "size=%d\n", datalen);

	/* 16バイト単位でパケットダンプ処理を繰り返す */
	for (current_len = 0; current_len < datalen; current_len += 16)
	{
		int data_length;
		char dump_data[68 + 1];	/* パケットダンプデータ */

		/* 処理データ長を設定 */
		data_length = (current_len + 16) < datalen ? 16 : datalen - current_len;

		/* パケットダンプ変換処理 */
		dump16(&(p_data[current_len]), data_length, dump_data);

		/* パケットダンプ出力 */
		fprintf(stdout, "\t%s\n", dump_data);
	}
}

static void dump16(
	const uint8_t	*p_packet_data,
	const int		data_length,
	char	*p_dump_data )
{
	int		ret;			/* 復帰値 */
	int		cnt_packet;		/* ループカウンタ */
	int		pointer;		/* 書き込み位置(データ長) */
	char	work_buf[5];	/* 文字列編集用Work領域 */
	char    dump_buf[67];	/* 変換後文字列格納領域 */

	/* 書き込み位置初期化 */
	pointer = 0;

	/* 変換後文字列格納領域をスペース(0x20)で初期化 */
	memset( dump_buf, ' ', sizeof(dump_buf) );

	/* ヘキサダンプ出力処理ループ */
	for ( cnt_packet = 0; cnt_packet < data_length; cnt_packet++ )
	{
		/* 出力文字列編集 */
		ret = snprintf( work_buf, sizeof(work_buf), "%02x ",
			p_packet_data[cnt_packet] );

		/* 変換後文字列設定 */
		memcpy( &dump_buf[pointer], &work_buf[0], ret );

		/* 書き込み位置更新 */
		pointer += ret;

		if ( 0 == ( (cnt_packet + 1) % 8 ) )
		{
			/* 8byte区切りでスペース分書き込み位置を更新 */
			pointer++;
		}
	}

	/* 書き込み位置をASCIIコード出力位置に更新 */
	pointer = 50;

	/* ASCIIコード出力処理ループ */
	for ( cnt_packet = 0; cnt_packet < data_length; cnt_packet++ )
	{
		if ( 0 == ( cnt_packet % 8 ) )
		{
			/* 8文字区切りでスペース分書き込み位置を更新 */
			pointer++;
		}

		/* スペース(0x20)～チルダ(0x7e)以外のコードはドット(0x2e)を出力 */
		if ( ( (int)' ' <= (int)p_packet_data[cnt_packet] ) &&
			( (int)p_packet_data[cnt_packet] <= (int)'~' ) )
		{
			dump_buf[pointer] = (char)p_packet_data[cnt_packet];
		}
		else
		{
			dump_buf[pointer] = '.';
		}

		/* 書き込み位置更新 */
		pointer++;
	}

	/* パケットダンプデータに変換後文字列をコピー */
	memcpy( p_dump_data, dump_buf, pointer );
	p_dump_data[pointer] = '\0';
}
