#!/bin/bash
set -e 
echo -n Testing Aliases...
cat test.txt | ./tsh | diff - expect.txt
echo Success
