#! /usr/bin/gnuplot
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2b_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#
# output:
#	lab2b_1.png -- Graph of the throughput of mutex and spin lock synchronized list operations
#
# Note:
#	Managing data is simplified by keeping all of the results in a single
#	file.  But this means that the individual graphing commands have to
#	grep to select only the data they want.
#
#	Early in your implementation, you will not have data for all of the
#	tests, and the later sections may generate errors for missing data.
#

# general plot parameters
set terminal png
set datafile separator ","



set title "Operations per Second vs Number of Threads for Spin and Mutex Locks"
set xlabel "Threads"
set logscale x 2
set ylabel "Operations per Second"
set logscale y 10
set output 'lab2b_1.png'

plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title 'Mutex' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title 'Spin' with linespoints lc rgb 'green'


set title "Average Time per Operations & Wait-for-Lock Time vs Number of Threads"
set xlabel "Threads"
set logscale x 2
set ylabel "Time per Operation/ Wait-for-Lock Time"
set logscale y 10
set output 'lab2b_2.png'

plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($7) \
	title 'Average Time per Operation' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($8) \
	title 'Wait-for-Lock Time' with linespoints lc rgb 'green'


set title "Succesful Iterations vs Number of Threads"
set xlabel "Threads"
set logscale x 2
set xrange [0.8:17]
set ylabel "Iterations"
set logscale y 10
set output 'lab2b_3.png'

plot \
     "< grep 'list-id-none,.*.*,4,' lab2b_list.csv" using ($2):($3) \
	title 'Unprotected with Yields' with points lc rgb 'blue', \
     "< grep 'list-id-m,.*.*,4,' lab2b_list.csv" using ($2):($3) \
	title 'Mutex Protected with Yields' with points lc rgb 'red', \
     "< grep 'list-id-s,.*.*,4,' lab2b_list.csv" using ($2):($3) \
	title 'Sync Protected with Yields' with points lc rgb 'green'



set title "Operations per Second vs Number of Threads for Mutex Locks with Multiple Lists"
set xlabel "Threads"
set logscale x 2
set ylabel "Operations per Second"
set logscale y 10
set output 'lab2b_4.png'

plot \
     "< grep 'list-none-m,.*,1000,1,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title '1 List' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,.*,1000,4,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title '4 Lists' with linespoints lc rgb 'green', \
     "< grep 'list-none-m,.*,1000,8,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title '8 Lists' with linespoints lc rgb 'orange', \
     "< grep 'list-none-m,.*,1000,16,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title '16 Lists' with linespoints lc rgb 'blue'


set title "Operations per Second vs Number of Threads for Sync Locks with Multiple Lists"
set xlabel "Threads"
set logscale x 2
set ylabel "Operations per Second"
set logscale y 10
set output 'lab2b_5.png'

plot \
     "< grep 'list-none-s,.*,1000,1,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title '1 List' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,.*,1000,4,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title '4 Lists' with linespoints lc rgb 'green', \
     "< grep 'list-none-s,.*,1000,8,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title '8 Lists' with linespoints lc rgb 'orange', \
     "< grep 'list-none-s,.*,1000,16,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title '16 Lists' with linespoints lc rgb 'blue'