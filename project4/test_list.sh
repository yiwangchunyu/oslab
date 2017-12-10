#!/bin/sh
sed '1c #define LOCK_TYPE 0' lock.h
gcc -g -Wall -o test_list test_list.c -lpthread
./test_list>output_list_lock0
echo you can check the result in file output_lock0
cat output_list_lock0


sed '1c #define LOCK_TYPE 1' lock.h
gcc -g -Wall -o test_list test_list.c -lpthread
./test_list>output_list_lock1
echo you can check the result in file output_lock1
cat output_list_lock1
 

sed '1c #define LOCK_TYPE 2' lock.h
gcc -g -Wall -o test_counter test_list.c -lpthread
./test_list>output_list_lock2
echo you can check the result in file output_lock2
cat output_list_lock2

sed '1c #define LOCK_TYPE 3' lock.h
gcc -g -Wall -o test_counter test_list.c -lpthread
./test_list>output_list_lock3
echo you can check the result in file output_lock3
cat output_list_lock3
