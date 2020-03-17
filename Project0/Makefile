#NAME: Ryan Riahi
#EMAIL: ryanr08@gmail.com
#ID: 105138860

lab0: lab0.c 
	gcc -g lab0.c -o lab0 -Wextra -Wall
clean:
	rm -rf lab0 out.txt file.txt lab0-105138860.tar.gz
check: lab0
	./lab0 --invalidOption;  \
	if [ $$? == 1 ];           \
	then \
		echo "Passed test 1";  \
	else \
		echo "Failed test 1";  \
	fi \
	
	./lab0 --segfault --catch; \
	if [ $$? == 4 ];  \
	then \
		echo "Passed test 2"; \
	else \
		echo "Failed test 2"; \
	fi \
	
	./lab0 --input=notafile; \
	if [ $$? == 2 ]; \
	then \
		echo "Passed test 3"; \
	else \
		echo "Failed test 3"; \
	fi \
	
	echo "Hello World" > file.txt; \
	chmod u-rw file.txt; \
	./lab0 --output=file.txt; \
	if [ $$? == 3 ];  \
	then \
		echo "Passed test 4"; \
	else  \
		echo "Failed test 4"; \
	fi  
	
	./lab0 --input=in.txt --output=out.txt; \
	if [ $$? == 0 ]; \
	then \
		if cmp --s out.txt in.txt; \
		then \
			echo "Output is equal to Input! All tests passed."; \
		else \
			ech "Output is not equal to input! Last test failed."; \
		fi \
	else \
		echo "Script did not exit with code 0"; \
	fi
dist: lab0
	tar -zcf lab0-105138860.tar.gz lab0.c Makefile README backtrace.png breakpoint.png 
