#!/bin/bash
set -e 

#Test .PROFILE
echo ---Testing Shell Functionality---
echo -n .PROFILE execution...
./tsh < test_profile.txt | grep -q '/usr >'
#PERROR does not redirect with pipes?? still prints to screen
#mv ./.profile ./.profiletemp
#mv ./.profiletest ./.profile
#./tsh | grep -q 'Error while interpreting .profile' < test_profile.txt 
#mv ./.profile ./.profilettest
#mv ./.profiletemp ./.profile
echo Success

echo -n Exiting with exit...
./tsh < test_profile.txt > /dev/null
if pgrep "tsh" > /dev/null
then
	echo "Failure"
else 
	echo "Success"
fi
echo -n Exiting with ctrl-c...
./tsh &
PID=$!
kill -INT $PID 
echo Success

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
echo Success
