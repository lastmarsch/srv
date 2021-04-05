#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>

pthread_cond_t cond1, cond2;
pthread_mutex_t mutex1, mutex2;

int T = 5;        // Период появления контейнера
int time_1 = 2;   // Время разгрузки
int n = 3;		  // Количество контейнеров

void* cont(void* args){
	int container_ID = (intptr_t)args;
    while(1) {
    	printf("*** container %d is waiting dispatcher thread %ld\n", container_ID, pthread_self());
    	// Контейнер прибыл, диспетчеру посылается запрос, сигнал
		// условной переменной cond_1
        pthread_mutex_lock(&mutex1);
		pthread_cond_signal(&cond1);
		pthread_mutex_unlock(&mutex1);
		// Ожидание разрешения диспетчера, блокировка на условной переменной cond_2
		pthread_mutex_lock(&mutex2);
		pthread_cond_wait(&cond2, &mutex2);
		pthread_mutex_unlock(&mutex2);
		// Контейнер разгружается
        printf("*** container %d unloads; *** thread %ld \n", container_ID, pthread_self());
		sleep(time_1);
		printf("*** container %d unloaded, end of cycle; *** thread %ld \n", container_ID, pthread_self());
        sleep(time_1);
    }
    return (void*)1;
}

void* dispatcher(void* args){
    while(1) {
    	// Диспетчер ждет прибытия контейнера
		pthread_mutex_lock(&mutex1);
		pthread_cond_wait(&cond1, &mutex1);
		sleep(T);
		printf("********** dispatcher in \n");
		pthread_mutex_unlock(&mutex1);

		// Диспетчер посылает разрешение на разгрузку
		pthread_mutex_lock(&mutex2);
		pthread_cond_signal(&cond2);
		printf("********** dispatcher out \n");
		pthread_mutex_unlock(&mutex2);				
    }
    return (void*)1;
}

int main(int argc, char *argv[])
{
	pthread_t tDispatcher;
	pthread_t containers[n]; // Массив с потоками-контейнерами
   
	pthread_mutex_init(&mutex1,NULL);
	pthread_mutex_init(&mutex2,NULL);	
	pthread_cond_init(&cond1,NULL);
	pthread_cond_init(&cond2,NULL);

    printf("Start\n");
    pthread_create(&tDispatcher, NULL, &dispatcher, NULL);
    for (int i = 0; i < n; ++i) {
        pthread_create(&containers[i], NULL, &cont, (void *)(intptr_t)i);
        sleep(time_1);
    }  

    pthread_join(tDispatcher, NULL);
    for (int i = 0; i < n; ++i)
    	pthread_join(containers[i], NULL);    

    pthread_cond_destroy(&cond1);
	pthread_cond_destroy(&cond2);
	pthread_mutex_destroy(&mutex1);
	pthread_mutex_destroy(&mutex2);
    printf("Finish\n");	
	return 0;
}
