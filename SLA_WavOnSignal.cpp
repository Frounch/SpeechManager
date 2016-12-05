//============================================================================
// Name        : SLA_WavOnSignal.cpp
// Author      : F. Glorieux
// Version     :
// Copyright   : ConnectCom
// Description : Hello World in C++, Ansi-style
//============================================================================
#define _USE_JACK
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
int count = 0;

std::string fixedLength(int value, int digits = 3) {
    unsigned int uvalue = value;
    if (value < 0) {
        uvalue = -uvalue;
    }
    std::string result;
    while (digits-- > 0) {
        result += ('0' + uvalue % 10);
        uvalue /= 10;
    }
    if (value < 0) {
        result += '-';
    }
    std::reverse(result.begin(), result.end());
    return result;
}

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
#ifdef _USE_JACK
	system("jack_capture -c 1 -b 16 -d 5 rec.wav");
#else
#ifdef PI
	system("arecord -D plughw:1 -f S16_LE -r 8000 -c 1 -t wav -d 5 rec.wav");
#else
	system("arecord -f S16_LE -r 8000 -c 1 -t wav -d 5 rec.wav");
#endif
#endif

	if(!rename("rec.wav",prefix + fixedLength(count,4) + postfix))
	{
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
		prefix(argc[2]);
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
			printf("Pulse #%d - Wait for pulse ...\n", count++);
		}
	}
	return 0;
}
