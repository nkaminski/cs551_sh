#!/bin/bash
#!/usr/pkg/bin/bash
#set -e
#pdir=$(pwd)
nfails=0

#Test .PROFILE
echo ---Testing Shell Functionality---
echo -n .PROFILE execution...
./tsh < test_profile.txt | grep -q '/usr >'
if [ $? -eq 0 ] 
then
	echo Success
else
	echo Failure
	let "nfails++"
fi

#PERROR does not redirect with pipes?? still prints to screen
# -> -> Nash: perror() prints to stderr, use '2>&1' to redirect stderr to
# stdout

echo -n Invalid .PROFILE PATH...
mv ./.profile ./.profiletemp
mv ./.profiletest ./.profile
./tsh < test_profile.txt 2>&1 >/dev/null | grep -q 'Error'
if [ $? -eq 0 ] 
then
	echo Success
else 
	echo Failure
	let "nfails++"
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
	let "nfails++"
else 
	echo "Success"
fi
#How to implement if
echo -n Exiting with ctrl-c...
./tsh &
PID=$!
kill -INT $PID 
kill -INT $PID
if pgrep "tsh" > /dev/null
then 
	echo Failure
	let "nfails++"
else
	echo Success
fi

echo -n Exit with ctrl-c confirmation...
./test_exit > test.txt
grep 'Are you sure?' test.txt  > /dev/null
if [ $? -eq 0 ]
then
	echo Success
else 
	echo Failure
	let "nfails++"
fi
rm test.txt
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
	let "nfails++"
fi

#Add cmds in /bin and /usr/bin within MINIX
echo -n Executing commands in PATH...
./tsh < test_cmds.txt | diff - test_cmds2.txt
if [ $? -eq 0 ]
then
	echo Success
else 
	echo Failure
	let "nfails++"
fi

echo -n Command in specified directory...
./tsh < test_cmds3.txt > test.txt
./tsh < test_cmds4.txt | diff - test.txt
if [ $? -eq 0 ]
then
	echo Success
else 
	echo Failure
	let "nfails++"
fi
rm test.txt

#How do you get 'command not found'?
echo -n Command not on path...
./tsh <  test_invalidcmd.txt | grep -q 'does not exist'
if [ $? -eq 0 ]
then
	echo Success
else 
	echo Failure
	let "nfails++"
fi

echo -n Command not in specified directory...
./tsh < test_invalidpath.txt | grep -q 'Command not found'
if [ $? -eq 0 ]
then
	echo Success
else 
	echo Failure
	let "nfails++"
fi

echo -n Sequential operation 1...
mv ./.profile ./.profiletemp
pwd > ./.profile
./tsh < test_parens2.txt | grep -q '21' 
if [ $? -eq 0 ]
then
	echo Success
else 
	echo Failure
	let "nfails++"
fi
rm ./.profile
mv ./.profiletemp ./.profile

echo Parallel operation 1, check 3 functions...
mv ./.profile ./.profiletemp
pwd > ./.profile
./tsh < test_parens1.txt 
if pgrep "./myspin 5" > /dev/null
then 
	echo Success
else
	echo Failure
	let "nfails++"
fi
if pgrep "./myspin 4" > /dev/null
then 
	echo Success
else
	echo Failure
	let "nfails++"
fi
if pgrep "./myspin 3" > /dev/null
then 
	echo Success
else
	echo Failure
	let "nfails++"
fi
rm ./.profile
mv ./.profiletemp ./.profile
echo Failed tests: $nfails 
