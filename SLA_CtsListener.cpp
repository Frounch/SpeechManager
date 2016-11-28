//============================================================================
// Name        : SLA_CtsListener.cpp
// Author      : F. Glorieux
// Version     :
// Copyright   : ConnectCom
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fcntl.h>       /* File Control Definitions           */
#include <termios.h>     /* POSIX Terminal Control Definitions */
#include <unistd.h>      /* UNIX Standard Definitions          */
#include <errno.h>       /* ERROR Number Definitions           */
#include <sys/ioctl.h>   /* ioctl() */

using namespace std;

int main() {
	int serial = open("/dev/ttyS1", O_RDWR | O_NOCTTY);
	int status;
	bool currentCts;
	bool previousCts = false;

	while(1)
	{
		ioctl(serial, TIOCMGET, &status);
		currentCts = status & TIOCM_CTS;
		if(previousCts != currentCts)
		{
//			if(currentCts)
			{
				cout << "--CTS--" << currentCts << endl; // prints Here I am
			}
			previousCts = currentCts;
		}
	}
	return 0;
}
