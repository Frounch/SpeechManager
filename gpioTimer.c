#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>

#define Period 60000
#define Peak 200
#define CTS_PIN 22

int main(int argc, char *argv[])
{
	wiringPiSetup();
	pinMode(CTS_PIN, OUTPUT);
	while(1)
	{
		digitalWrite(CTS_PIN,1);
		sleep(Peak);
		digitalWrite(CTS_PIN,0);
		sleep(Period-Peak);
	}
}