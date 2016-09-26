#CS551 shell

# Running and Testing
## To build: Run `make` in this directory.
## To run testcases: Run `make tests` in this directory
## To run the shell (for normal use): Run `./tsh`


## State of functionality

* Testcases - 100%
* fork / exec - 100%
* PROFILE execution - 100%
* Aliases 100%
* Command parsing - 100%
* PATH walking - 100%
* Getting and setting woking directory - 100%
* Signal handlers - 100%
* Exit on command or Control-c - 100%

##Files:

* Makefile	# Compiles the shell program and runs the tests
* README.md		# This file
* .aliases 	# File that stores persistent aliases
* .profile	# Defines home directory where shell starts in

## The remaining files are used to test the shell
* tests.sh
* test_*.txt

### Little C programs that are called by the test cases
* myspin.c	# Takes argument <n> and spins for <n> seconds
* mysplit.c	# Forks a child that spins for <n> seconds
* mystop.c        # Spins for <n> seconds and sends SIGTSTP to itself
* myint.c         # Spins for <n> seconds and sends SIGINT to itself
* test_exit.c	# Runs the tsh shell as a child process and sends it 2 sigint signals, 2 seconds apart.
* test_multiint.c # Runs the tsh shell as a child and sends it a sigint followed by a sigquit signal.
* test_print<n>.c # Prints the number N to the terminal.
