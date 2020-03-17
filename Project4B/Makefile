#NAME: Ryan Riahi
#EMAIL: ryanr08@gmail.com
#ID: 105138860

default:
	gcc lab4b.c -o lab4b -lmraa -lm -Wall -Wextra -std=gnu99

clean:
	rm lab4b *.tar.gz

dist: default
	tar -czvf lab4b-105138860.tar.gz Makefile README lab4b.c testscript.sh

check: default
	./testscript.sh
	
