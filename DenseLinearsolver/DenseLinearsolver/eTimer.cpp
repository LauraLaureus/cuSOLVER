/*
Clase para medir tiempos en Windows con bastante precisión.

Juan Méndez para MNC, juan.mendez@ulpgc.es
*/
#include <stdio.h>
#include <windows.h>
#include "eTimer.h"

eTimer::eTimer(){

	LARGE_INTEGER tq;

	int count;

	min=0;
	number=0;
	max=0;
	average=0;
	firstdate = 1;

	QueryPerformanceFrequency(&tq);
	frecuency=tq.QuadPart;

}
void eTimer::reset(){

	last = 0;
	min=0;
	number=0;
	max=0;
	average=0;
	firstdate = 1;
}
void eTimer::start(){

	LARGE_INTEGER tq;

	QueryPerformanceCounter(&tq);
	last=tq.QuadPart;

}

double eTimer::stop(){

	LARGE_INTEGER tq;

	QueryPerformanceCounter(&tq);
	last = tq.QuadPart - last;

	if(firstdate){
		min = last;
		max = last;
		firstdate = 0;
	}
	else{
		if(last < min) min = last;
		if(last > max) max = last;
	}
	average += last;
	number ++;
	return (double)(last)/(double)frecuency;
}

void eTimer::printTime(double t){

	if(t > 1.0){
		printf("%8.3lf sec. ", t);
	}
	else {
		if(t > 0.001){
			printf("%8.3lf msec. ",  t*1000.0);
		}
		else{
			printf("%8.3lf usec. ", t*1000000.0);
		}
	}
}

void eTimer::report(char *text){

	printf("\n<%s> ",text);
	report();
}

void eTimer::report(){
	
	printf("Time (Min,Aver,Max):");
	printTime((double)(min)/(double)frecuency);
	printTime((double)(average+number/2)/((double)number*(double)frecuency));
	printTime((double)(max)/(double)frecuency);
	printf("\n");
}

double eTimer::get(){

	return (double)(min)/(double)frecuency;
}
