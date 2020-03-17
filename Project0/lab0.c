//NAME: Ryan Riahi
//EMAIL: ryanr08@gmail.com
//ID: 105138860

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
void handler();


int main(int argc, char** argv){
char* inputfile = NULL;
char* outputfile = NULL;
int segflag = 0;
int catchflag = 0;
int option_index = 0;

static struct option long_options[] = {
{"input", required_argument, NULL, 1},
{"output", required_argument, NULL, 2},
{"segfault", no_argument, NULL, 3},
{"catch", no_argument, NULL, 4},
{0, 0, 0, 0}
};

int c;
while (1){
c = getopt_long(argc, argv, "", long_options, &option_index);
if (c == -1)
    break;

switch(c){
    case 1:
	inputfile = optarg;
	break;
    case 2:
	outputfile = optarg;
	break;
    case 3:
	segflag = 1;
	break;
    case 4:
	catchflag = 1;
	break;
    default:
	fprintf(stderr, "getopt returned character code %o \nThe correct options are \n--input=filename \n--output=filename \n--segfault \n--catch\n", c);
	exit(1);

}
}

int fd0 = 0;
int fd1 = 1;

if (inputfile != NULL)
{
	fd0 = open(inputfile, O_RDONLY);
	if (fd0 == -1){
	fprintf(stderr, "%s \n", strerror(errno));
	exit(2);
	}
	close(0);
	dup(fd0);	
}

if (outputfile != NULL)
{
	fd1 = creat(outputfile, 00777);
	if (fd1 == -1){
	fprintf(stderr, "%s \n", strerror(errno));
	exit(3);
	}
	close(1);
	dup(fd1);
}

if (catchflag)
{
	signal(SIGSEGV, handler);
}

if (segflag)
{
	char* cp = NULL;
	*cp = 'a';
}

char ch;
while (read(fd0, &ch, 1) > 0){
	write(fd1, &ch, 1);
}

close(fd0);
close(fd1);
exit(0);
}

void handler(){
	fprintf(stderr, "Segmentation fault occurred! Exiting program.\n");
	exit(4);
}
