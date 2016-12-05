//============================================================================
// Name        : SLA_WavOnSignal.cpp
// Author      : F. Glorieux
// Version     :
// Copyright   : ConnectCom
// Description : Hello World in C++, Ansi-style
//============================================================================
<<<<<<< HEAD
=======
//#define _USE_JACK
>>>>>>> a42c7de4b7750975fe6be29982d9e4e66ea14611
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
#define CTS_PIN 7
#include <wiringPi.h>
#endif

using namespace std;

atomic<bool> CTS;
std::string prefix("rec-");
std::string postfix(".wav");
char  inputFile[] = "rec.wav";
int pulseCount = 0;

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

void readCTS()
{
#ifndef PI
	int gps = open("/dev/ttyS1", O_RDWR | O_NOCTTY);
	int status;
	int mask = TIOCM_CTS;
#endif

	while (1)
	{
		// Read CTS
#ifdef PI
		CTS.store(digitalRead(CTS_PIN));
#else
		ioctl(gps, TIOCMIWAIT, mask)
		ioctl(gps, TIOCMGET, &status);
		CTS.store(status & TIOCM_CTS);
#endif

	}
}

void recordWav()
{
#ifdef PI
	system("arecord -D plughw:1 -f S16_LE -r 8000 -c 1 -t wav -d 5 rec.wav");
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
#ifdef _USE_JACK
	system("mcp -l ref.wav");
	sleep(5000);
#else
#ifdef PI
	system("aplay -D plughw:1,0 -d5 ref.wav");
#else
	system("aplay -d5 ref.wav");
#endif
#endif
}

int main(int argc, char* argv[])
{
	for (int i = 0; i < argc; i++)
	{
		cout << "Arg #" << i << ":" << argv[i] << endl;
	}
	bool record = argc > 1;
	if(record)
	{
		std::printf("-- Recording mode --\n");
	}
	else
	{
		std::printf("-- Playing mode --\n");
	}
#ifdef PI
	wiringPiSetup();
	pinMode(CTS_PIN, INPUT);
	pullUpDnControl(CTS_PIN, PUD_DOWN);
#endif

	printf("starting application\n");
	thread getCTS(readCTS);

	while (1)
	{
		if(CTS.load())
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
			printf("Pulse #%d - Wait for pulse ...\n", pulseCount++);
		}
	}
	return 0;
}
