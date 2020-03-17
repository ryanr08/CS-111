#NAME: Ryan Riahi
#EMAIL: ryanr08@gmail.com
#ID: 105138860

default:
	gcc lab2_list.c -o lab2_list -pthread -lrt -Wall -Wextra -g SortedList.c

graphs: tests
	./lab2_list.gp

profile:
	gcc lab2_list.c -o lab2_list -pthread -lrt -Wall -Wextra -g -lprofiler SortedList.c
	CPUPROFILE=profile.gperf ./lab2_list --threads=12 --iterations=1000 --sync=s
	pprof --text lab2_list profile.gperf > profile.out
	pprof --list=listRoutine ./lab2_list profile.gperf >> profile.out
	rm -f profile.gperf

clean:
	rm -f lab2_add lab2_list *.tar.gz *.o

dist: graphs
	tar -czvf lab2b-105138860.tar.gz lab2_list.c SortedList.h SortedList.c lab2b_list.csv lab2_list.gp profile.out Makefile README lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png

tests: default
	
	for i in 1 2 4 8 12 16 24; do \
		./lab2_list --threads=$$i --iterations=1000 --sync=m >> lab2b_list.csv; \
	done \

	for i in 1 2 4 8 12 16 24; do \
		./lab2_list --threads=$$i --iterations=1000 --sync=s >> lab2b_list.csv; \
	done \

	 for i in 1 4 8 12 16 ; do \
	 	for j in 10 20 40 80 ; do \
	 		./lab2_list --threads=$$i --iterations=$$j --yield=id --lists=4 --sync=m >> lab2b_list.csv; \
	 	done \
	 done \

	 for i in 1 4 8 12 16 ; do \
	 	for j in 10 20 40 80 ; do \
	 		./lab2_list --threads=$$i --iterations=$$j --yield=id --lists=4 --sync=s >> lab2b_list.csv; \
	 	done \
	 done \

	 for i in 1 2 4 8 12 ; do \
	 	for j in 4 8 16 ; do \
	 		./lab2_list --threads=$$i --iterations=1000 --lists=$$j --sync=s >> lab2b_list.csv; \
	 	done \
	 done \

	 for i in 1 2 4 8 12 ; do \
	 	for j in 4 8 16 ; do \
	 		./lab2_list --threads=$$i --iterations=1000 --lists=$$j --sync=m >> lab2b_list.csv; \
	 	done \
	 done \

	-./lab2_list --threads=1 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=1 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=1 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv