int main(int argc, char* argv[])
{
	int i = 0;
	while(1)
	{
		system("./playback_test 8000 1 5 < ref.wav");
		printf("Pulse #%d\n",i++);
	}
	
}
