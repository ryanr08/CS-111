#NAME: Ryan Riahi
#EMAIL: ryanr08@gmail.com
#ID: 105138860

default: lab1b-client lab1b-server

lab1b-client: lab1b-client.c 
	gcc -g lab1b-client.c -o lab1b-client -Wextra -Wall -lz

lab1b-server: lab1b-server.c 
	gcc -g lab1b-server.c -o lab1b-server -Wextra -Wall -lz

clean:
	rm -rf lab1b-server lab1b-client lab1b-105138860.tar.gz

dist: 
	tar -zcf lab1b-105138860.tar.gz lab1b-client.c lab1b-server.c Makefile README 
