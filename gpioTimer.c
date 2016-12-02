#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <wiringPi.h>

#define Period 60000
#define Peak 100
#define CTS_PIN 22
void sleep(int period)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(period));
}

int main(int argc, char *argv[])
{
	wiringPiSetup();
	pinMode(CTS_PIN, OUTPUT);
	while(1)
	{
printf("Tick\n");
		digitalWrite(CTS_PIN,1);
		sleep(Peak);
		digitalWrite(CTS_PIN,0);
		sleep(Period-Peak);
	}
}
