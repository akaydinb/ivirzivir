#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

void *thread_proc(void *param);

struct thread_block {
  void *input;
  void *output;
  int elements;
  int threads;
  int thr_id;
} ;


int main(int argc, char *argv[])	{
	pthread_t *tid;
	struct thread_block *param;
	int i, num_element, num_threads;
        int *dizi;
        long *l;

        num_threads = atoi(argv[1]);
	num_element = atoi(argv[2]);

	dizi  = (int *)malloc(num_element * sizeof(int));
	l     = (long *)malloc(num_threads * sizeof(long));
	param = (struct thread_block *)malloc(num_threads * sizeof(struct thread_block));
	tid   = (pthread_t *)malloc(num_threads * sizeof(pthread_t));

	//printf("%d elemanli dizi olusturuluyor.\n", num_element);
	for(i = 0; i < num_element; i++)	{
		dizi[i] = i;
	}

	//printf("%d thread olusturulacak\n", num_threads);
	for(i = 0; i < num_threads; i++)	{
	    //printf("[ %d / %d ] thread\n", i, num_threads);
	    param[i].input = dizi;
	    param[i].output = &l[i];
	    param[i].elements = num_element;
	    param[i].threads = num_threads;
	    param[i].thr_id = i;
	    
	    if(pthread_create(&tid[i], NULL, thread_proc, (void *)&param[i]) != 0)	{
		fprintf(stderr, "Cannot create thread\n");
		exit(EXIT_FAILURE);
	    }
	    
	}
	
	for(i = 0; i < num_threads; i++)
	    pthread_join(tid[i], NULL);

	return 0;
}

void *thread_proc(void *param)	{
	int i, starte, ende, t;
	int *iptr;
	long l = 0;
	struct thread_block *TB = (struct thread_block *)param;
	
	//printf("Merhaba!.\n");
	//printf("Ben %d threadden %d.\n", TB->thr_id, TB->threads);
	//printf("Bana islemem icin %d elemanli dizi verildi.\n", TB->elements);
	//printf("Bunun %d elemanindan baslayip %d elemanini isleyecegim\n", 
	//       (TB->elements / TB->threads) * (TB->thr_id), 
	//       (TB->elements / TB->threads));

	t = TB->elements / TB->threads;
	starte = t * (TB->thr_id);
	ende = starte  + t;

	iptr = (int *)TB->input;
	for(i = starte; i < ende; i++)	{
	    l = l + iptr[i];
	}

	printf("%ld\n", l);
	//TB->output = l;
	
	return NULL;
}

