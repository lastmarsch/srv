/*
   Задача 1. «Поставщик – Потребитель»
   Киселёва Виолетта 7372
   Версия 1.0 21.02.2021
*/

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <chrono>

using namespace std;

int buffer = 0;
int maxValue = 2;

pthread_mutex_t mutex;
pthread_cond_t condition;

// структура для передачи аргументов в потоки Потребитель и Поставщик
struct ThreadArgs {
	int retValue;
	bool flag;	
};

void* Consumer (void* args) {
	cout << "Consumer:\tSTART THREAD\n";
	while(((ThreadArgs*)args)->flag) {
		pthread_mutex_lock(&mutex);

		while(buffer < maxValue)
			pthread_cond_wait(&condition, &mutex);

		{
			cout << "[" << std::chrono::steady_clock::now()
									.time_since_epoch()
									.count() 
							<< "] Consumer\tStart\tBuffer = " << buffer << endl;
			fflush(stdout);  

			buffer--;

			cout << "[" << std::chrono::steady_clock::now()
								.time_since_epoch()
								.count() 
						<< "] Consumer\tEnd\tBuffer = " << buffer << endl;
			fflush(stdout); 
		}

		pthread_mutex_unlock(&mutex);
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
	while(((ThreadArgs*)args)->flag) {
		pthread_mutex_lock(&mutex);

		{
			cout << "[" << std::chrono::steady_clock::now()
									.time_since_epoch()
									.count() 
							<< "] Supplier\tStart\tBuffer = " << buffer << endl;
			fflush(stdout);  

			buffer++; 

			cout << "[" << std::chrono::steady_clock::now()
								.time_since_epoch()
								.count() 
						<< "] Supplier\tEnd\tBuffer = " << buffer << endl;
			fflush(stdout);
		}

		if (buffer >= maxValue)	
			pthread_cond_signal(&condition);

		pthread_mutex_unlock(&mutex);
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
	
	void* retCode[2]; // массив с возвращенными значениями функций потоков

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&condition, NULL);
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

    pthread_cond_destroy(&condition);
    pthread_mutex_destroy(&mutex);

	return 0;
}
