#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <wiringPi.h>

#define Period 15000
#define Peak     500
#define CTS_PIN 25

int count = 0;

int main(int argc, char *argv[])
{
	if(argc > 1)
 	{
		count = atoi(argv[1]);
	}
	wiringPiSetup();
	pinMode(CTS_PIN, OUTPUT);

	printf("Generating %d pulses every ",count);
	printf("%d sec\n", Period / 1000);
	while(1)
	{
		printf("Tick #%d\n",count);
		digitalWrite(CTS_PIN,1);
		delay(Peak);
		digitalWrite(CTS_PIN,0);
		count++;
		delay(Period-Peak);
	}
}
