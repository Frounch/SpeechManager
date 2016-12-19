#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

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

int main(int argc, char* argv[])
{
	while(1)
	{
		// Record -> rec.wav
		system("./record_test");

		// Rename file
		std::string outputFile_str = prefix + fixedLength(pulseCount, 6) + postfix;
		char outputFile[50];
		strcpy(outputFile, outputFile_str.c_str());
		if(rename(inputFile, outputFile))
		{
			printf("could not rename file\n");
			exit(1);
		}
		printf("Pulse #%d\n", pulseCount++);
	}
}
