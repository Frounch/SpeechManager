#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <wiringPi.h>

#define Period 15000
#define Peak     500
#define CTS_PIN 7

int count = 1000;
void sleep(int period)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(period));
}

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
	while(count > 0)
	{
		printf("Tick #%d\n",count);
		digitalWrite(CTS_PIN,1);
		sleep(Peak);
		digitalWrite(CTS_PIN,0);
		count--;
		sleep(Period-Peak);
	}
}
