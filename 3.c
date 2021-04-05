#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>

int x = 1;

int doControl() {
	const int workingTime = 500;
	const int deviation = 50;
	const int timeLimit = 40;

	int dt =  rand() % (deviation);
	int time = workingTime + dt;
	
	if (x == 0) while(1) {} 

	usleep(time); 
	return time;
}

void deadlineHandler(int s) {
	// вывод сообщения «#### Deadline exceeded ####»
	printf("#### Deadline exceeded ####\n");
}

void alarmHandler(int s) {
	// Вывод сообщения «#####-- Restart required! --#####»	
	printf("#####-- Restart required! --#####\n");
	// Прекращение работы системы посылкой сообщения SIGUSR1
	exit(SIGUSR1);
}

void* Timeout(void* args) {
	int time = rand() % 5 + 10;
    sleep(time);
    x = 0;
	return (void*)0;
}

void* Control(void* args) {
	const int workingLimit = 520;

	signal(SIGALRM, alarmHandler);
	signal(SIGUSR2, deadlineHandler);

	while (1) {
		alarm(0);
		alarm(4);

		int time = doControl();
		printf("########## %d \n", time);

		if (time > workingLimit)
			raise(SIGUSR2);

		sleep(3);
	}
	return (void*)0;
}

int main(int argc, char *argv[])
{
	pthread_t tControl;
    pthread_t tTimeout;
    printf("Start\n");
    pthread_create(&tControl, NULL, &Control, NULL);
    pthread_create(&tTimeout, NULL, &Timeout, NULL);

    srand(time(NULL));

    pthread_join(tControl, NULL);
    pthread_join(tTimeout, NULL);
    printf("Finish\n");	
	return 0;
}
