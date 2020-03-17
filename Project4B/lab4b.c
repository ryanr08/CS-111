//NAME: Ryan Riahi
//EMAIL: ryanr08@gmail.com
//ID: 105138860

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <mraa.h>
#include <mraa/aio.h>
#include <mraa/gpio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <poll.h>
#include <math.h>

char scale;
int period;
char* logfile;
FILE* logptr;
mraa_aio_context adcPin;
mraa_gpio_context button;
const int B = 4275;
int buttonFlag;

int argChecker(char* optarg)
{
	for (size_t i = 0; i < strlen(optarg); i++)
	{
		if (!isdigit(optarg[i]))
			return 0;
	}
	return 1;	
}

double getTemp()
{
	float adcValue = mraa_aio_read(adcPin);
	float R = 1023.0/((float)adcValue) - 1.0;
	R = 100000.0*R;
	float celcius = 1.0/(log(R/100000.0)/B + 1/298.15) - 273.15;
	if (scale == 'C')
		return celcius;
	else
		return celcius * 9/5 + 32;
}

void button_interrupt()
{
	buttonFlag = 1;
}

//return 0 on a bad command and 1 on a correct command (either PERIOD= or LOG ...)
int period_and_log_handle(char* buffer)
{
	if (buffer[0] == 'L' && buffer[1] == 'O' && buffer[2] == 'G' && buffer[3] == ' ')
	{	
		if (logfile != NULL)
			fprintf(logptr, buffer);
	
		return 1;
	}
	char* PERIOD = "PERIOD=";
	int i = 0;
	for (; i < 7; i++)
	{
		if (buffer[i] != PERIOD[i])
			return 0;
	}
	while(buffer[i] != '\n')
	{
		if (!isdigit(buffer[i]))
			return 0;
		i++;
	}
	
	period = atoi(&buffer[7]);
	if (logfile != NULL)
		fprintf(logptr, buffer);

	return 1;	
}

int main(int argc, char** argv)
{
	buttonFlag = 0;
	mraa_init();
	period = 1;
	scale = 'F';
	logfile = NULL;
	static struct option long_options[] = 
	{
		{"period", required_argument, NULL, 1},
		{"scale", required_argument, NULL, 2},
		{"log", required_argument, NULL, 3},
		{0, 0, 0, 0}
	};
	int c;
	while(1)
	{
		c = getopt_long(argc, argv, "", long_options, NULL);
		if (c == -1)
			break;
		switch(c)
		{
			case 1:
				if (!argChecker(optarg))
				{
					fprintf(stderr, "Error, only positive integer values allowed for --period argument\n");
					exit(1);
				}
				period = atoi(optarg);
				break;
			case 2:
				if (strlen(optarg) != 1)
				{
					fprintf(stderr, "Error, --scale argument only takes a 1 character input\n");
					exit(1);
				}
				scale = optarg[0];
				break;
			case 3:
				logfile = optarg;
				break;
			default:
				fprintf(stderr, "Unrecognized argument.");
				exit(1);
		}
	}
	if (scale != 'F' && scale != 'C')
	{
		fprintf(stderr, "Error, Only F or C are allowed to be passed into --scale argument\n");
		exit(1);
	}

	if (logfile != NULL)
	{
		logptr = fopen(logfile, "w");
		if (logptr == NULL)
			fprintf(stderr, "Error opening log file\n");
	}

	adcPin = mraa_aio_init(1);
	if (adcPin == NULL)   //exit(2) if error with temperature initialization
	{
		fprintf(stderr, "Error initializing temperature aio\n");
		exit(2);                
	}
	button = mraa_gpio_init(60);
	if (button == NULL)     //exit(2) if error with button initialization
	{
		fprintf(stderr, "Error initializing button gpio\n");
		exit(2);
	}
	mraa_gpio_dir(button, MRAA_GPIO_IN);
	mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &button_interrupt, NULL);


	struct pollfd fds[1];
	fds[0].fd = STDIN_FILENO;
	fds[0].events = POLLIN | POLLHUP | POLLERR;

	struct timeval clock;
	struct tm* currentTime;
	time_t current;
	time_t next = 0;
	int running = 1;

	while(1)
	{
		gettimeofday(&clock, 0);
		float temperature = getTemp();

		if (buttonFlag)
		{
			time(&current);
			currentTime = localtime(&current);
			
			fprintf(stdout, "%02d:%02d:%02d SHUTDOWN\n", currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec);
			
			if (logfile != NULL)
			{
				fprintf(logptr, "%02d:%02d:%02d SHUTDOWN\n", currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec);
				fflush(logptr);
			}
			exit(0);

		}
		
		if (running && clock.tv_sec >= next)
		{
			time(&current);
			currentTime = localtime(&current);
			
			fprintf(stdout, "%02d:%02d:%02d %0.1f\n", currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec, temperature);
			
			if (logfile != NULL)
			{
				fprintf(logptr, "%02d:%02d:%02d %0.1f\n", currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec, temperature);
				fflush(logptr);
			}
			next = clock.tv_sec + period;
		}

		int ret = poll(fds, 1, 0);
		if (ret < 0)
		{
			fprintf(stderr, "Error polling\n");
			exit(1);
		}
		if (fds[0].revents & POLLIN)
		{
			char buff[100];
			char buff2[20];
			int num = read(0, buff, 256);
			int i = 0;
			for (int j = 0; j < num; j++)
			{

			if (buff[j] == '\n')
			{
				buff2[i] = '\n';
				buff2[i+1] = '\0';
				i = 0;
			}
			else
			{
				buff2[i] = buff[j];
				i++;
				continue;
			}

			if (!strcmp(buff2, "SCALE=F\n"))
			{
				scale = 'F';
				if (logfile != NULL)
				{
					fprintf(logptr, "SCALE=F\n");
					fflush(logptr);
				}
			}
			else if (!strcmp(buff2, "SCALE=C\n"))
			{
				scale = 'C';
				if (logfile != NULL)
				{
					fprintf(logptr, "SCALE=C\n");
					fflush(logptr);
				}
			}
			else if (!strcmp(buff2, "STOP\n"))
			{
				running = 0;
				if (logfile != NULL)
				{
					fprintf(logptr, "STOP\n");
					fflush(logptr);
				}
			}
			else if (!strcmp(buff2, "START\n"))
			{
				running = 1;
				if (logfile != NULL)
				{
					fprintf(logptr, "START\n");
					fflush(logptr);
				}
			}
			else if (!strcmp(buff2, "OFF\n"))
			{
				time(&current);
				currentTime = localtime(&current);
			
				fprintf(stdout, "%02d:%02d:%02d SHUTDOWN\n", currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec);
			
				if (logfile != NULL)
				{
					fprintf(logptr, "OFF\n");
					fflush(logptr);
					fprintf(logptr, "%02d:%02d:%02d SHUTDOWN\n", currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec);
					fflush(logptr);
				}
				exit(0);
			}
			else if (period_and_log_handle(buff2) == 0)
			{
				if (logfile != NULL)
					fprintf(logptr, buff2);
				fprintf(stderr, "Invalid command given\n");
				exit(1);
			}
			}
		}
	}

	if (logfile != NULL)
		fclose(logptr);

	mraa_aio_close(adcPin);
	mraa_gpio_close(button);
	exit(0);

}
