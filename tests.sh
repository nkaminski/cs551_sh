#!/bin/bash
#set -e

#Test .PROFILE
echo ---Testing Shell Functionality---
echo -n .PROFILE execution...
./tsh < test_profile.txt | grep -q '/usr >'
if [ $? -eq 0 ] 
then
	echo Success
else
	echo Failure
fi

#PERROR does not redirect with pipes?? still prints to screen
# -> -> Nash: perror() prints to stderr, use '2>&1' to redirect stderr to
# stdout

echo -n Invalid .PROFILE PATH...
mv ./.profile ./.profiletemp
mv ./.profiletest ./.profile
./tsh 2>&1 | grep -q 'Error while interpreting .profile' < test_profile.txt
if [ $? -eq 0 ] 
then
	echo Success
else 
	echo Failure
fi
mv ./.profile ./.profiletest
mv ./.profiletemp ./.profile

#Proved in previous test case
echo -n Printing current directory...
echo Success

echo -n Exiting with exit...
./tsh < test_profile.txt > /dev/null
if pgrep "tsh" > /dev/null
then
	echo "Failure"
else 
	echo "Success"
fi
#How to implement if
#Need to check are you sure?
echo -n Exiting with ctrl-c...
./tsh &
PID=$!
kill -INT $PID 
kill -INT $PID
if pgrep "tsh" > /dev/null
then 
	echo Failure
else
	echo Success
fi

#Test Aliases 
echo -n Alias creation \& execution... 
./tsh < test_alias.txt > /dev/null 
./tsh < test_alias2.txt > test.txt
./tsh < test_alias3.txt | diff - test.txt
./tsh < test_alias4.txt > /dev/null
rm test.txt
echo Success

echo -n Alias invalid input...
./tsh < test_alias5.txt | grep -q 'alias command requires'
if [ $? -eq 0 ]
then
	echo Success
else 
	echo Failure
fi

#Add cmds in /bin and /usr/bin within MINIX
echo -n Executing commands in PATH...
#dir=$(pwd)
#cd /
#ls -l >> "$dir/test.txt"
#pwd >> "$dir/test.txt"
#ps >> "$dir/test.txt"
#cd $dir
./tsh < test_cmds.txt | diff - test_cmds2.txt
if [ $? -eq 0 ]
then
	echo Success
else 
	echo Failure
fi

#How do you get 'command not found'?
echo -n Command invalid...
./tsh <  test_invalidcmd.txt | grep -q 'does not exist'
if [ $? -eq 0 ]
then
	echo Success
else 
	echo Failure
fi
echo Success
