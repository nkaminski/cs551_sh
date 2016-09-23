#!/bin/ash
echo -n Minix execution...
./tsh < test_profile.txt | grep -q '/usr >'
if [ $? -eq 0 ] 
then
	echo Success
else
	echo Failure
fi
