#!/bin/sh
sed '1c #define LOCK_TYPE 0' lock.h
gcc -g -Wall -o test_counter test_counter.c -lpthread
./test_counter>output_lock0
echo you can check the result in file output_lock0
cat output


sed '1c #define LOCK_TYPE 1' lock.h
gcc -g -Wall -o test_counter test_counter.c -lpthread
./test_counter>output_lcok1
echo you can check the result in file output_lock1
cat output
 

sed '1c #define LOCK_TYPE 2' lock.h
gcc -g -Wall -o test_counter test_counter.c -lpthread
./test_counter>output_lock2
echo you can check the result in file output_lock2
cat output

sed '1c #define LOCK_TYPE 3' lock.h
gcc -g -Wall -o test_counter test_counter.c -lpthread
./test_counter>output_lock3
echo you can check the result in file output_lock3
cat output
