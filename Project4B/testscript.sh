#NAME: Ryan Riahi
#EMAIL: ryanr08@gmail.com
#ID: 105138860
#!/bin/bash

./lab4b --scale=C --period=2 <<-EOF
SCALE=F
STOP
START
START
OFF
EOF

if [ $? -ne 0 ]
then
	echo "TEST FAILED: Incorrectly exited with wrong error code despite correct inputs"
else
	echo "TEST PASSED: Correctly exited with code 0 given proper inputs"
fi

./lab4b --badargument <<-EOF
EOF

if [ $? -eq 1 ]
then
	echo "TEST PASSED: Correctly detected bad argument"
else
	echo "TEST FAILED: Did not detect bad argument"
fi

./lab4b --log=logfile.txt <<-EOF
PERIOD=5
LOG hello
OFF
EOF

string2="LOG hello"
string=$(cat logfile.txt | grep "LOG hello")

if [ "$string" = "$string2" ]; then
	echo "TEST PASSED: Correctly logged to log file"
else
	echo "TEST FAILED: Did not correctly log to log file"
fi

rm -f logfile.txt
