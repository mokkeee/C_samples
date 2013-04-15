#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <sys/time.h>

#include <atomic.h>

volatile unsigned int counter = 0;

#define	THREAD_NUM		1000
#define COUNTUP_PER_THREAD	100000

void countup_thr_atomic() {
	int i;
	for(i=0; i<COUNTUP_PER_THREAD; i++)
		atomic_inc_uint_nv(&counter);
}

pthread_mutex_t mutexlock;
void countup_thr_mutex() {
	int i;
	for(i=0; i<COUNTUP_PER_THREAD; i++) {
		pthread_mutex_lock(&mutexlock);
		counter++;
		pthread_mutex_unlock(&mutexlock);
	}
}

int main(int argc, char* argv[]) {
	void* (*thr_func)(void*);
	if ( argc > 1 && strcmp(argv[1], "mutex") == 0 ) {
		pthread_mutex_init(&mutexlock, NULL);
		thr_func = (void*(*)(void*))countup_thr_mutex;
		puts("set mutexlock countup");
	} else {
		thr_func = (void*(*)(void*))countup_thr_atomic;
		puts("set atomic countup");
	}
	
	struct timeval	start, end;
	gettimeofday(&start, NULL);
	pthread_t	tid[THREAD_NUM];
	int i;
	for(i=0; i<THREAD_NUM; i++)
		pthread_create(&tid[i], NULL, thr_func, NULL);
	for(i=0; i<THREAD_NUM; i++)
		pthread_join(tid[i], NULL);
	gettimeofday(&end, NULL);

	assert(counter == THREAD_NUM * COUNTUP_PER_THREAD);

	printf("elapsed time:%d.%06d\n",
		end.tv_sec - start.tv_sec -
			(start.tv_usec<end.tv_usec ? 0 : 1),
		end.tv_usec - start.tv_usec +
			(start.tv_usec<end.tv_usec ? 0 : 1000000) );

	return 0;
}
