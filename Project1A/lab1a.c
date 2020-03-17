//NAME: Ryan Riahi
//EMAIL: ryanr08@gmail.com
//ID: 105138860


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>


int ready_to_exit = 0;
void errorCheck(void)
{
	fprintf(stderr, "%s, \n", strerror(errno));
	exit(1);
}
void signalHandler(int signum)
{
	if (signum == SIGPIPE)
	   ready_to_exit = 1;	
}

struct termios savedAttr;

void reset_input_mode (void)
{
	if(tcsetattr(0, TCSANOW, &savedAttr) < 0)
		errorCheck();
}

void nonShellIO (void)
{
	char buff;
	char buff2[2];

	struct termios currentAttr;

	if(tcgetattr(0, &savedAttr) < 0)
		errorCheck();
	if(tcgetattr(0, &currentAttr) < 0)
		errorCheck();

	currentAttr.c_iflag = ISTRIP;
	currentAttr.c_oflag = 0;
	currentAttr.c_lflag = 0;

	if(tcsetattr(0, TCSANOW, &currentAttr) < 0)
		errorCheck();

	while (1)
	{
		if (read(0, &buff, 1) < 0)
		   errorCheck();
		if (buff == '\004')
		   break;
		else if (buff == '\r' || buff == '\n')
		   {
			buff2[0] = '\r';
			buff2[1] = '\n';
			if (write(0, &buff2, 2) < 0)
				errorCheck();
		   }
		else{
		  if( write(0, &buff,1) < 0)
			errorCheck();
		}
	}

	reset_input_mode();
}


int main(int argc, char** argv)
{
int option_index = 0; 
int shellFlag = 0;
int rc = 1;
int fd1[2];
int fd2[2];
signal(SIGPIPE, signalHandler); 
int child_status;

static struct option long_options[] = {
{"shell", no_argument, NULL, 1},
{0, 0, 0, 0}
};

int c;
while(1){
	c = getopt_long(argc, argv, "", long_options, &option_index);
	if (c == -1)
	    break;
	switch(c){
	case 1:
	shellFlag = 1;
	break;
	default:
	fprintf(stderr, "Unrecognized argument. The options are: \n --shell\n");
	exit(1);	
	}
}

if (shellFlag)
{
	if (pipe(fd1) < 0)
	   errorCheck();
	if (pipe(fd2) < 0)
	   errorCheck();
	rc = fork();
	if (rc < 0)
	   errorCheck();
}
if (shellFlag & (rc == 0))
{
	close(fd1[1]);
	close(fd2[0]);
	close(0);
	dup(fd1[0]);
	close(1);
	dup(fd2[1]);
	close(2);
	dup(fd2[1]);
	char *myargs[2];
	myargs[0] = strdup("/bin/bash");
	myargs[1] = NULL;
	if(execvp(myargs[0], myargs) < 0)
		errorCheck();
}
else if (shellFlag)	
{
close(fd1[0]);
close(fd2[1]);
char buff;
char buff2[2];

struct termios currentAttr;

if(tcgetattr(0, &savedAttr) < 0)
	errorCheck();
if(tcgetattr(0, &currentAttr) < 0)
	errorCheck();

currentAttr.c_iflag = ISTRIP;
currentAttr.c_oflag = 0;
currentAttr.c_lflag = 0;

if(tcsetattr(0, TCSANOW, &currentAttr) < 0)
	errorCheck();

atexit(reset_input_mode);

struct pollfd fds[2];
fds[0].fd = 0;         //Pipe for standard input
fds[0].events = POLLIN | POLLHUP | POLLERR;
fds[1].fd = fd2[0];    //Pipe for input from shell
fds[1].events = POLLIN | POLLHUP | POLLERR;


while (1)
{
	if (ready_to_exit)
		break;

	int ret = poll(fds, 2, -1);

	if (ret < 0)
	   errorCheck();
   if(ret > 0)
    {
	if ((fds[0].revents & POLLIN)){  //read from stdin
		int num = read(0, &buff, 1);	
		if (num < 0)
		   errorCheck();
		if (buff == 0x04)
		{
			close(fd1[1]);
		}
		else if (buff == 0x03)
		{
			buff2[0] = '^';
			buff2[1] = 'C';
			if (write(1, &buff2, 2) < 0)
				errorCheck();
			
			if (kill(rc, SIGINT) < 0)
			    errorCheck();
		}
		else if (buff == '\r' || buff == '\n')
	        {
			buff2[0] = '\r';
			buff2[1] = '\n';
			if (write(1, &buff2, 2) < 0)
				errorCheck();
			
			buff = '\n';
			if (write(fd1[1], &buff, 1) < 0)
			   errorCheck();	
		}
		else
		{
		  if(write(1, &buff, 1) < 0)
			errorCheck();
		  if(write(fd1[1], &buff, 1) < 0)
		        errorCheck();
		}
	}
	if ((fds[0].revents & POLLHUP) | (fds[0].revents & POLLERR)){ //read from stderr
		ready_to_exit = 1;
		
	}
	if ((fds[1].revents & POLLIN))   //read from shell stdout
	{
		char buffer[256];
		int num = read(fd2[0], &buffer, 256);
		if (num < 0)
	     	   errorCheck();

		for (int i = 0; i < num; i++)
		{
		if (buffer[i] == EOF)
		{
			ready_to_exit = 1;
		}
		else if (buffer[i] == '\n')
		{
			buff2[0] = '\r';
			buff2[1] = '\n';
			if (write(1, &buff2, 2) < 0)
				errorCheck();
		}
		else{
			if (write(1, &buffer[i], 1) < 0)
			    errorCheck();	
	  	    }	
		}
		
	}
	if ((fds[1].revents & POLLHUP) | (fds[1].revents & POLLERR)) //read from shell stderr
	{
		   ready_to_exit = 1;
		
	}
    }
}
	close(fd2[0]);

	if (waitpid(rc, &child_status, 0) < 0)
	   errorCheck();

	int signal_code = child_status & 0x7f;
	int status_code = WEXITSTATUS(child_status);
	fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d", signal_code, status_code);

	reset_input_mode();

}
else
	nonShellIO();	

exit(0);
}

