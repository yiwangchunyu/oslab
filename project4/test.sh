#!/bin/sh

gcc -g -Wall -o test test.c -lpthread
./test>output
echo you can check the result in file output
cat output

 
