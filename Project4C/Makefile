#NAME: Ryan Riahi
#EMAIL: ryanr08@gmail.com
#ID: 105138860

default:
	gcc lab4c_tcp.c -o lab4c_tcp -Wall -Wextra -lmraa -lm -std=gnu99
	gcc lab4c_tls.c -o lab4c_tls -Wall -Wextra -lmraa -lm -lssl -lcrypto -std=gnu99

clean:
	rm -f lab4c_tcp lab4c-105138860.tar.gz lab4c_tls

dist: default
	tar -czvf lab4c-105138860.tar.gz Makefile README lab4c_tcp.c lab4c_tls.c 
