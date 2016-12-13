/*
* Simple sound playback using ALSA API and libasound.
*
* Compile:
* $ cc -o play sound_playback.c -lasound
* 
* Usage:
* $ ./play <sample_rate> <channels> <seconds> < <file>
* 
* Examples:
* $ ./play 44100 2 5 < /dev/urandom
* $ ./play 22050 1 8 < /path/to/file.wav
*
* Copyright (C) 2009 Alessandro Ghedini <alessandro@ghedini.me>
* --------------------------------------------------------------
* "THE BEER-WARE LICENSE" (Revision 42):
* Alessandro Ghedini wrote this file. As long as you retain this
* notice you can do whatever you want with this stuff. If we
* meet some day, and you think this stuff is worth it, you can
* buy me a beer in return.
* --------------------------------------------------------------
*/

#include <alsa/asoundlib.h>
#include <stdio.h>

#ifdef __arm__
#define PI
#endif

#ifdef PI
#define CTS_PIN 25
#include <wiringPi.h>
//#include <condition_variable>
#else
#include <fcntl.h>       /* File Control Definitions           */
#include <termios.h>     /* POSIX Terminal Control Definitions */
#include <unistd.h>      /* UNIX Standard Definitions          */
#include <errno.h>       /* ERROR Number Definitions           */
#include <sys/ioctl.h>   /* ioctl() */
#endif

#define PCM_DEVICE "plughw:1"

#ifdef PI
pthread_mutex_t interrupt_mutex;
pthread_cond_t interrupt_cv;

void interrupt()
{
	printf("--Interrupt--\n");
	pthread_mutex_lock(&interrupt_mutex);
	pthread_cond_signal(&interrupt_cv);
	pthread_mutex_unlock(&interrupt_mutex);
}
#endif

int main(int argc, char **argv) 
{
	unsigned int pcm, tmp, dir;
	int rate, channels, seconds;
	snd_pcm_t *pcm_handle;
	snd_pcm_hw_params_t *params;
	snd_pcm_uframes_t frames;
	char *buff;
	int buff_size, loops;

	if (argc < 4) 
	{
		printf("Usage: %s <sample_rate> <channels> <seconds>\n",
		argv[0]);
		return -1;
	}

#ifdef PI
	/* Initialize mutex and condition variable objects */
        pthread_mutex_init(&interrupt_mutex, NULL);
        pthread_cond_init (&interrupt_cv, NULL);
	pthread_mutex_lock(&interrupt_mutex);

	// Init GPIO
	wiringPiSetup();
	wiringPiISR (CTS_PIN, INT_EDGE_RISING, &interrupt);
#else
	// Init serial
	int serial = open("/dev/ttyS1", O_RDWR | O_NOCTTY);
	int status;
	unsigned int mask = TIOCM_CTS;
#endif

	rate 	 = atoi(argv[1]);
	channels = atoi(argv[2]);
	seconds  = atoi(argv[3]);

	/* Open the PCM device in playback mode */
	if (pcm = snd_pcm_open(&pcm_handle, PCM_DEVICE,
				SND_PCM_STREAM_PLAYBACK, 0) < 0) 
	printf("ERROR: Can't open \"%s\" PCM device. %s\n",
	PCM_DEVICE, snd_strerror(pcm));

	/* Allocate parameters object and fill it with default values*/
	snd_pcm_hw_params_alloca(&params);

	snd_pcm_hw_params_any(pcm_handle, params);

	/* Set parameters */
	if (pcm = snd_pcm_hw_params_set_access(pcm_handle, params,
				SND_PCM_ACCESS_RW_INTERLEAVED) < 0) 
	printf("ERROR: Can't set interleaved mode. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_format(pcm_handle, params,
				SND_PCM_FORMAT_S16_LE) < 0) 
	printf("ERROR: Can't set format. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_channels(pcm_handle, params, channels) < 0) 
	printf("ERROR: Can't set channels number. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0) < 0) 
	printf("ERROR: Can't set rate. %s\n", snd_strerror(pcm));

	/* Write parameters */
	if (pcm = snd_pcm_hw_params(pcm_handle, params) < 0)
	printf("ERROR: Can't set harware parameters. %s\n", snd_strerror(pcm));

	/* Resume information */
	printf("PCM name: '%s'\n", snd_pcm_name(pcm_handle));

	printf("PCM state: %s\n", snd_pcm_state_name(snd_pcm_state(pcm_handle)));

	snd_pcm_hw_params_get_channels(params, &tmp);
	printf("channels: %i ", tmp);

	if (tmp == 1)
	printf("(mono)\n");
	else if (tmp == 2)
	printf("(stereo)\n");

	snd_pcm_hw_params_get_rate(params, &tmp, 0);
	printf("rate: %d bps\n", tmp);

	printf("seconds: %d\n", seconds);	

		/* Allocate buffer to hold single period */
		snd_pcm_hw_params_get_period_size(params, &frames, 0);

		buff_size = frames * channels * 2 /* 2 -> sample size */;
		buff = (char *) malloc(buff_size);

		snd_pcm_hw_params_get_period_time(params, &tmp, NULL);

#ifdef PI
		// wait for the interrupt
		{
			printf("Wait for interrupt\n");
			pthread_cond_wait(&interrupt_cv, &interrupt_mutex);
			pthread_mutex_unlock(&interrupt_mutex);
		}
#else
		do
		{
			// Wait for CTS change
			ioctl(serial, TIOCMIWAIT, mask);

			// Read CTS state
			ioctl(serial, TIOCMGET, &status);
		}
		while(!(status & TIOCM_CTS));
#endif
printf("Playing file ... \n");
		for (loops = (seconds * 1000000) / tmp; loops > 0; loops--) 
		{
			if (pcm = read(0, buff, buff_size) == 0) {
				printf("Early end of file.\n");
				return 0;
			}

			if (pcm = snd_pcm_writei(pcm_handle, buff, frames) == -EPIPE) 
			{
				printf("XRUN.\n");
				snd_pcm_prepare(pcm_handle);
			} else if (pcm < 0) 
			{
				printf("ERROR. Can't write to PCM device. %s\n", snd_strerror(pcm));
			}
		}

		snd_pcm_drain(pcm_handle);
		snd_pcm_rewind(pcm_handle, frames);
	snd_pcm_close(pcm_handle);

	free(buff);

	return 0;
}
