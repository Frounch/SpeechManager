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

#ifdef __arm__
#define PI
#endif

#ifdef PI
#define CTS_PIN 7
#include <wiringPi.h>
#endif

using namespace std;

atomic<bool> CTS;

void readCTS()
{
#ifndef PI
	int gps = open("/dev/ttyS1", O_RDWR | O_NOCTTY);
	int status;
#endif

	while (1)
	{
		// Read CTS
#ifdef PI
		CTS.store(digitalRead(CTS_PIN));
#else
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
}

void playWav()
{
#ifdef PI
	system("aplay -D plughw:1,0 -d5 ref.wav");
#else
	system("aplay -d5 ref.wav");
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
			printf("Wait for pulse ...\n");
		}
	}
	return 0;
}
