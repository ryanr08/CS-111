#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <getopt.h>
#include <termios.h>
#include <poll.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "zlib.h"

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void errorCheck(void)
{
    fprintf(stderr, "%s, \n", strerror(errno));
    exit(1);
}

struct termios savedAttr;
int ready_to_exit = 0;

void reset_input_mode(void)
{
    if (tcsetattr(0, TCSANOW, &savedAttr) < 0)
        errorCheck();
}

int main(int argc, char *argv[])
{
    int sockfd;
    int portno = 0;
    int logOption = 0;
    int compressOption = 0;
    char *filename = NULL;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    server = gethostbyname("localhost");
    z_stream toServer;
    z_stream fromServer;

    static struct option long_options[] = {
        {"port", required_argument, NULL, 1},
        {"log", required_argument, NULL, 2},
        {"host", required_argument, NULL, 3},
        {"compress", no_argument, NULL, 4},
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
            logOption = 1;
            filename = optarg;
            break;
        case 3:
            server = gethostbyname(optarg);
            break;
        case 4:
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
        toServer.zalloc = NULL;
        toServer.zfree = NULL;
        toServer.opaque = NULL;

        fromServer.zalloc = NULL;
        fromServer.zfree = NULL;
        fromServer.opaque = NULL;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    struct termios currentAttr;

    if (tcgetattr(0, &savedAttr) < 0)
        errorCheck();
    if (tcgetattr(0, &currentAttr) < 0)
        errorCheck();

    currentAttr.c_iflag = ISTRIP;
    currentAttr.c_oflag = 0;
    currentAttr.c_lflag = 0;

    if (tcsetattr(0, TCSANOW, &currentAttr) < 0)
        errorCheck();

    atexit(reset_input_mode);

    struct pollfd fds[2];
    fds[0].fd = 0; //Pipe for standard input
    fds[0].events = POLLIN | POLLHUP | POLLERR;
    fds[1].fd = sockfd; //Pipe for input from server
    fds[1].events = POLLIN | POLLHUP | POLLERR;


    int logfd;
    if (logOption)
    {
        logfd = open(filename, O_TRUNC | O_CREAT | O_RDWR, S_IRWXU);
        if (logfd < 0)
            error("ERROR opening file given by --log");
    }

    while (1)
    {

        if (ready_to_exit)
            break;

        int ret = poll(fds, 2, 0);

        if (ret < 0)
            errorCheck();
        if (ret > 0)
        {
            if ((fds[0].revents & POLLIN)) //read from stdin
            {
                char buffer[15];
                int num = read(0, &buffer, 15);
                if (num < 0)
                    errorCheck();
                for (int i = 0; i < num; i++)
                {
                    if (buffer[i] == '\r' || buffer[i] == '\n')
                    {
                        if (write(1, "\r\n", 2) < 0)
                            errorCheck();
                    }
                    else
                    {
                        if (write(1, &buffer[i], 1) < 0)
                            errorCheck();
                    }
                }
                if (compressOption)
                {
                    unsigned char compressOutput[15];
                    toServer.avail_in = (uInt)num;
                    toServer.next_in = (Bytef *)buffer;
                    toServer.avail_out = (uInt)sizeof(compressOutput);
                    toServer.next_out = (Bytef *)compressOutput;

                    if (deflateInit(&toServer, Z_SYNC_FLUSH) != Z_OK)
                        error("Error initializing compression");
                    if (deflate(&toServer, Z_SYNC_FLUSH) != Z_OK)
                        error("Error calling deflate");                   
                    deflateEnd(&toServer);
                    num = toServer.total_out;
                    for (int x = 0; x < num; x++)
                        buffer[x] = compressOutput[x];
                }
                if (write(sockfd, &buffer, num) < 0)
                    errorCheck();
                if (logOption && num != 0)
                {
                    char numBuff[5] = {'\0'};
                    sprintf(numBuff, "%d", num);
                    if (write(logfd, "SENT ", 5) < 0)
                        error("Error writing to log");
                    if (write(logfd, &numBuff, strlen(numBuff)) < 0)
                        error("Error writing to log");
                    if (write(logfd, " bytes: ", 8) < 0)
                        error("Error writing to log");
                    if (write(logfd, &buffer, num) < 0)
                        error("Error writing to log");
                    if (write(logfd, "\n", 1) < 0)
                        error("Error writing to log");
                }
            }

            if ((fds[0].revents & POLLHUP) | (fds[0].revents & POLLERR)) //read from stderr
            {
                ready_to_exit = 1;
            }

            if ((fds[1].revents & POLLIN)) //read from socket
            {
                char buffer[256];
                int num = read(sockfd, &buffer, 256);
                if (num < 0)
                    errorCheck();
                if (num == 0)
                    ready_to_exit = 1;
                
                if (logOption && num != 0)
                {
                    char numBuff[5] = {'\0'};
                    sprintf(numBuff, "%d", num);
                    if (write(logfd, "RECEIVED ", 9) < 0)
                        error("Error writing to log");
                    if (write(logfd, &numBuff, strlen(numBuff)) < 0)
                        error("Error writing to log");
                    if (write(logfd, " bytes: ", 8) < 0)
                        error("Error writing to log");
                    if (write(logfd, &buffer, num) < 0)
                        error("Error writing to log");
                    if (write(logfd, "\n", 1) < 0)
                        error("Error writing to log");
                }

                if (compressOption && num != 0)
                {
                    char decompressedOutput[256];
                    fromServer.avail_in = (uInt)num;
                    fromServer.next_in = (Bytef *)buffer;
                    fromServer.avail_out = (uInt)(256);
                    fromServer.next_out = (Bytef *)decompressedOutput;
                    if (inflateInit(&fromServer) != Z_OK)
                        error("Error initializing Decompression");
                    if (inflate(&fromServer, Z_SYNC_FLUSH) != Z_OK)
                        error("Error calling inflate");
                    if (inflateEnd(&fromServer) != Z_OK)
                        error("Error ending inflation");
                    num = fromServer.total_out;
                    for (int x = 0; x < num; x++)
                        buffer[x] = decompressedOutput[x];
                }

                for (int i = 0; i < num; i++)
                {
                    if (buffer[i] == '\n')
                    {
                        if (write(1, "\r\n", 2) < 0)
                            errorCheck();
                    }
                    else
                    {
                        if (write(1, &buffer[i], 1) < 0)
                            errorCheck();
                    }
                }
            }
            if ((fds[1].revents & POLLHUP) | (fds[1].revents & POLLERR)) //read from socket stderr
            {
                ready_to_exit = 1;
            }
        }
    }
    if (logOption)
        close(logfd);

    reset_input_mode();
    exit(0);
}