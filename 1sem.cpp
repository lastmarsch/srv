/*
   Задача 1. «Поставщик – Потребитель»
   Киселёва Виолетта 7372
   Версия 1.0 21.02.2021
*/

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <chrono>

using namespace std;

int buffer = 0;
int maxValue = 3;

// структура для передачи аргументов в потоки Потребитель и Поставщик
struct ThreadArgs {
	int retValue;
	bool flag;	
	sem_t *sem;
};

void* Consumer (void* args) {
	cout << "Consumer:\tSTART THREAD\n";
	timespec ts{.tv_sec = 1, .tv_nsec = 0};
	while(((ThreadArgs*)args)->flag) {
		sem_timedwait(((ThreadArgs*)args)->sem, &ts);

		{
			cout << "[" << std::chrono::steady_clock::now()
									.time_since_epoch()
									.count() 
							<< "] Consumer\tStart\tBuffer = " << buffer << endl;
			fflush(stdout);  
			if (buffer > 0)
				buffer--;
			cout << "[" << std::chrono::steady_clock::now()
								.time_since_epoch()
								.count() 
						<< "] Consumer\tEnd\tBuffer = " << buffer << endl;
			fflush(stdout); 
		}

		sem_post(((ThreadArgs*)args)->sem);
        sleep(2);
	}

	cout << "[" << std::chrono::steady_clock::now()
							.time_since_epoch()
							.count() 
				<< "] Consumer:\tSTOP THREAD\n";
	((ThreadArgs*)args)->retValue = 1;
	pthread_exit(args);
}

void* Supplier (void* args) {
	cout << "Supplier:\tSTART THREAD\n";
	timespec ts{.tv_sec = 1, .tv_nsec = 0};
	while(((ThreadArgs*)args)->flag) {
		sem_timedwait(((ThreadArgs*)args)->sem, &ts);

		{
			cout << "[" << std::chrono::steady_clock::now()
									.time_since_epoch()
									.count() 
							<< "] Supplier\tStart\tBuffer = " << buffer << endl;
			fflush(stdout);  
			if (buffer < maxValue) 
				buffer++;    
			cout << "[" << std::chrono::steady_clock::now()
								.time_since_epoch()
								.count() 
						<< "] Supplier\tEnd\tBuffer = " << buffer << endl;
			fflush(stdout);
		}

		sem_post(((ThreadArgs*)args)->sem);
        sleep(2);
	}

	cout << "[" << std::chrono::steady_clock::now()
							.time_since_epoch()
							.count() 
				<< "] Supplier:\tSTOP THREAD\n";
	((ThreadArgs*)args)->retValue = 2;
	pthread_exit(args);	
}

int main(int argc, char *argv[]) {
	pthread_t threadConsumer, threadSupplier;
	ThreadArgs argsConsumer, argsSupplier;		
	sem_t sem;
	void* retCode[2]; // массив с возвращенными значениями функций потоков

    sem_init(&sem, 0, 1);
    argsConsumer.sem = argsSupplier.sem = &sem;
    argsConsumer.flag = argsSupplier.flag = true;

	pthread_create(&threadConsumer, NULL, &Consumer, &argsConsumer);
	pthread_create(&threadSupplier, NULL, &Supplier, &argsSupplier);

	getchar();
	argsConsumer.flag = argsSupplier.flag = false;

	pthread_join(threadConsumer, &retCode[0]);
	pthread_join(threadSupplier, &retCode[1]);

	if (*(int*) retCode[0] == 1)
        cout << "Consumer:\tEXIT_SUCCESS\n";

    if (*(int*) retCode[1] == 2)
        cout << "Supplier:\tEXIT_SUCCESS\n";

    sem_destroy(&sem);

	return 0;
}
