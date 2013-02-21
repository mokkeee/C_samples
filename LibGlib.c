/*
 ============================================================================
 Name        : LibGlib.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

guint basic_hash_func(gconstpointer key){
	char * _key = (char *)key;
//	printf("hash_func=%c\n", *_key );
	return (guint)_key[0];
}

gboolean basic_equals_func(gconstpointer a, gconstpointer b){
	char * _a = (char *)a;
	char * _b = (char *)b;
//	printf("equals_func %c %c\n", *_a, *_b );
	return (gboolean)((char *)_a[0] == (char *)_b[0]);
}


void ite(gpointer key, gpointer value, gpointer user_data){
	printf("key=%s value=%s\n", key, value );
}

void basic_case(){
	GHashTable *p_hash;
	gpointer p_value;
	char a[] = "a";
	char b[] = "b";
	char c[] = "c";
	char a2[] = "a";

	printf("begin basic case\n");
	p_hash = g_hash_table_new(basic_hash_func, basic_equals_func);
//	p_hash = g_hash_table_new(NULL, NULL);

	g_hash_table_insert( p_hash, a, "key=a value" );
	g_hash_table_insert( p_hash, b, "key=b value" );
	g_hash_table_insert( p_hash, c, "key=c value" );
	g_hash_table_insert( p_hash, a2, "key=a2 value" );


	printf("hash size=%d\n", g_hash_table_size(p_hash) );

	p_value = g_hash_table_lookup( p_hash, "a");


	printf("got value=%s\n", p_value );
	g_hash_table_foreach(p_hash,  ite, 	NULL );
}


guint perform_hash_func(gconstpointer key){
	return (guint)((char *)key)[0];
}

gboolean perform_equals_func(gconstpointer a, gconstpointer b){
	int ret = strcmp( a, b);
	//printf("%s %s", a, b );
	if( ret == 0 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	//return (gboolean)(!strcmp( a, b) );
}


void performance_case(){
	GHashTable *p_hash;
	gpointer p_value;
	char *keybuf;
	char *value;
	long i;

	printf("begin performance_case \n" );
	p_hash = g_hash_table_new(perform_hash_func, perform_equals_func);
	//p_hash = g_hash_table_new( NULL, perform_equals_func);

	// テーブル設定
	for( i=0; i<10000; i++ )
	{
		// key設定
		keybuf = malloc( 128 );
		memset( keybuf, 0, sizeof(keybuf) );
		sprintf( keybuf,"key=%d",  i );

		// value設定
		value = malloc( 128 );
		sprintf( value,"value=%d",  i );
		g_hash_table_insert( p_hash, keybuf, value );
	}

	printf("hash size=%d\n", g_hash_table_size(p_hash) );

	// テーブル参照
	i = 3;
	keybuf = malloc( 128 );
	memset( keybuf, 0, sizeof(keybuf) );
	sprintf( keybuf, "key=%d", i );

	p_value = g_hash_table_lookup( p_hash, keybuf );
	printf("got value=%s\n", p_value );
	p_value = g_hash_table_lookup( p_hash, "key=3" );
	printf("got value=%s\n", p_value );
	//g_hash_table_foreach(p_hash,  ite, 	NULL );
}

int main(void) {

	//basic_case();
	performance_case();

}
