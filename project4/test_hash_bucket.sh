#!/bin/sh
sed '1c #define LOCK_TYPE 0' lock.h
gcc -g -Wall -o test_hash_bucket test_hash_bucket.c -lpthread
./test_hash>output_hash_lock0
echo you can check the result in file output_hash_lock0
cat output_hash_lock0


sed '1c #define LOCK_TYPE 1' lock.h
gcc -g -Wall -o test_hash_bucket test_hash_bucket.c -lpthread
./test_hash>output_hash_lock1
echo you can check the result in file output_hash_lock1
cat output_hash_lock1
 

sed '1c #define LOCK_TYPE 2' lock.h
gcc -g -Wall -o test_hash_bucket test_hash_bucket.c -lpthread
./test_hash>output_hash_lock2
echo you can check the result in file output_hash_lock2
cat output_hash_lock2

sed '1c #define LOCK_TYPE 3' lock.h
gcc -g -Wall -o test_hash_bucket test_hash_bucket.c -lpthread
./test_hash>output_hash_lock3
echo you can check the result in file output_hash_lock3
cat output_hash_lock3
