#!/bin/sh
echo -n Execution from MINIX ash shell...
./tsh < test_profile.txt | grep -q '/usr >'
if [ $? -eq 0 ] 
then
	echo Success
else
	echo Failure
fi
