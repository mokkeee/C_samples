for f in *.c;
do
	cc -c $f -g -Wall -I/usr/local/include;
done;

cc *.o -L/usr/local/lib -lnet
