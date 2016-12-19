#include <stdio.h>

int main(int argc, char* argv[])
{
	int i = 0;
	while(1)
	{
		system("./playback_test 16000 1 5 < ref16000.wav");
		printf("Pulse #%d\n",i++);
	}
}
