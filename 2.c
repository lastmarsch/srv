/*
   Задача 2 «Вызов лифта». 
   Киселёва Виолетта 7372
   Версия 1.0 08.03.2021
*/

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

// Сигналы реального времени
#define    Up	      48  
#define    Down	      49

enum LiftPosition {
	TOP,				// лифт вверху
	BOTTOM, 			// лифт внизу
	THIS_FLOOR			// лифт на нашем («данном») этаже
} state{LiftPosition::THIS_FLOOR};

// работа автомата и перемещение лифта
void lift_control(int signo){
	switch(state) {              //state – текущее состояние лифта
		case THIS_FLOOR: {
			if(signo == Up) {
		        printf("Остановка на текущем этаже\n");
	            printf("Движение вверх\n");
	            state = LiftPosition::TOP;
		   	}
			if(signo == Down) {
			    printf("Остановка на текущем этаже\n");
	            printf("Движение вниз\n");
	            state = LiftPosition::BOTTOM;
		   	}
		   	break;
		}	  	
		case TOP: {
			if(signo == Up) {
		   		printf("Движение вниз\n");
		        printf("Остановка на текущем этаже\n");
		        printf("Движение вверх\n");
		        state = LiftPosition::TOP;
		   	}
			if(signo == Down) {
				printf("Движение вниз\n");
	            printf("Остановка на текущем этаже\n");
	            printf("Движение вниз\n");
	            state = LiftPosition::BOTTOM;
		   	}
		   	break;
		}
		case BOTTOM: {
			if(signo == Up) {
		   	    printf("Движение вверх\n");
	            printf("Остановка на текущем этаже\n");
	            printf("Движение вверх\n");
	            state = LiftPosition::TOP;
		   	}
			if(signo == Down) {
			    printf("Движение вверх\n");
	            printf("Остановка на текущем этаже\n");
	            printf("Движение вниз\n");
	            state = LiftPosition::BOTTOM;
		   	}
		   	break;
		}
	}
}

// работа кнопки
void* push_button(void* args) {
    char ch;

    signal(48, lift_control); 
    signal(49, lift_control);

    while(ch != 'q'){
	    ch = getchar();
	    switch(ch){
	    case 'u': {
	        raise(48);
	        break;
	    }
	    case 'd': {
	        raise(49);
	        break;
	    }
	    case 'q': {
	    	printf("Окончание работы\n");
	    	break;
	    }
	    };
	}
	return (void*)0;
}

int main(int argc, char *argv[])
{
    pthread_t t;
    printf("Start\n");
    pthread_create(&t, NULL, &push_button, NULL);
    pthread_join(t, NULL);
    printf("Finish\n");
    return 0;
}