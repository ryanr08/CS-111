#NAME: Ryan Riahi
#EMAIL: ryanr08@gmail.com
#ID: 105138860

default:
	gcc lab2_add.c -o lab2_add -pthread -Wall -Wextra -lrt
	gcc lab2_list.c -o lab2_list -pthread -lrt -Wall -Wextra SortedList.c

graphs: tests
	./lab2_add.gp
	./lab2_list.gp

clean:
	rm -f lab2_add lab2_list *.tar.gz *.o

dist: graphs
	tar -czvf lab2a-105138860.tar.gz lab2_add.c lab2_list.c SortedList.h SortedList.c lab2_add.csv lab2_list.csv lab2_add.gp lab2_add-1.png  lab2_add-2.png lab2_add-3.png lab2_add-4.png lab2_add-5.png lab2_list.gp lab2_list-1.png lab2_list-2.png lab2_list-3.png lab2_list-4.png Makefile README

tests: default
	
	for i in 10 100 1000 10000 100000; do \
		./lab2_add --threads=1 --iterations=$$i >> lab2_add.csv; \
	done \

	for i in 10 100 1000 10000 100000; do \
		./lab2_add --threads=1 --iterations=$$i --yield >> lab2_add.csv; \
	done \
	
	for i in 2 4 8 12 ; do \
		for j in 10 20 40 80 100 1000 10000 100000 ; do \
			./lab2_add --threads=$$i --iterations=$$j >> lab2_add.csv; \
		done \
	done \

	for i in 2 4 8 12 ; do \
		for j in 10 20 40 80 100 1000 10000 100000 ; do \
			./lab2_add --threads=$$i --iterations=$$j --yield >> lab2_add.csv; \
		done \
	done \

	for i in 2 4 8 12 ; do \
		./lab2_add --threads=$$i --iterations=10000 --yield --sync=m >> lab2_add.csv; \
	done \

	for i in 2 4 8 12 ; do \
		./lab2_add --threads=$$i --iterations=10000 --yield --sync=c >> lab2_add.csv; \
	done \

	for i in 2 4 8 12 ; do \
		./lab2_add --threads=$$i --iterations=1000 --yield --sync=s >> lab2_add.csv; \
	done \

	for i in 1 2 4 8 ; do \
		./lab2_add --threads=$$i --iterations=10000 >> lab2_add.csv; \
	done \

	for i in 1 2 4 8 12 ; do \
		./lab2_add --threads=$$i --iterations=10000 --sync=m >> lab2_add.csv; \
	done \

	for i in 1 2 4 8 12 ; do \
		./lab2_add --threads=$$i --iterations=10000 --sync=s >> lab2_add.csv; \
	done \

	for i in 1 2 4 8 12 ; do \
		./lab2_add --threads=$$i --iterations=10000 --sync=c >> lab2_add.csv; \
	done \


	for i in 10 100 1000 10000 20000 ; do \
		./lab2_list --threads=1 --iterations=$$i >> lab2_list.csv; \
	done \

	-./lab2_list --threads=2 --iterations=1 >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=10 >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=100 >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=1000 >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=1 >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=10 >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=100 >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=1000 >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=1 >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=10 >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=100 >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=1000 >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1 >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=10 >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=100 >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1000 >> lab2_list.csv

	-./lab2_list --threads=2 --iterations=1 --yield=d >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=2 --yield=d >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=4 --yield=d >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=8 --yield=d >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=16 --yield=d >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=32 --yield=d >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=1 --yield=d >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=2 --yield=d >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=4 --yield=d >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=8 --yield=d >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=16 --yield=d >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=32 --yield=d >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=1 --yield=d >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=2 --yield=d >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=4 --yield=d >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=8 --yield=d >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=16 --yield=d >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=32 --yield=d >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1 --yield=d >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2 --yield=d >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4 --yield=d >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8 --yield=d >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16 --yield=d >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=32 --yield=d >> lab2_list.csv

	-./lab2_list --threads=2 --iterations=1 --yield=i >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=2 --yield=i >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=4 --yield=i >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=8 --yield=i >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=16 --yield=i >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=32 --yield=i >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=1 --yield=i >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=2 --yield=i >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=4 --yield=i >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=8 --yield=i >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=16 --yield=i >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=32 --yield=i >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=1 --yield=i >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=2 --yield=i >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=4 --yield=i >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=8 --yield=i >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=16 --yield=i >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=32 --yield=i >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1 --yield=i >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2 --yield=i >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4 --yield=i >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8 --yield=i >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16 --yield=i >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=32 --yield=i >> lab2_list.csv

	-./lab2_list --threads=2 --iterations=1 --yield=il >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=2 --yield=il >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=4 --yield=il >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=8 --yield=il >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=16 --yield=il >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=32 --yield=il >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=1 --yield=il >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=2 --yield=il >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=4 --yield=il >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=8 --yield=il >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=16 --yield=il >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=32 --yield=il >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=1 --yield=il >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=2 --yield=il >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=4 --yield=il >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=8 --yield=il >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=16 --yield=il >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=32 --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1 --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2 --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4 --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8 --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16 --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=32 --yield=il >> lab2_list.csv

	-./lab2_list --threads=2 --iterations=1 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=2 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=4 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=8 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=16 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=32 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=1 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=2 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=4 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=8 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=16 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=32 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=1 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=2 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=4 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=8 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=16 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=32 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=32 --yield=dl >> lab2_list.csv


	-./lab2_list --threads=1 --iterations=1000 --sync=m >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=1000 --sync=m >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=1000 --sync=m >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=1000 --sync=m >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1000 --sync=m >> lab2_list.csv
	-./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2_list.csv
	-./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2_list.csv

	-./lab2_list --threads=1 --iterations=1000 --sync=s >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=1000 --sync=s >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=1000 --sync=s >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=1000 --sync=s >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1000 --sync=s >> lab2_list.csv
	-./lab2_list --threads=16 --iterations=1000 --sync=s >> lab2_list.csv
	-./lab2_list --threads=24 --iterations=1000 --sync=s >> lab2_list.csv

	./lab2_list --threads=12 --iterations=32 --yield=i --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=d --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=16 --yield=i --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=16 --yield=d --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=16 --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=16 --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=4 --iterations=32 --yield=i --sync=m >> lab2_list.csv
	./lab2_list --threads=4 --iterations=32 --yield=d --sync=m >> lab2_list.csv
	./lab2_list --threads=4 --iterations=32 --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=4 --iterations=32 --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=4 --iterations=16 --yield=i --sync=m >> lab2_list.csv
	./lab2_list --threads=4 --iterations=16 --yield=d --sync=m >> lab2_list.csv
	./lab2_list --threads=4 --iterations=16 --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=4 --iterations=16 --yield=il --sync=m >> lab2_list.csv

	./lab2_list --threads=12 --iterations=32 --yield=i --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=d --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=16 --yield=i --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=16 --yield=d --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=16 --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=16 --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=4 --iterations=32 --yield=i --sync=s >> lab2_list.csv
	./lab2_list --threads=4 --iterations=32 --yield=d --sync=s >> lab2_list.csv
	./lab2_list --threads=4 --iterations=32 --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=4 --iterations=32 --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=4 --iterations=16 --yield=i --sync=s >> lab2_list.csv
	./lab2_list --threads=4 --iterations=16 --yield=d --sync=s >> lab2_list.csv
	./lab2_list --threads=4 --iterations=16 --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=4 --iterations=16 --yield=il --sync=s >> lab2_list.csv