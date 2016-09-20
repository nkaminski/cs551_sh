#CS551 shell

## State of functionality

* Testcases - 0%
* fork / exec - 100%
* PROFILE execution - 100%
* Aliases 60%
* Command parsing - 75% (all except parenthesis)
* PATH walking - 100%
* Getting and setting woking directory - 100%
* Signal handlers - 100%
* Exit on command or Control-c - 100%

#Files:

Makefile	# Compiles your shell program and runs the tests
README		# This file
tsh.c		# The shell program that you will write and hand in
tshref		# The reference shell binary.

## The remaining files are used to test your shell
sdriver.pl	# The trace-driven shell driver
trace*.txt	# The 15 trace files that control the shell driver
tshref.out 	# Example output of the reference shell on all 15 traces

## Little C programs that are called by the trace files
myspin.c	# Takes argument <n> and spins for <n> seconds
mysplit.c	# Forks a child that spins for <n> seconds
mystop.c        # Spins for <n> seconds and sends SIGTSTP to itself
myint.c         # Spins for <n> seconds and sends SIGINT to itself

