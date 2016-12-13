//============================================================================
// Name        : SLA_WavOnSignal.cpp
// Author      : F. Glorieux
// Version     :
// Copyright   : ConnectCom
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <iostream>
#include <iostream>
#include <fcntl.h>       /* File Control Definitions           */
#include <termios.h>     /* POSIX Terminal Control Definitions */
#include <unistd.h>      /* UNIX Standard Definitions          */
#include <errno.h>       /* ERROR Number Definitions           */
#include <sys/ioctl.h>   /* ioctl() */
#include <atomic>
#include <thread>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

#ifdef __arm__
#define PI
#endif

#ifdef PI
#define CTS_PIN 25
#include <wiringPi.h>
#endif

using namespace std;

atomic<bool> CTS;
std::string prefix("rec-");
std::string postfix(".wav");
char  inputFile[] = "rec.wav";
int pulseCount = 0;
bool record;

std::string fixedLength(int value, int digits = 3) {
	std::string result;
	if (value < 0) 
	{
		result += '-';
		value = -value;
	}
	while (digits-- > 0) 
	{
		result += ('0' + value % 10);
		value /= 10;
	}

	std::reverse(result.begin(), result.end());
	return result;
}

void recordWav()
{
#ifdef PI
	system("arecord -D plughw-1 -f S16_LE -r 8000 -c 1 -t wav -d 5 rec.wav");
#else
	system("arecord -f S16_LE -r 8000 -c 1 -t wav -d 5 rec.wav");
#endif
	std::string outputFile_str =prefix + fixedLength(pulseCount, 4) + postfix ;
	char outputFile[50];
	strcpy(outputFile, outputFile_str.c_str());
	if(rename(inputFile, outputFile))
	{
		printf("could not rename file\n");
		exit(1);
	}
}

void playWav()
{
#ifdef PI
	system("aplay -d5 ref.wav");
#else
	system("aplay -d5 ref.wav");
#endif
}

void tick()
{
	if(record)
	{
		// std::printf("-- Recording sound --\n");
		recordWav();
	}else
	{
		// std::printf("-- Playing sound --\n");
		playWav();
	}
	printf("Pulse #%d\n", pulseCount++);
}

#ifdef PI
void trig()
{
	if(digitalRead(CTS_PIN))
	{
		tick();
	}
}
#endif

int main(int argc, char* argv[])
{
	/*
	* Read args
	*/
	for (int i = 0; i < argc; i++)
	{
		cout << "Arg #" << i << ":" << argv[i] << endl;
	}
	record = argc > 1;

	if(record)
	{
		std::printf("-- Recording mode --\n");
	}
	else
	{
		std::printf("-- Playing mode --\n");
	}

	/* 
	* Init
	*/
	printf("Initializing ...");

#ifdef PI
	// Init gpio
	wiringPiSetup();
//	pinMode(CTS_PIN, INPUT);
//	pullUpDnControl(CTS_PIN, PUD_DOWN);
#else
	// Init serial
	int gps = open("/dev/ttyS1", O_RDWR | O_NOCTTY);
	int status;
	bool cts;
	unsigned int mask = TIOCM_CTS;
#endif
	printf(" OK\n");

    /*
     * Run 
     */
	
	printf("Starting application\n");

#ifdef PI
	// on each rising edge, execute tick
	wiringPiISR (CTS_PIN, INT_EDGE_RISING, &trig);
	
	// wait
	while (1)
	{
		delay(1000);
	}
#else
	while (1)
	{
		
		// Wait for CTS change
		ioctl(gps, TIOCMIWAIT, mask);
		
		// Read CTS state
		ioctl(gps, TIOCMGET, &status);
		if(status & TIOCM_CTS)
		{
			tick();
		}
	}
	
#endif
	
	return 0;
}
