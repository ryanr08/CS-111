#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <getopt.h>
#include <termios.h>
#include <poll.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include "zlib.h"

void error(char *msg)
{
    perror(msg);
    exit(1);
}
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

z_stream fromClient;
z_stream toClient;

int main(int argc, char *argv[])
{
    int sockfd, newsockfd;
    unsigned int clilen;
    int portno = 0;
    int compressOption = 0;
    struct sockaddr_in serv_addr, cli_addr;
    int fd1[2];
    int fd2[2];
    signal(SIGPIPE, signalHandler);


    static struct option long_options[] = {
        {"port", required_argument, NULL, 1},
        {"compress", no_argument, NULL, 2},
        {0, 0, 0, 0}};

    int c;
    while (1)
    {
        c = getopt_long(argc, argv, "", long_options, NULL);
        if (c == -1)
            break;
        switch (c)
        {
        case 1:
            portno = atoi(optarg);
            break;
        case 2:
            compressOption = 1;
            break;
        default:
            fprintf(stderr, "Unrecognized argument. The options are: \n--port=#\n");
            exit(1);
        }
    }
    if (portno == 0)
    {
        fprintf(stderr, "ERROR --port=# is a required argument\n");
        exit(1);
    }

    if (compressOption)
    {
        fromClient.zalloc = NULL;
        fromClient.zfree = NULL;
        fromClient.opaque = NULL;

        toClient.zalloc = NULL;
        toClient.zfree = NULL;
        toClient.opaque = NULL;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");

    if (pipe(fd1) < 0)
        errorCheck();
    if (pipe(fd2) < 0)
        errorCheck();
    int rc = fork();
    if (rc < 0)
        errorCheck();

    if (rc == 0)
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
        if (execvp(myargs[0], myargs) < 0)
            errorCheck();
    }

    close(fd1[0]);
    close(fd2[1]);
    struct pollfd fds[2];
    fds[0].fd = newsockfd; //Pipe from client
    fds[0].events = POLLIN | POLLHUP | POLLERR;
    fds[1].fd = fd2[0]; //Pipe for input from shell
    fds[1].events = POLLIN | POLLHUP | POLLERR;

    char buff[25];

    while (1)
    {
        if (ready_to_exit)
            break;

        int ret = poll(fds, 2, 0);

        if (ret < 0)
            errorCheck();
        if (ret > 0)
        {
            if ((fds[0].revents & POLLIN)) //read from client
            { 
                int num = read(newsockfd, &buff, 10);
                if (num < 0)
                {
                    error("Error reading from client");
                }
                if (num == 0)
                    close(fd1[1]);
                if (compressOption)
                {
                    char decompressedOutput[25];
                    fromClient.avail_in = (uInt)num;
                    fromClient.next_in = (Bytef *)buff;
                    fromClient.avail_out = (uInt)(25);
                    fromClient.next_out = (Bytef *)decompressedOutput;
                    if (inflateInit(&fromClient) != Z_OK)
                        error("Error initializing Decompression");
                    if (inflate(&fromClient, Z_SYNC_FLUSH) != Z_OK)
                        error("Error calling inflate");
                    if (inflateEnd(&fromClient) != Z_OK)
                        error("Error ending inflation");
                    num = fromClient.total_out;
                    for (int x = 0; x < num; x++)
                        buff[x] = decompressedOutput[x];
                }
                for (int i = 0; i < num; i++){
                if (buff[i] == 0x04)
                {
                    close(fd1[1]);
                }
                else if (buff[i] == 0x03)
                {

                    if (kill(rc, SIGINT) < 0)
                        errorCheck();
                }
                else if (buff[i] == '\r' || buff[i] == '\n')
                {
                    
                    buff[i] = '\n';
                    if (write(fd1[1], &buff[i], num) < 0)
                        errorCheck();
                }
                else
                {
                    if (write(fd1[1], &buff[i], 1) < 0)
                        errorCheck();
                }
            }
            }
            if ((fds[0].revents & POLLHUP) | (fds[0].revents & POLLERR)) //read from socket stderr
            { 
                ready_to_exit = 1;
            }
            if ((fds[1].revents & POLLIN)) //read from shell stdout
            {
                char buffer[256];
                int num = read(fd2[0], &buffer, 256);
                if (num < 0)
                    errorCheck();
                if (num == 0)
                    ready_to_exit = 1;
                
                if (compressOption)
                {
                    char compressOutput[256];
                    toClient.avail_in = (uInt)num;
                    toClient.next_in = (Bytef *)buffer;
                    toClient.avail_out = (uInt)sizeof(compressOutput);
                    toClient.next_out = (Bytef *)compressOutput;

                    if (deflateInit(&toClient, Z_SYNC_FLUSH) != Z_OK)
                        error("Error initializing compression");
                    if (deflate(&toClient, Z_SYNC_FLUSH) != Z_OK)
                        error("Error calling deflate");
                    deflateEnd(&toClient);
                    num = toClient.total_out;
                    for (int x = 0; x < num; x++)
                        buffer[x] = compressOutput[x];
                }
                if (write(newsockfd, &buffer, num) < 0)
                    errorCheck();
                usleep(3000);
            }
            if ((fds[1].revents & POLLHUP) | (fds[1].revents & POLLERR)) //read from shell stderr
            {
                ready_to_exit = 1;
            }
        }
    }

    close(fd2[0]);
    int child_status;

	if (waitpid(rc, &child_status, 0) < 0)
	   errorCheck();

	int signal_code = child_status & 0x7f;
	int status_code = WEXITSTATUS(child_status);
	fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d", signal_code, status_code);

    exit(0);
}