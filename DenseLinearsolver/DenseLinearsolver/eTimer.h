/*
Clase para medir tiempos en Windows con bastante precisión.

Juan Méndez para MNC, juan.mendez@ulpgc.es
*/
typedef __int64 longint;

class eTimer{
private:
	longint	last;
	longint	min;
	longint	max;
	longint	average;
	longint	frecuency;

	int		number;
	int     firstdate;
	void printTime(double t);

public:
	eTimer();
	//~eTimer();
	void start();
	double stop();
	void reset();
	void report();
	void report(char *text);
	double get();
};
